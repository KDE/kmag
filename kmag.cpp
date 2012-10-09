/***************************************************************************
                          kmag.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sourceforge.net
    copyright            : (C) 2003-2004 by Olaf Schmidt
    email                : ojschmidt@kde.org
    copyright            : (C) 2008 by Matthew Woehlke
    email                : mw_triad@users.sourceforge.net
    copyright              (C) 2010 Sebastian Sauer
    email                  sebsauer@kdab.com
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
#include <QtCore/QDir>
#include <QtCore/QPointer>
#include <QtGui/QPrintDialog>
#include <QtGui/QPainter>
#include <QtGui/QLayout>
#include <QtGui/QClipboard>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QPixmap>
#include <QtGui/QDesktopWidget>

// include files for KDE
#include <kxmlguiclient.h>
#include <ktoolbar.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kselectaction.h>
#include <kdeversion.h>
#include <kxmlguifactory.h>
#include <kicon.h>
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
#include <kglobal.h>
#include <kdeprintdialog.h>

// application specific includes
#include "kmag.moc"
#include "kmagzoomview.h"
#include "kmagselrect.h"


KmagApp::KmagApp(QWidget* , const char* name)
  : KXmlGuiWindow(0) // Qt::WStyle_MinMax | Qt::WType_TopLevel | Qt::WDestructiveClose | Qt::WStyle_ContextHelp | Qt::WindowCloseButtonHint | Qt::WStyle_StaysOnTop
  , m_defaultMouseCursorType(2)
{
  setObjectName( QLatin1String( name ) );
  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
  config=KGlobal::config();

  zoomArrayString << QLatin1String( "5:1" ) << QLatin1String( "2:1" ) << QLatin1String( "1:1" ) << QLatin1String( "1:1.5" ) << QLatin1String( "1:2" ) << QLatin1String( "1:3" ) << QLatin1String( "1:4" ) << QLatin1String( "1:5" )
    << QLatin1String( "1:6" ) << QLatin1String( "1:7" ) << QLatin1String( "1:8" ) << QLatin1String( "1:12" ) << QLatin1String( "1:16" ) << QLatin1String( "1:20" );

  zoomArray << 0.2 <<0.5 << 1.0;
  zoomArray << 1.5 << 2.0 << 3.0;
  zoomArray << 4.0 << 5.0 << 6.0 << 7.0;
  zoomArray << 8.0 << 12.0 << 16.0 << 20.0;

  fpsArrayString << i18nc("Zoom at very low", "&Very Low") << i18nc("Zoom at low", "&Low") << i18nc("Zoom at medium", "&Medium") << i18nc("Zoom at high", "&High") << i18nc("Zoom at very high", "V&ery High");

  fpsArray.append(2); // very low
  fpsArray.append(6); // low
  fpsArray.append(10); // medium
  fpsArray.append(15); // high
  fpsArray.append(25); // very high

  colorArrayString << i18nc("No color-blindness simulation, i.e. 'normal' vision", "&Normal") << i18n("&Protanopia") << i18n("&Deuteranopia") << i18n("&Tritanopia") << i18n("&Achromatopsia");

  colorArray.append(0);
  colorArray.append(1);
  colorArray.append(2);
  colorArray.append(3);
  colorArray.append(4);

  rotationArrayString << i18n("&No Rotation (0 Degrees)") << i18n("&Left (90 Degrees)") << i18n("&Upside Down (180 Degrees)") << i18n("&Right (270 Degrees)");

  rotationArray.append(0); // no rotation
  rotationArray.append(90); // left
  rotationArray.append(180); // upside down
  rotationArray.append(270); // right

  // call inits to invoke all other construction parts
  initView();
  initActions();
  initConnections();

  // read options from config file
  readOptions();

  #ifndef QT_NO_PRINTER
  m_printer = 0;
  #endif // QT_NO_PRINTER
}

/**
 * Default destructor.
 */
KmagApp::~KmagApp()
{
    m_zoomView->showSelRect(false);

    #ifndef QT_NO_PRINTER
    delete m_printer;
    #endif // QT_NO_PRINTER
}

void KmagApp::initActions()
{
  fileNewWindow = actionCollection()->addAction(QLatin1String( "new_window" ));
  fileNewWindow->setIcon(KIcon(QLatin1String( "window-new" )));
  fileNewWindow->setText(i18n("New &Window"));
  connect(fileNewWindow, SIGNAL(triggered(bool)), SLOT(slotFileNewWindow()));
  fileNewWindow->setShortcuts(KStandardShortcut::openNew());
  fileNewWindow->setToolTip(i18n("Open a new KMagnifier window"));

  refreshSwitch = actionCollection()->addAction(QLatin1String( "start_stop_refresh" ));
  refreshSwitch->setIcon(KIcon(QLatin1String( "process-stop" )));
  refreshSwitch->setText(i18n("&Stop"));
  connect(refreshSwitch, SIGNAL(triggered(bool)), SLOT(slotToggleRefresh()));
  refreshSwitch->setShortcuts(KStandardShortcut::reload());
  refreshSwitch->setToolTip(i18n("Click to stop window refresh"));
  refreshSwitch->setWhatsThis(i18n("Clicking on this icon will <b>start</b> / <b>stop</b> "
                                   "updating of the display. Stopping the update will zero the processing power "
                                   "required (CPU usage)"));

  m_pSnapshot = actionCollection()->addAction(QLatin1String( "snapshot" ));
  m_pSnapshot->setIcon(KIcon(QLatin1String( "ksnapshot" )));
  m_pSnapshot->setText(i18n("&Save Snapshot As..."));
  connect(m_pSnapshot, SIGNAL(triggered(bool)), SLOT(saveZoomPixmap()));
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

  m_pShowMenu = KStandardAction::showMenubar(this, SLOT(slotShowMenu()), actionCollection());

  setStandardToolBarMenuEnabled(true);

  m_modeFollowMouse = new KToggleAction(KIcon(QLatin1String( "followmouse" )), i18n("&Follow Mouse Mode"), this);
  actionCollection()->addAction(QLatin1String( "mode_followmouse" ), m_modeFollowMouse);
  connect(m_modeFollowMouse, SIGNAL(triggered(bool)), SLOT(slotModeChanged()));
  m_modeFollowMouse->setShortcut(Qt::Key_F1);
  m_modeFollowMouse->setIconText(i18n("Mouse"));
  m_modeFollowMouse->setToolTip(i18n("Magnify around the mouse cursor"));
  m_modeFollowMouse->setWhatsThis(i18n("If selected, the area around the mouse cursor is magnified"));

#ifdef QAccessibilityClient_FOUND

  m_modeFollowFocus = new KToggleAction(KIcon(QLatin1String( "view-restore" )), i18n("&Follow Focus Mode"), this);
  actionCollection()->addAction(QLatin1String( "mode_followfocus" ), m_modeFollowFocus);
  connect(m_modeFollowFocus, SIGNAL(triggered(bool)), SLOT(slotModeChanged()));
  m_modeFollowFocus->setShortcut(Qt::Key_F2);
  m_modeFollowFocus->setIconText(i18n("Focus"));
  m_modeFollowFocus->setToolTip(i18n("Magnify around the keyboard focus"));
  m_modeFollowFocus->setWhatsThis(i18n("If selected, the area around the keyboard cursor is magnified"));

#endif

  m_modeSelWin = new KToggleAction(KIcon(QLatin1String( "window" )), i18n("Se&lection Window Mode"), this);
  actionCollection()->addAction(QLatin1String( "mode_selectionwindow" ), m_modeSelWin);
  connect(m_modeSelWin, SIGNAL(triggered(bool)), SLOT(slotModeSelWin()));
  m_modeSelWin->setShortcut(Qt::Key_F3);
  m_modeSelWin->setIconText(i18n("Window"));
  m_modeSelWin->setToolTip(i18n("Show a window for selecting the magnified area"));

  m_modeWholeScreen = new KToggleAction(KIcon(QLatin1String( "view-fullscreen" )), i18n("&Whole Screen Mode"), this);
  actionCollection()->addAction(QLatin1String( "mode_wholescreen" ), m_modeWholeScreen);
  connect(m_modeWholeScreen, SIGNAL(triggered(bool)), SLOT(slotModeWholeScreen()));
  m_modeWholeScreen->setShortcut(Qt::Key_F4);
  m_modeWholeScreen->setIconText(i18n("Screen"));
  m_modeWholeScreen->setToolTip(i18n("Magnify the whole screen"));
  m_modeWholeScreen->setWhatsThis(i18n("Click on this button to fit the zoom view to the zoom window."));

  m_hideCursor = new KToggleAction(KIcon(QLatin1String( "hidemouse" )), i18n("Hide Mouse &Cursor"), this);
  actionCollection()->addAction(QLatin1String( "hidecursor" ), m_hideCursor);
  connect(m_hideCursor, SIGNAL(triggered(bool)), SLOT(slotToggleHideCursor()));
  m_hideCursor->setShortcut(Qt::Key_F5);
  #ifdef havesetCheckedStatef
  m_hideCursor->setCheckedState(KGuiItem(i18n("Show Mouse &Cursor")));
  #endif
  m_hideCursor->setIconText(i18n("Hide"));
  m_hideCursor->setToolTip(i18n("Hide the mouse cursor"));

  m_staysOnTop = new KToggleAction(KIcon(QLatin1String( "go-top" )), i18n("Stays On Top"), this);
  actionCollection()->addAction(QLatin1String( "staysontop" ), m_staysOnTop);
  connect(m_staysOnTop, SIGNAL(triggered(bool)), SLOT(slotStaysOnTop()));
  m_staysOnTop->setShortcut(Qt::Key_F6);
  m_staysOnTop->setToolTip(i18n("The KMagnifier Window stays on top of other windows."));

  m_pZoomIn = actionCollection()->addAction(KStandardAction::ZoomIn, this, SLOT(zoomIn()));
  m_pZoomIn->setWhatsThis(i18n("Click on this button to <b>zoom-in</b> on the selected region."));

  m_pZoomBox = new KSelectAction(i18n("&Zoom"), this);
  actionCollection()->addAction(QLatin1String( "zoom" ), m_pZoomBox);
  m_pZoomBox->setItems(zoomArrayString);
  m_pZoomBox->setWhatsThis(i18n("Select the zoom factor."));
  m_pZoomBox->setToolTip(i18n("Zoom factor"));

  m_pZoomOut = actionCollection()->addAction(KStandardAction::ZoomOut, this, SLOT(zoomOut()));
  m_pZoomOut->setWhatsThis(i18n("Click on this button to <b>zoom-out</b> on the selected region."));

  m_pRotationBox = new KSelectAction(i18n("&Rotation"),this);
  actionCollection()->addAction(QLatin1String( "rotation" ), m_pRotationBox);
  m_pRotationBox->setItems(rotationArrayString);
  m_pRotationBox->setWhatsThis(i18n("Select the rotation degree."));
  m_pRotationBox->setToolTip(i18n("Rotation degree"));

  // KHelpMenu *newHelpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData());

  m_keyConf = actionCollection()->addAction(KStandardAction::KeyBindings, this, SLOT(slotConfKeys()));
  m_toolConf = actionCollection()->addAction(KStandardAction::ConfigureToolbars, this, SLOT(slotEditToolbars()));

  m_pFPSBox = new KSelectAction(i18n("&Refresh"),this);
  actionCollection()->addAction(QLatin1String( "fps_selector" ), m_pFPSBox);
  m_pFPSBox->setItems(fpsArrayString);
  m_pFPSBox->setWhatsThis(i18n("Select the refresh rate. The higher the rate, the more computing power (CPU) will be needed."));
  m_pFPSBox->setToolTip(i18n("Refresh rate"));

  m_pColorBox = new KSelectAction(i18nc("Color-blindness simulation mode", "&Color"),this);
  actionCollection()->addAction(QLatin1String( "color_mode" ), m_pColorBox);
  m_pColorBox->setItems(colorArrayString);
  m_pColorBox->setWhatsThis(i18n("Select a mode to simulate various types of color-blindness."));
  m_pColorBox->setToolTip(i18n("Color-blindness Simulation Mode"));

  setupGUI(ToolBar | Keys | Save | Create);
}

void KmagApp::initView()
{
  m_zoomView = new KMagZoomView( this, "ZoomView" );
//m_zoomView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, m_zoomView->sizePolicy().hasHeightForWidth() ) );
  m_zoomView->setFrameShape( QFrame::NoFrame );

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

void KmagApp::slotChangeColorIndex(int index)
{
  m_pColorBox->setCurrentItem(index);
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
  connect(this, SIGNAL(updateColorValue(int)), m_zoomView, SLOT(setColorMode(int)));

  // change in zoom index -> update the selector
  connect(this, SIGNAL(updateZoomIndex(int)), this, SLOT(slotChangeZoomBoxIndex(int)));
  connect(this, SIGNAL(updateRotationIndex(int)), this, SLOT(slotChangeRotationBoxIndex(int)));
  connect(this, SIGNAL(updateFPSIndex(int)), this, SLOT(slotChangeFPSIndex(int)));
  connect(this, SIGNAL(updateColorIndex(int)), this, SLOT(slotChangeColorIndex(int)));

  // selector selects a zoom index -> set the zoom index
  connect(m_pZoomBox, SIGNAL(triggered(int)), this, SLOT(setZoomIndex(int)));
  connect(m_pRotationBox, SIGNAL(triggered(int)), this, SLOT(setRotationIndex(int)));
  connect(m_pFPSBox, SIGNAL(triggered(int)), this, SLOT(setFPSIndex(int)));
  connect(m_pColorBox, SIGNAL(triggered(int)), this, SLOT(setColorIndex(int)));
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
  cg.writeEntry("ColorIndex", m_colorIndex);
  cg.writeEntry("SelRect", m_zoomView->getSelRectPos());
  cg.writeEntry("ShowMouse", m_zoomView->getShowMouseType());
  cg.writeEntry("StaysOnTop", m_staysOnTop->isChecked());

  if (m_modeFollowMouse->isChecked())
     cg.writeEntry("Mode", "followmouse");
#ifdef QAccessibilityClient_FOUND
  else if (m_modeFollowFocus->isChecked())
     cg.writeEntry("Mode", "followfocus");
#endif
  else if (m_modeWholeScreen->isChecked())
     cg.writeEntry("Mode", "wholescreen");
  else if (m_modeSelWin->isChecked())
     cg.writeEntry("Mode", "selectionwindow");
}


/**
 * Read settings from config file.
 */
void KmagApp::readOptions()
{
  QColor blue (0,0,128);
  QColor yellow (255,255,0);
  QColor white (255,255,255);

  KConfigGroup cgWM( config, "WM");
  setTitleColors (
      cgWM.readEntry("inactiveBackground", blue),
      cgWM.readEntry("inactiveForeground", white),
      cgWM.readEntry("inactiveTitleBtnBg", yellow));

  KConfigGroup cg(config,"General Options");

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

  unsigned int colorIndex = cg.readEntry("ColorIndex", 0);
  setColorIndex(colorIndex);
  emit updateColorIndex(colorIndex);
/*
  QString mode = cg.readEntry("Mode", "followmouse");
  if (mode == QLatin1String( "wholescreen" )) {
    slotModeWholeScreen();
  } else if (mode == QLatin1String( "selectionwindow" )) {
    slotModeSelWin();
  } else if (mode == QLatin1String( "followfocus" )) {
    m_modeFollowFocus->setChecked(true);
    slotModeChanged();
  } else {
    m_modeFollowMouse->setChecked(true);
    slotModeChanged();
  }
*/
  QRect defaultRect(0,0,211,164);
  m_zoomView->setSelRectPos(cg.readEntry("SelRect", defaultRect));

  m_mouseCursorType = cg.readEntry("ShowMouse", m_defaultMouseCursorType);
  m_zoomView->showMouse(m_mouseCursorType);
  m_hideCursor->setChecked(!m_mouseCursorType);

  m_staysOnTop->setChecked(cg.readEntry("StaysOnTop", true));
  slotStaysOnTop();

  // XMLGui has already read and set up the menuBar for us
  m_pShowMenu->setChecked(menuBar()->isVisible());
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
 QMenu *popup = (QMenu *)factory->container(QLatin1String( "mainPopUp" ),this);
 if (popup != 0)
   popup->popup(e->globalPos());
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
    kWarning() << "Invalid index!" ;
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
  emit updateZoomValue(zoomArray.at(m_zoomIndex));
}

/**
 * Sets the rotation index to index
 */
void KmagApp::setRotationIndex(int index)
{
  if(index < 0 || index >= (int)rotationArray.size()) {
    // the index is invalid
    kWarning() << "Invalid index!" ;
    return;
  } else if((int)m_rotationIndex == index) {
    // do nothing!
    return;
  } else {
    m_rotationIndex = index;
  }

  // signal change in zoom value
  emit updateRotationValue(rotationArray.at(m_rotationIndex));
}

/**
 * Sets the fps index to index
 */
void KmagApp::setFPSIndex(int index)
{
  if(index < 0 || index >= (int)fpsArray.size()) {
    // the index is invalid
    kWarning() << "Invalid index!" ;
    return;
  } else if((int)m_fpsIndex == index) {
    // do nothing!
    return;
  } else {
    m_fpsIndex = index;
  }

  // signal change in fps value
  emit updateFPSValue(fpsArray.at(m_fpsIndex));
}

/**
 * Sets the color index to index
 */
void KmagApp::setColorIndex(int index)
{
  if(index < 0 || index >= (int)colorArray.size()) {
    // the index is invalid
    kWarning() << "Invalid index!" ;
    return;
  } else if((int)m_colorIndex == index) {
    // do nothing!
    return;
  } else {
    m_colorIndex = index;
  }

  // signal change in fps value
  emit updateColorValue(colorArray.at(m_colorIndex));
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
      if(!tempFile.open() || !m_zoomView->getImage().save(tempFile.fileName(),"png"/*, KImageIO::type(url.fileName()).latin1()*/)) {
        KMessageBox::error(0, i18n("Unable to save temporary file (before uploading to the network file you specified)."),
                          i18n("Error Writing File"));
      } else {
        if(!KIO::NetAccess::upload(tempFile.fileName(), url, this)) {
          KMessageBox::error(0, i18n("Unable to upload file over the network."),
                            i18n("Error Writing File"));
        } else {
          KMessageBox::information(0, i18n("Current zoomed image saved to\n%1", url.prettyUrl()),
                              i18n("Information"), QLatin1String( "save_confirm" ));
        }
      }

    } else {
#ifdef __GNUC__
#warning "kde4 : port KImageIO::type(...) \n";
#endif
      if(!m_zoomView->getImage().save(url.path(), "png"/*KImageIO::type(url.fileName()).latin1()*/)) {
        KMessageBox::error(0, i18n("Unable to save file. Please check if you have permission to write to the directory."),
                            i18n("Error Writing File"));
      } else {
        KMessageBox::information(0, i18n("Current zoomed image saved to\n%1", url.prettyUrl()),
                                i18n("Information"), QLatin1String( "save_confirm" ));
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
    refreshSwitch->setIcon(KIcon(QLatin1String( "process-stop" )));
    refreshSwitch->setText(i18n("Stop"));
    refreshSwitch->setToolTip(i18n("Click to stop window update"));
  } else {
    refreshSwitch->setIcon(KIcon(QLatin1String( "system-run" )));
    refreshSwitch->setText(i18nc("Start updating the window", "Start"));
    refreshSwitch->setToolTip(i18n("Click to start window update"));
  }
}


void KmagApp::slotModeWholeScreen()
{
  m_zoomView->followMouse(false);
  m_zoomView->setSelRectPos(QRect (0, 0, QApplication::desktop()->width(), QApplication::desktop()->height()));
  m_zoomView->showSelRect(false);
  m_zoomView->setFitToWindow (false);
  m_modeFollowMouse->setChecked(false);
#ifdef QAccessibilityClient_FOUND
  m_zoomView->followBoth(false);
  m_zoomView->followFocus(false);
  m_modeFollowFocus->setChecked(false);
#endif
  m_modeWholeScreen->setChecked(true);
  m_modeSelWin->setChecked(false);
}


void KmagApp::slotModeSelWin()
{
  m_zoomView->followMouse(false);
  m_zoomView->showSelRect(true);
  m_zoomView->setFitToWindow (false);
  m_modeFollowMouse->setChecked(false);
#ifdef QAccessibilityClient_FOUND
  m_zoomView->followBoth(false);
  m_zoomView->followFocus(false);
  m_modeFollowFocus->setChecked(false);
#endif
  m_modeWholeScreen->setChecked(false);
  m_modeSelWin->setChecked(true);
}

void KmagApp::slotModeChanged()
{
#ifdef QAccessibilityClient_FOUND
  if(m_modeFollowMouse->isChecked() && m_modeFollowFocus->isChecked()) {

    //BOTH MODE
    m_zoomView->followMouse(false);
    m_zoomView->followBoth(true);
    m_zoomView->followFocus(false);
    m_zoomView->showSelRect(false);
    m_zoomView->setFitToWindow (true);
    m_modeWholeScreen->setChecked(false);
    m_modeSelWin->setChecked(false);
    return;
  }
#endif

  if (m_modeFollowMouse->isChecked()) {

    //MOUSE MODE
    m_zoomView->followMouse(true);
    m_zoomView->showSelRect(false);
    m_zoomView->setFitToWindow (true);
    m_modeFollowMouse->setChecked(true);
#ifdef QAccessibilityClient_FOUND
    m_zoomView->followBoth(false);
    m_zoomView->followFocus(false);
    m_modeFollowFocus->setChecked(false);
#endif
    m_modeWholeScreen->setChecked(false);
    m_modeSelWin->setChecked(false);

#ifdef QAccessibilityClient_FOUND
  } else if (m_modeFollowFocus->isChecked()) {

    //FOCUS MODE
    m_zoomView->followBoth(false);
    m_zoomView->followMouse(false);
    m_zoomView->followFocus(true);
    m_zoomView->showSelRect(false);
    m_zoomView->setFitToWindow (true);
    m_modeFollowMouse->setChecked(false);
    m_modeFollowFocus->setChecked(true);
    m_modeWholeScreen->setChecked(false);
    m_modeSelWin->setChecked(false);
#endif

  }
}

void KmagApp::slotToggleHideCursor()
{
  showMouseCursor(!m_hideCursor->isChecked());
}


void KmagApp::slotStaysOnTop()
{
  if (m_staysOnTop->isChecked())
      setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  else
      setWindowFlags( windowFlags() & !Qt::WindowStaysOnTopHint );
  show();
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

  if (m_printer == 0) {
    m_printer = new QPrinter();
  }

  // stop refresh temporarily
  if (m_zoomView->getRefreshStatus()) {
    slotToggleRefresh();
    toggled = true;
  }

  const QImage pixmap(m_zoomView->getImage());

  // use some AI to get the best orientation
  if(pixmap.width() > pixmap.height()) {
    m_printer->setOrientation(QPrinter::Landscape);
  } else {
    m_printer->setOrientation(QPrinter::Portrait);
  }

  QPrintDialog *printDialog = KdePrint::createPrintDialog(m_printer, this);

  if (printDialog->exec()) {
    QPainter paint;

    if(!paint.begin(m_printer))
    {
      delete printDialog;
      return;
    }
    // draw the pixmap
    paint.drawImage(0, 0, pixmap);
    // end the painting
    paint.end();
  }

  if(toggled) {
    slotToggleRefresh();
  }
  delete printDialog;
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
  cb->setPixmap(QPixmap::fromImage(m_zoomView->getImage()));
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

void KmagApp::slotConfKeys()
{
  KShortcutsDialog::configure( actionCollection() );
}

void KmagApp::slotEditToolbars()
{
  KConfigGroup cg( KGlobal::config(), "MainWindow" );
  saveMainWindowSettings( cg );
  QPointer<KEditToolBar> dlg = new KEditToolBar( actionCollection() );
  connect(dlg, SIGNAL(newToolBarConfig()), this, SLOT(slotNewToolbarConfig()) );
  if ( dlg->exec() )
    createGUI();
  delete dlg;
}


void KmagApp::slotNewToolbarConfig()
{
  applyMainWindowSettings( KGlobal::config()->group( "MainWindow" ) );
  createGUI();
}
