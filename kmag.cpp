/***************************************************************************
                          kmag.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sourceforge.net
    copyright            : (C) 2003-2004 by Olaf Schmidt
    email                : ojschmidt@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for QT
#include <QDir>
#include <QPrinter>
#include <QPainter>
#include <QLayout>
#include <QClipboard>
#include <q3dragobject.h>
#include <QToolTip>
#include <q3popupmenu.h>
#include <kxmlguiclient.h>
#include <ktoolbar.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kselectaction.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QPixmap>
#include <QDesktopWidget>
#include <kdeversion.h>
#include <kxmlguifactory.h>
#include <kicon.h>
// include files for KDE
#include <kapplication.h>
#include <kstandardshortcut.h>
#include <kshortcutsdialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstandardaction.h>
#include <khelpmenu.h>
#include <kimageio.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <ktemporaryfile.h>
#include <kmenu.h>
#include <kedittoolbar.h>

#include <kprinter.h>
#include <kglobal.h>

// application specific includes
#include "kmag.moc"
#include "kmagzoomview.h"
#include "kmagselrect.h"


#define havesetCheckedState

KmagApp::KmagApp(QWidget* , const char* name)
  : KXmlGuiWindow(0, Qt::WStyle_MinMax | Qt::WType_TopLevel | Qt::WDestructiveClose | Qt::WStyle_ContextHelp | Qt::WStyle_StaysOnTop),
    m_defaultMouseCursorType(2)
{
  setObjectName(name);
  config=KGlobal::config();

  zoomArrayString << "5:1" << "2:1" << "1:1" << "1:1.5" << "1:2" << "1:3" << "1:4" << "1:5"
    << "1:6" << "1:7" << "1:8" << "1:12" << "1:16" << "1:20";

  // Is there a better way to initialize a vector array?
  zoomArray.push_back(0.2); zoomArray.push_back(0.5); zoomArray.push_back(1.0);
  zoomArray.push_back(1.5); zoomArray.push_back(2.0); zoomArray.push_back(3.0);
  zoomArray.push_back(4.0); zoomArray.push_back(5.0); zoomArray.push_back(6.0); zoomArray.push_back(7.0);
  zoomArray.push_back(8.0); zoomArray.push_back(12.0); zoomArray.push_back(16.0); zoomArray.push_back(20.0);

  fpsArrayString << i18n("&Very Low") << i18n("&Low") << i18n("&Medium") << i18n("&High") << i18n("V&ery High");

  fpsArray.push_back(2); // very low
  fpsArray.push_back(6); // low
  fpsArray.push_back(10); // medium
  fpsArray.push_back(15); // high
  fpsArray.push_back(25); // very high

  rotationArrayString << i18n("&No Rotation (0 Degrees)") << i18n("&Left (90 Degrees)") << i18n("&Upside Down (180 Degrees)") << i18n("&Right (270 Degrees)");

  rotationArray.push_back(0); // no rotation
  rotationArray.push_back(90); // left
  rotationArray.push_back(180); // upside down
  rotationArray.push_back(270); // right

  // call inits to invoke all other construction parts
  initView();
  initActions();
  initConnections();

  // read options from config file
  readOptions();

}

/**
 * Default destructor.
 */
KmagApp::~KmagApp()
{
    m_zoomView->showSelRect(false);
}

void KmagApp::initActions()
{
  fileNewWindow = actionCollection()->addAction("new_window");
  fileNewWindow->setIcon(KIcon("window-new"));
  fileNewWindow->setText(i18n("New &Window"));
  connect(fileNewWindow, SIGNAL(triggered(bool) ), SLOT(slotFileNewWindow()));
  fileNewWindow->setShortcuts(KStandardShortcut::openNew());
  fileNewWindow->setToolTip(i18n("Open a new KMagnifier window"));

  refreshSwitch = actionCollection()->addAction("start_stop_refresh");
  refreshSwitch->setIcon(KIcon("process-stop"));
  refreshSwitch->setText(i18n("&Stop"));
  connect(refreshSwitch, SIGNAL(triggered(bool) ), SLOT(slotToggleRefresh()));
  refreshSwitch->setShortcuts(KStandardShortcut::reload());
  refreshSwitch->setToolTip(i18n("Click to stop window refresh"));
  refreshSwitch->setWhatsThis(i18n("Clicking on this icon will <b>start</b> / <b>stop</b>\
  updating of the display. Stopping the update will zero the processing power\
  required (CPU usage)"));

  m_pSnapshot = actionCollection()->addAction("snapshot");
  m_pSnapshot->setIcon(KIcon("ksnapshot"));
  m_pSnapshot->setText(i18n("&Save Snapshot As..."));
  connect(m_pSnapshot, SIGNAL(triggered(bool) ), SLOT(saveZoomPixmap()));
  m_pSnapshot->setShortcuts(KStandardShortcut::save());
  m_pSnapshot->setWhatsThis(i18n("Saves the zoomed view to an image file."));
  m_pSnapshot->setToolTip(i18n("Save image to a file"));

  m_pPrint = actionCollection()->addAction(KStandardAction::Print, this, SLOT(slotFilePrint()));
  m_pPrint->setWhatsThis(i18n("Click on this button to print the current zoomed view."));

  m_pQuit = actionCollection()->addAction(KStandardAction::Quit, this, SLOT(slotFileQuit()));
  m_pQuit->setToolTip(i18n("Quits the application"));
  m_pQuit->setWhatsThis (i18n("Quits the application"));

  m_pCopy = actionCollection()->addAction(KStandardAction::Copy, this, SLOT(copyToClipBoard()));
  m_pCopy->setWhatsThis(i18n("Click on this button to copy the current zoomed view to the clipboard which you can paste in other applications."));
  m_pCopy->setToolTip(i18n("Copy zoomed image to clipboard"));

  m_pShowMenu = new KToggleAction(KIcon("show-menu"), i18n("Show &Menu"), this);
  actionCollection()->addAction("show_menu", m_pShowMenu);
  connect(m_pShowMenu, SIGNAL(triggered(bool)), SLOT(slotShowMenu()));
  m_pShowMenu->setShortcut(Qt::CTRL+Qt::Key_M);
  #ifdef havesetCheckedState
  m_pShowMenu->setCheckedState(KGuiItem(i18n("Hide &Menu")));
  #endif
  m_pShowMainToolBar = new KToggleAction(i18n("Show Main &Toolbar"), this);
  actionCollection()->addAction("show_mainToolBar", m_pShowMainToolBar);
  connect(m_pShowMainToolBar, SIGNAL(triggered(bool)), SLOT(slotShowMainToolBar()));
  #ifdef havesetCheckedState
  m_pShowMainToolBar->setCheckedState(KGuiItem(i18n("Hide Main &Toolbar")));
  #endif
  m_pShowViewToolBar = new KToggleAction(i18n("Show &View Toolbar"), this);
  actionCollection()->addAction("show_viewToolBar", m_pShowViewToolBar);
  connect(m_pShowViewToolBar, SIGNAL(triggered(bool)), SLOT(slotShowViewToolBar()));
  #ifdef havesetCheckedState
  m_pShowViewToolBar->setCheckedState(KGuiItem(i18n("Hide &View Toolbar")));
  #endif
  m_pShowSettingsToolBar = new KToggleAction(i18n("Show &Settings Toolbar"), this);
  actionCollection()->addAction("show_settingsToolBar", m_pShowSettingsToolBar);
  connect(m_pShowSettingsToolBar, SIGNAL(triggered(bool)), SLOT(slotShowSettingsToolBar()));
  #ifdef havesetCheckedState
  m_pShowSettingsToolBar->setCheckedState(KGuiItem(i18n("Hide &Settings Toolbar")));
  #endif

  m_modeFollowMouse = new KToggleAction(KIcon("followmouse"), i18n("&Follow Mouse Mode"), this);
  actionCollection()->addAction("mode_followmouse", m_modeFollowMouse);
  connect(m_modeFollowMouse, SIGNAL(triggered(bool)), SLOT(slotModeFollowMouse()));
  m_modeFollowMouse->setShortcut(Qt::Key_F1);
  m_modeFollowMouse->setToolTip(i18n("Magnify around the mouse cursor"));
  m_modeFollowMouse->setWhatsThis(i18n("If selected, the area around the mouse cursor is magnified"));

  m_modeSelWin = new KToggleAction(KIcon("window"), i18n("Se&lection Window Mode"), this);
  actionCollection()->addAction("mode_selectionwindow", m_modeSelWin);
  connect(m_modeSelWin, SIGNAL(triggered(bool)), SLOT(slotModeSelWin()));
  m_modeSelWin->setShortcut(Qt::Key_F2);
  m_modeSelWin->setToolTip(i18n("Show a window for selecting the magnified area"));

  m_modeWholeScreen = new KToggleAction(KIcon("view-fullscreen"), i18n("&Whole Screen Mode"), this);
  actionCollection()->addAction("mode_wholescreen", m_modeWholeScreen);
  connect(m_modeWholeScreen, SIGNAL(triggered(bool)), SLOT(slotModeWholeScreen()));
  m_modeWholeScreen->setShortcut(Qt::Key_F3);
  m_modeWholeScreen->setToolTip(i18n("Magnify the whole screen"));
  m_modeWholeScreen->setWhatsThis(i18n("Click on this button to fit the zoom view to the zoom window."));

  m_hideCursor = new KToggleAction(KIcon("hidemouse"), i18n("Hide Mouse &Cursor"), this);
  actionCollection()->addAction("hidecursor", m_hideCursor);
  connect(m_hideCursor, SIGNAL(triggered(bool)), SLOT(slotToggleHideCursor()));
  m_hideCursor->setShortcut(Qt::Key_F4);
  #ifdef havesetCheckedState
  m_hideCursor->setCheckedState(KGuiItem(i18n("Show Mouse &Cursor")));
  #endif
  m_hideCursor->setToolTip(i18n("Hide the mouse cursor"));

  m_pZoomIn = actionCollection()->addAction(KStandardAction::ZoomIn, this, SLOT(zoomIn()));
  m_pZoomIn->setWhatsThis(i18n("Click on this button to <b>zoom-in</b> on the selected region."));

  m_pZoomBox = new KSelectAction(i18n("&Zoom"), this);
  actionCollection()->addAction("zoom", m_pZoomBox);
  m_pZoomBox->setItems(zoomArrayString);
  m_pZoomBox->setWhatsThis(i18n("Select the zoom factor."));
  m_pZoomBox->setToolTip(i18n("Zoom factor"));

  m_pZoomOut = actionCollection()->addAction(KStandardAction::ZoomOut, this, SLOT(zoomOut()));
  m_pZoomOut->setWhatsThis(i18n("Click on this button to <b>zoom-out</b> on the selected region."));

  m_pRotationBox = new KSelectAction(i18n("&Rotation"),this);
  actionCollection()->addAction("rotation", m_pRotationBox);
  m_pRotationBox->setItems(rotationArrayString);
  m_pRotationBox->setWhatsThis(i18n("Select the rotation degree."));
  m_pRotationBox->setToolTip(i18n("Rotation degree"));

  // KHelpMenu *newHelpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData());

  m_keyConf = actionCollection()->addAction(KStandardAction::KeyBindings, this, SLOT(slotConfKeys()));
  m_toolConf = actionCollection()->addAction(KStandardAction::ConfigureToolbars, this, SLOT(slotEditToolbars()));

  m_pFPSBox = new KSelectAction(i18n("&Refresh"),this);
  actionCollection()->addAction("fps_selector", m_pFPSBox);
  m_pFPSBox->setItems(fpsArrayString);
  m_pFPSBox->setWhatsThis(i18n("Select the refresh rate. The higher the rate, the more computing power (CPU) will be needed."));
  m_pFPSBox->setToolTip(i18n("Refresh rate"));

  createGUI();
}

void KmagApp::initView()
{
  m_zoomView = new KMagZoomView( this, "ZoomView" );
  m_zoomView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, m_zoomView->sizePolicy().hasHeightForWidth() ) );
  m_zoomView->setFrameShape( QFrame::StyledPanel );
  m_zoomView->setFrameShadow( QFrame::Raised );

  setCentralWidget(m_zoomView);
}


void KmagApp::slotChangeZoomBoxIndex(int index)
{
   m_pZoomBox->setCurrentItem(index);
}

void KmagApp::slotChangeRotationBoxIndex(int index)
{
  m_pRotationBox->setCurrentItem(index);
}

void KmagApp::slotChangeFPSIndex(int index)
{
  m_pFPSBox->setCurrentItem(index);
}


/**
 * Initialize all connections.
 */
void KmagApp::initConnections()
{
  // change in zoom value -> update the view
  connect(this, SIGNAL(updateZoomValue(float)), m_zoomView, SLOT(setZoom(float)));
  connect(this, SIGNAL(updateRotationValue(int)), m_zoomView, SLOT(setRotation(int)));
  connect(this, SIGNAL(updateFPSValue(float)), m_zoomView, SLOT(setRefreshRate(float)));

  // change in zoom index -> update the selector
  connect(this, SIGNAL(updateZoomIndex(int)), this, SLOT(slotChangeZoomBoxIndex(int)));
  connect(this, SIGNAL(updateRotationIndex(int)), this, SLOT(slotChangeRotationBoxIndex(int)));
  connect(this, SIGNAL(updateFPSIndex(int)), this, SLOT(slotChangeFPSIndex(int)));

  // selector selects a zoom index -> set the zoom index
  connect(m_pZoomBox, SIGNAL(activated(int)), this, SLOT(setZoomIndex(int)));
  connect(m_pRotationBox, SIGNAL(activated(int)), this, SLOT(setRotationIndex(int)));
  connect(m_pFPSBox, SIGNAL(activated(int)), this, SLOT(setFPSIndex(int)));
}

/**
 * Save options to config file.
 */
void KmagApp::saveOptions()
{
  KConfigGroup cg( config, "General Options");
  cg.writeEntry("Geometry", size());
  cg.writeEntry("ZoomIndex", m_zoomIndex);
  cg.writeEntry("RotationIndex", m_rotationIndex);
  cg.writeEntry("FPSIndex", m_fpsIndex);
  cg.writeEntry("SelRect", m_zoomView->getSelRectPos());
  cg.writeEntry("ShowMouse", m_zoomView->getShowMouseType());

  if (m_modeFollowMouse->isChecked())
     cg.writeEntry("Mode", "followmouse");
  else if (m_modeWholeScreen->isChecked())
     cg.writeEntry("Mode", "wholescreen");
  else if (m_modeSelWin->isChecked())
     cg.writeEntry("Mode", "selectionwindow");

  cg.writeEntry("ShowMenu", m_pShowMenu->isChecked());
  cg.writeEntry("ShowMainToolBar", m_pShowMainToolBar->isChecked());
  cg.writeEntry("ShowViewToolBar", m_pShowViewToolBar->isChecked());
  cg.writeEntry("ShowSettingsToolBar", m_pShowSettingsToolBar->isChecked());

  cg.changeGroup( "Main ToolBar" );
  toolBar("mainToolBar")->saveSettings( cg );
  cg.changeGroup( "View ToolBar" );
  toolBar("viewToolBar")->saveSettings( cg );
  cg.changeGroup( "Settings ToolBar" );
  toolBar("settingsToolBar")->saveSettings( cg );
}


/**
 * Read settings from config file.
 */
void KmagApp::readOptions()
{
  QColor blue (0,0,128);
  QColor yellow (255,255,0);
  QColor white (255,255,255);

  KConfigGroup cg( config, "WM");
  setTitleColors (
      cg.readEntry("inactiveBackground", blue),
      cg.readEntry("inactiveForeground", white),
      cg.readEntry("inactiveTitleBtnBg", yellow));

  cg.changeGroup("General Options");

  QSize defSize(460,390);
  QSize size=cg.readEntry("Geometry", defSize);
  if(!size.isEmpty())
  {
    resize(size);
  }

  // set zoom - defaults to 2x
  unsigned int zoomIndex = cg.readEntry("ZoomIndex", 4);
  setZoomIndex(zoomIndex);
  emit updateZoomIndex(m_zoomIndex);

  unsigned int rotationIndex = cg.readEntry("RotationIndex", 0);
  setRotationIndex(rotationIndex);
  emit updateRotationIndex(m_rotationIndex);

  unsigned int fpsIndex = cg.readEntry("FPSIndex", 2);
  setFPSIndex(fpsIndex);
  emit updateFPSIndex(m_fpsIndex);

  QString mode = cg.readEntry("Mode", "followmouse");
  if (mode == "wholescreen")
    slotModeWholeScreen();
  else if (mode == "selectionwindow")
    slotModeSelWin();
  else
    slotModeFollowMouse();

  QRect defaultRect(0,0,211,164);
  m_zoomView->setSelRectPos(cg.readEntry("SelRect", defaultRect));

  m_mouseCursorType = cg.readEntry("ShowMouse", m_defaultMouseCursorType);
  m_zoomView->showMouse(m_mouseCursorType);
  if(m_mouseCursorType)
    m_hideCursor->setChecked(false);
  else
    m_hideCursor->setChecked(true);

  cg.changeGroup("Settings ToolBar");
  if( cg.exists() )
      toolBar("settingsToolBar")->applySettings(cg );

  cg.changeGroup("Main ToolBar");
  if( cg.exists() )
      toolBar("mainToolBar")->applySettings( cg );

  cg.changeGroup("View ToolBar" );
  if( cg.exists() )
      toolBar("viewToolBar")->applySettings( cg );

  cg.changeGroup("General Options");
  m_pShowMenu->setChecked( cg.readEntry("ShowMenu", true));
  slotShowMenu();

  m_pShowMainToolBar->setChecked(cg.readEntry("ShowMainToolBar", false));
  slotShowMainToolBar();

  m_pShowViewToolBar->setChecked(cg.readEntry("ShowViewToolBar", true));
  slotShowViewToolBar();

  m_pShowSettingsToolBar->setChecked(cg.readEntry("ShowSettingsToolBar", true));
  slotShowSettingsToolBar();
}

bool KmagApp::queryClose()
{
  return (true);
}

bool KmagApp::queryExit()
{
  saveOptions();
  return true;
}

/**
 * Called when mouse is clicked inside the window
 *
 * @param e
 */
void KmagApp::contextMenuEvent ( QContextMenuEvent * e )
{
 // show popup
 KXMLGUIFactory *factory = this->factory();
 QMenu *popup = (QMenu *)factory->container("mainPopUp",this);
 if (popup != 0)
   popup->popup(e->globalPos(), 0);
 e->accept();
}


/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

/**
 * Shows/hides the mouse cursor
 */
void KmagApp::showMouseCursor(bool show)
{
  if(show) {
    if(m_mouseCursorType == 0)
      m_mouseCursorType = m_defaultMouseCursorType;
    m_zoomView->showMouse(m_mouseCursorType);
  } else {
    m_zoomView->showMouse(0);
  }
}

/**
 * Zoom in.
 */
void KmagApp::zoomIn()
{
  // set the new index .. checking will done inside setZoom
  setZoomIndex(m_zoomIndex+1);
  // signal change in zoom index
  emit updateZoomIndex((int)m_zoomIndex);
}

/**
 * Zoom out.
 */
void KmagApp::zoomOut()
{
  // set the new index .. checking will done inside setZoom
  setZoomIndex(m_zoomIndex-1);
  // signal change in zoom index
  emit updateZoomIndex((int)m_zoomIndex);
}
/**
 * Sets the zoom index to index
 */
void KmagApp::setZoomIndex(int index)
{
  if(index < 0 || index >= (int)zoomArray.size()) {
    // the index is invalid
    kWarning() << "Invalid index!" << endl;
    return;
  } else if((int)m_zoomIndex == index) {
    // do nothing!
    return;
  } else {
    m_zoomIndex = index;
  }

  if(m_zoomIndex == 0) {
    // meaning that no more zooming-out is possible
    // -> disable zoom-out icon
    m_pZoomOut->setEnabled(false);
  } else { // enable the icon
    m_pZoomOut->setEnabled(true);
  }

  if(m_zoomIndex == zoomArray.size()-1) {
    // meaning that no more zooming-in is possible
    // -> disable zoom-in icon
    m_pZoomIn->setEnabled(false);
  } else { // enable the icon
    m_pZoomIn->setEnabled(true);
  }

  // signal change in zoom value
  emit updateZoomValue(zoomArray[m_zoomIndex]);
}

/**
 * Sets the rotation index to index
 */
void KmagApp::setRotationIndex(int index)
{
  if(index < 0 || index >= (int)rotationArray.size()) {
    // the index is invalid
    kWarning() << "Invalid index!" << endl;
    return;
  } else if((int)m_rotationIndex == index) {
    // do nothing!
    return;
  } else {
    m_rotationIndex = index;
  }

  // signal change in zoom value
  emit updateRotationValue(rotationArray[m_rotationIndex]);
}

/**
 * Sets the fps index to index
 */
void KmagApp::setFPSIndex(int index)
{
  if(index < 0 || index >= (int)fpsArray.size()) {
    // the index is invalid
    kWarning() << "Invalid index!" << endl;
    return;
  } else if((int)m_fpsIndex == index) {
    // do nothing!
    return;
  } else {
    m_fpsIndex = index;
  }

  // signal change in fps value
  emit updateFPSValue(fpsArray[m_fpsIndex]);
}


/**
 * Save the zoomed image
 */
void KmagApp::saveZoomPixmap()
{
  bool toggled(false);

  // stop refresh temporarily
  if (m_zoomView->getRefreshStatus()) {
    slotToggleRefresh();
    toggled = true;
  }

  KUrl url = KFileDialog::getSaveUrl(QString(),
              KImageIO::pattern(KImageIO::Writing),
             0,i18n("Save Snapshot As"));

  if(!url.fileName().isEmpty()) {
    if(!url.isLocalFile()) {
      // create a temp file.. save image to it.. copy over the n/w and then delete the temp file.
      KTemporaryFile tempFile;
#ifdef __GNUC__
#warning "kde4: port KImageIO::type \n";
#endif
      if(!tempFile.open() || !m_zoomView->getPixmap().save(tempFile.fileName(),"png"/*, KImageIO::type(url.fileName()).latin1()*/)) {
        KMessageBox::error(0, i18n("Unable to save temporary file (before uploading to the network file you specified)."),
                          i18n("Error Writing File"));
      } else {
        if(!KIO::NetAccess::upload(tempFile.fileName(), url, this)) {
          KMessageBox::error(0, i18n("Unable to upload file over the network."),
                            i18n("Error Writing File"));
        } else {
          KMessageBox::information(0, i18n("Current zoomed image saved to\n%1", url.prettyUrl()),
                              i18n("Information"), "save_confirm");
        }
      }

    } else {
#ifdef __GNUC__
#warning "kde4 : port KImageIO::type(...) \n";
#endif
      if(!m_zoomView->getPixmap().save(url.path(), "png"/*KImageIO::type(url.fileName()).latin1()*/)) {
        KMessageBox::error(0, i18n("Unable to save file. Please check if you have permission to write to the directory."),
                            i18n("Error Writing File"));
      } else {
        KMessageBox::information(0, i18n("Current zoomed image saved to\n%1", url.prettyUrl()),
                                i18n("Information"), "save_confirm");
      }
    }
  }
  if(toggled) {
    slotToggleRefresh();
  }
}


void KmagApp::slotToggleRefresh()
{
  m_zoomView->toggleRefresh();
  if(m_zoomView->getRefreshStatus()) {
    refreshSwitch->setIcon(KIcon("stop.png"));
    refreshSwitch->setText(i18n("Stop"));
    refreshSwitch->setToolTip(i18n("Click to stop window update"));
  } else {
    refreshSwitch->setIcon(KIcon("reload.png"));
    refreshSwitch->setText(i18n("Start"));
    refreshSwitch->setToolTip(i18n("Click to start window update"));
  }
}


void KmagApp::slotModeWholeScreen()
{
  m_zoomView->setSelRectPos(QRect (0, 0, QApplication::desktop()->width(), QApplication::desktop()->height()));
  m_zoomView->followMouse(false);
  m_zoomView->showSelRect(false);
  m_zoomView->setFitToWindow (false);
  m_modeFollowMouse->setChecked(false);
  m_modeWholeScreen->setChecked(true);
  m_modeSelWin->setChecked(false);
}


void KmagApp::slotModeSelWin()
{
  m_zoomView->followMouse(false);
  m_zoomView->showSelRect(true);
  m_zoomView->setFitToWindow (false);
  m_modeFollowMouse->setChecked(false);
  m_modeWholeScreen->setChecked(false);
  m_modeSelWin->setChecked(true);
}


void KmagApp::slotModeFollowMouse()
{
  m_zoomView->followMouse(true);
  m_zoomView->showSelRect(false);
  m_zoomView->setFitToWindow (true);
  m_modeFollowMouse->setChecked(true);
  m_modeWholeScreen->setChecked(false);
  m_modeSelWin->setChecked(false);
}


void KmagApp::slotToggleHideCursor()
{
  showMouseCursor(!m_hideCursor->isChecked());
}


void KmagApp::slotFileNewWindow()
{
  KmagApp *new_window= new KmagApp();
  new_window->show();
}


void KmagApp::slotFilePrint()
{
#ifndef QT_NO_PRINTER

  bool toggled(false);

  KPrinter printer;

  // stop refresh temporarily
  if (m_zoomView->getRefreshStatus()) {
    slotToggleRefresh();
    toggled = true;
  }

  const QPixmap pixmap(m_zoomView->getPixmap());

  // use some AI to get the best orientation
  if(pixmap.width() > pixmap.height()) {
    printer.setOrientation(KPrinter::Landscape);
  } else {
    printer.setOrientation(KPrinter::Portrait);
  }

  if (printer.setup(this)) {
    QPainter paint;

    if(!paint.begin(&printer))
      return;
    // draw the pixmap
    paint.drawPixmap(0, 0, pixmap);
    // end the painting
    paint.end();
  }

  if(toggled) {
    slotToggleRefresh();
  }
#endif // QT_NO_PRINTER
}

void KmagApp::slotFileQuit()
{
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if (!memberList().isEmpty())
  {
	for (int i = 0; i < memberList().size(); ++i)
    {
	  w = memberList().at(i);
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
         break;
#ifdef __GNUC__
#warning "kde4: now memberList() is constant => we can't remove some element!"
#endif
	//memberList()->removeRef(w);
    }
  }
}

void KmagApp::copyToClipBoard()
{
  QClipboard *cb=KApplication::clipboard();
  cb->setPixmap(m_zoomView->getPixmap());
}

void KmagApp::slotShowMenu()
{
  ///////////////////////////////////////////////////////////////////
  // turn Menu on or off
  if(!m_pShowMenu->isChecked())
  {
    menuBar()->hide();
  }
  else
  {
    menuBar()->show();
  }


}

void KmagApp::slotShowMainToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // turn mainToolbar on or off
  if(!m_pShowMainToolBar->isChecked())
  {
    toolBar("mainToolBar")->hide();
  }
  else
  {
    toolBar("mainToolBar")->show();
  }
}

void KmagApp::slotShowViewToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // turn viewToolbar on or off
  if(!m_pShowViewToolBar->isChecked())
  {
    toolBar("viewToolBar")->hide();
  }
  else
  {
    toolBar("viewToolBar")->show();
  }
}

void KmagApp::slotShowSettingsToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // turn viewToolbar on or off
  if(!m_pShowSettingsToolBar->isChecked())
  {
    toolBar("settingsToolBar")->hide();
  }
  else
  {
    toolBar("settingsToolBar")->show();
  }
}

void KmagApp::slotConfKeys()
{
  KShortcutsDialog::configure( actionCollection() );
}

void KmagApp::slotEditToolbars()
{
  KConfigGroup cg( KGlobal::config(), "MainWindow" );
  saveMainWindowSettings( cg );
  KEditToolBar dlg( actionCollection() );
  connect( &dlg, SIGNAL( newToolbarConfig() ), this, SLOT( slotNewToolbarConfig() ) );
  if ( dlg.exec() )
    createGUI();
}


void KmagApp::slotNewToolbarConfig()
{
  applyMainWindowSettings( KGlobal::config()->group( "MainWindow" ) );
  createGUI();
}
