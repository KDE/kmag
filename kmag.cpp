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
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qpopupmenu.h>

#include <kdeversion.h>

// include files for KDE
#if KDE_VERSION > 300
#include <kapplication.h>
#else
#include <kapp.h>
#endif // KDE 3.x

#include <kkeydialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstdaction.h>
#include <khelpmenu.h>
#include <kimageio.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <ktempfile.h>
#include <kpopupmenu.h>
#include <kedittoolbar.h>

#if KDE_VERSION < 220
#include <qprinter.h>
#else
#include <kprinter.h>
#endif

// application specific includes
#include "kmag.moc"
#include "kmagzoomview.h"
#include "kmagselrect.h"


#ifdef KDE_IS_VERSION
#if KDE_IS_VERSION(3,3,0)
   #define setCheckedState
#endif
#endif

KmagApp::KmagApp(QWidget* , const char* name)
  : KMainWindow(0, name, WStyle_MinMax | WType_TopLevel | WDestructiveClose | WStyle_ContextHelp | WStyle_StaysOnTop),
    m_defaultMouseCursorType(2)
{
  config=kapp->config();

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

  rotationArrayString << i18n("&No rotation (0°)") << i18n("&Left (90°)") << i18n("&Upside down (180°)") << i18n("&Right (270°)");

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

  // Register all KIO image formats - to be used when saving image.
  KImageIO::registerFormats();
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
  fileNewWindow = new KAction(i18n("New &Window"), "window_new", KStdAccel::key(KStdAccel::New), this,
                              SLOT(slotFileNewWindow()), actionCollection(),"new_window");
  fileNewWindow->setToolTip(i18n("Open a new KMagnifier window"));

  refreshSwitch = new KAction(i18n("&Stop"), "stop", KStdAccel::key(KStdAccel::Reload), this,
                              SLOT(slotToggleRefresh()), actionCollection(), "start_stop_refresh");
  refreshSwitch->setToolTip(i18n("Click to stop window refresh"));
  refreshSwitch->setWhatsThis(i18n("Clicking on this icon will <b>start</b> / <b>stop</b>\
  updating of the display. Stopping the update will zero the processing power\
  required (CPU usage)"));

  m_pSnapshot = new KAction(i18n("&Save Snapshot As..."), "ksnapshot", KStdAccel::key(KStdAccel::Save), this,
                            SLOT(saveZoomPixmap()), actionCollection(),"snapshot");
  m_pSnapshot->setWhatsThis(i18n("Saves the zoomed view to an image file."));
  m_pSnapshot->setToolTip(i18n("Save image to a file"));

  m_pPrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection(), "print");
  m_pPrint->setWhatsThis(i18n("Click on this button to print the current zoomed view."));

  m_pQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection(), "quit");
  m_pQuit->setStatusText(i18n("Quits the application"));
  m_pQuit->setWhatsThis (i18n("Quits the application"));

  m_pCopy = KStdAction::copy(this, SLOT(copyToClipBoard()), actionCollection(), "copy");
  m_pCopy->setWhatsThis(i18n("Click on this button to copy the current zoomed view to the clipboard which you can paste in other applications."));
  m_pCopy->setToolTip(i18n("Copy zoomed image to clipboard"));

  m_pShowMenu = new KToggleAction(i18n("Show &Menu"), "showmenu", CTRL+Key_M, this,
                            SLOT(slotShowMenu()), actionCollection(),"show_menu");
  #ifdef setCheckedState
  m_pShowMenu->setCheckedState(i18n("Hide &Menu"));
  #endif
  m_pShowMainToolBar = new KToggleAction(i18n("Show Main &Toolbar"), 0, 0, this,
                            SLOT(slotShowMainToolBar()), actionCollection(),"show_mainToolBar");
  #ifdef setCheckedState
  m_pShowMainToolBar->setCheckedState(i18n("Hide Main &Toolbar"));
  #endif
  m_pShowViewToolBar = new KToggleAction(i18n("Show &View Toolbar"), 0, 0, this,
                            SLOT(slotShowViewToolBar()), actionCollection(),"show_viewToolBar");
  #ifdef setCheckedState
  m_pShowViewToolBar->setCheckedState(i18n("Hide &View Toolbar"));
  #endif
  m_pShowSettingsToolBar = new KToggleAction(i18n("Show &Settings Toolbar"), 0, 0, this,
                            SLOT(slotShowSettingsToolBar()), actionCollection(),"show_settingsToolBar");
  #ifdef setCheckedState
  m_pShowSettingsToolBar->setCheckedState(i18n("Hide &Settings Toolbar"));
  #endif

  m_modeFollowMouse = new KRadioAction(i18n("&Follow Mouse Mode"), "followmouse", Key_F1, this,
                            SLOT(slotModeFollowMouse()), actionCollection(), "mode_followmouse");
  m_modeFollowMouse->setToolTip(i18n("Magnify around the mouse cursor"));
  m_modeFollowMouse->setWhatsThis(i18n("If selected, the area around the mouse cursor is magnified"));

  m_modeSelWin = new KRadioAction(i18n("Se&lection Window Mode"), "window", Key_F2, this,
                            SLOT(slotModeSelWin()), actionCollection(), "mode_selectionwindow");
  m_modeSelWin->setToolTip(i18n("Show a window for selecting the magnified area"));

  m_modeWholeScreen = new KRadioAction(i18n("&Whole Screen Mode"), "window_fullscreen", Key_F3, this,
                              SLOT(slotModeWholeScreen()), actionCollection(),"mode_wholescreen");
  m_modeWholeScreen->setToolTip(i18n("Magnify the whole screen"));
  m_modeWholeScreen->setWhatsThis(i18n("Click on this button to fit the zoom view to the zoom window."));

  m_hideCursor = new KToggleAction(i18n("Hide Mouse &Cursor"), "hidemouse", Key_F4, this,
                            SLOT(slotToggleHideCursor()), actionCollection(), "hidecursor");
  #ifdef setCheckedState
  m_hideCursor->setCheckedState(i18n("Show Mouse &Cursor"));
  #endif
  m_hideCursor->setToolTip(i18n("Hide the mouse cursor"));

  m_pZoomIn = KStdAction::zoomIn(this, SLOT(zoomIn()), actionCollection(), "zoom_in");
  m_pZoomIn->setWhatsThis(i18n("Click on this button to <b>zoom-in</b> on the selected region."));

  m_pZoomBox = new KSelectAction(i18n("&Zoom"),0,actionCollection(),"zoom");
  m_pZoomBox->setItems(zoomArrayString);
  m_pZoomBox->setWhatsThis(i18n("Select the zoom factor."));
  m_pZoomBox->setToolTip(i18n("Zoom factor"));

  m_pZoomOut = KStdAction::zoomOut(this, SLOT(zoomOut()), actionCollection(), "zoom_out");
  m_pZoomOut->setWhatsThis(i18n("Click on this button to <b>zoom-out</b> on the selected region."));

  m_pRotationBox = new KSelectAction(i18n("&Rotation"),0,actionCollection(),"rotation");
  m_pRotationBox->setItems(rotationArrayString);
  m_pRotationBox->setWhatsThis(i18n("Select the rotation factor."));
  m_pRotationBox->setToolTip(i18n("Rotation factor"));

  // KHelpMenu *newHelpMenu = new KHelpMenu(this, KGlobal::instance()->aboutData());

  m_keyConf = KStdAction::keyBindings( this, SLOT( slotConfKeys() ), actionCollection(), "key_conf");
  m_toolConf = KStdAction::configureToolbars( this, SLOT( slotEditToolbars() ),
                                              actionCollection(), "toolbar_conf");

  m_pFPSBox = new KSelectAction(i18n("&Refresh"),0,actionCollection(),"fps_selector");
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
  connect(this, SIGNAL(updateZoomIndex(int)), m_pZoomBox, SLOT(setCurrentItem(int)));
  connect(this, SIGNAL(updateRotationIndex(int)), m_pRotationBox, SLOT(setCurrentItem(int)));
  connect(this, SIGNAL(updateFPSIndex(int)), m_pFPSBox, SLOT(setCurrentItem(int)));

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
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
  config->writeEntry("ZoomIndex", m_zoomIndex);
  config->writeEntry("RotationIndex", m_rotationIndex);
  config->writeEntry("FPSIndex", m_fpsIndex);
  config->writeEntry("SelRect", m_zoomView->getSelRectPos());
  config->writeEntry("ShowMouse", m_zoomView->getShowMouseType());

  if (m_modeFollowMouse->isChecked())
     config->writeEntry("Mode", "followmouse");
  else if (m_modeWholeScreen->isChecked())
     config->writeEntry("Mode", "wholescreen");
  else if (m_modeSelWin->isChecked())
     config->writeEntry("Mode", "selectionwindow");

  config->writeEntry("ShowMenu", m_pShowMenu->isChecked());
  config->writeEntry("ShowMainToolBar", m_pShowMainToolBar->isChecked());
  config->writeEntry("ShowViewToolBar", m_pShowViewToolBar->isChecked());
  config->writeEntry("ShowSettingsToolBar", m_pShowSettingsToolBar->isChecked());

  toolBar("mainToolBar")->saveSettings(config,"Main ToolBar");
  toolBar("viewToolBar")->saveSettings(config,"View ToolBar");
  toolBar("settingsToolBar")->saveSettings(config,"Settings ToolBar");
}


/**
 * Read settings from config file.
 */
void KmagApp::readOptions()
{
  QColor blue (0,0,128);
  QColor yellow (255,255,0);
  QColor white (255,255,255);

  config->setGroup ("WM");
  setTitleColors (
      config->readColorEntry("inactiveBackground", &blue),
      config->readColorEntry("inactiveForeground", &white),
      config->readColorEntry("inactiveTitleBtnBg", &yellow));

  config->setGroup("General Options");

  QSize defSize(460,390);
  QSize size=config->readSizeEntry("Geometry", &defSize);
  if(!size.isEmpty())
  {
    resize(size);
  }

  // set zoom - defaults to 2x
  unsigned int zoomIndex = config->readUnsignedNumEntry("ZoomIndex", 4);
  setZoomIndex(zoomIndex);
  emit updateZoomIndex(m_zoomIndex);

  unsigned int rotationIndex = config->readUnsignedNumEntry("RotationIndex", 0);
  setRotationIndex(rotationIndex);
  emit updateRotationIndex(m_rotationIndex);

  unsigned int fpsIndex = config->readUnsignedNumEntry("FPSIndex", 2);
  setFPSIndex(fpsIndex);
  emit updateFPSIndex(m_fpsIndex);

  QString mode = config->readEntry("Mode", "followmouse");
  if (mode == "wholescreen")
    slotModeWholeScreen();
  else if (mode == "selectionwindow")
    slotModeSelWin();
  else
    slotModeFollowMouse();

  QRect defaultRect(0,0,211,164);
  m_zoomView->setSelRectPos(config->readRectEntry("SelRect", &defaultRect));

  m_mouseCursorType = config->readUnsignedNumEntry("ShowMouse", m_defaultMouseCursorType);
  m_zoomView->showMouse(m_mouseCursorType);
  if(m_mouseCursorType)
    m_hideCursor->setChecked(false);
  else
    m_hideCursor->setChecked(true);

  if(config->hasGroup("Settings ToolBar"))
    toolBar("settingsToolBar")->applySettings(config,"Settings ToolBar");

  if(config->hasGroup("Main ToolBar"))
    toolBar("mainToolBar")->applySettings(config,"Main ToolBar");

  if(config->hasGroup("View ToolBar"))
    toolBar("viewToolBar")->applySettings(config,"View ToolBar");

  m_pShowMenu->setChecked(config->readBoolEntry("ShowMenu", true));
  slotShowMenu();

  m_pShowMainToolBar->setChecked(config->readBoolEntry("ShowMainToolBar", false));
  slotShowMainToolBar();

  m_pShowViewToolBar->setChecked(config->readBoolEntry("ShowViewToolBar", true));
  slotShowViewToolBar();

  m_pShowSettingsToolBar->setChecked(config->readBoolEntry("ShowSettingsToolBar", true));
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
 QPopupMenu *popup = (QPopupMenu *)factory->container("mainPopUp",this);
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
    kdWarning() << "Invalid index!" << endl;
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
    kdWarning() << "Invalid index!" << endl;
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
    kdWarning() << "Invalid index!" << endl;
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

  KURL url = KFileDialog::getSaveURL(QString::null,
              KImageIO::pattern(KImageIO::Writing),
             0,i18n("Save Snapshot As"));

  if(!url.filename().isEmpty()) {
    if(!url.isLocalFile()) {
      // create a temp file.. save image to it.. copy over the n/w and then delete the temp file.
      KTempFile tempFile;
      if(!m_zoomView->getPixmap().save(tempFile.name(), KImageIO::type(url.fileName()).latin1())) {
        KMessageBox::error(0, i18n("Unable to save temporary file (before uploading to the network file you specified)."),
                          i18n("Error Writing File"));
      } else {
        if(!KIO::NetAccess::upload(tempFile.name(), url)) {
          KMessageBox::error(0, i18n("Unable to upload file over the network."),
                            i18n("Error Writing File"));
        } else {
          KMessageBox::information(0, i18n("Current zoomed image saved to\n%1").arg(url.prettyURL()),
                              i18n("Information"), "save_confirm");
        }
      }
      // remove the temporary file
      tempFile.unlink();

    } else {
      if(!m_zoomView->getPixmap().save(url.path(), KImageIO::type(url.fileName()).latin1())) {
        KMessageBox::error(0, i18n("Unable to save file. Please check if you have permission to write to the directory."),
                            i18n("Error Writing File"));
      } else {
        KMessageBox::information(0, i18n("Current zoomed image saved to\n%1").arg(url.prettyURL()),
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
    refreshSwitch->setIcon("stop.png");
    refreshSwitch->setText(i18n("Stop"));
    refreshSwitch->setToolTip(i18n("Click to stop window update"));
  } else {
    refreshSwitch->setIcon("reload.png");
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

#if KDE_VERSION < 220
  QPrinter printer;
#else
  KPrinter printer;
#endif

  // stop refresh temporarily
  if (m_zoomView->getRefreshStatus()) {
    slotToggleRefresh();
    toggled = true;
  }

  const QPixmap pixmap(m_zoomView->getPixmap());

#if KDE_VERSION >= 220
  // use some AI to get the best orientation
  if(pixmap.width() > pixmap.height()) {
    printer.setOrientation(KPrinter::Landscape);
  } else {
    printer.setOrientation(KPrinter::Portrait);
  }
#endif

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
  if (memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
         break;
      memberList->removeRef(w);
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
  KKeyDialog::configureKeys( actionCollection(), xmlFile() );
}

void KmagApp::slotEditToolbars()
{
  saveMainWindowSettings( KGlobal::config(), "MainWindow" );
  KEditToolbar dlg( actionCollection() );
  connect( &dlg, SIGNAL( newToolbarConfig() ), this, SLOT( slotNewToolbarConfig() ) );
  if ( dlg.exec() )
    createGUI();
}


void KmagApp::slotNewToolbarConfig()
{
  applyMainWindowSettings( KGlobal::config(), "MainWindow" );
  createGUI();
}
