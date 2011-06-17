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
#include <tqdir.h>
#include <tqprinter.h>
#include <tqpainter.h>
#include <tqlayout.h>
#include <tqclipboard.h>
#include <tqdragobject.h>
#include <tqwhatsthis.h>
#include <tqtooltip.h>
#include <tqpopupmenu.h>

#include <kdeversion.h>

// include files for KDE
#if KDE_VERSION > 300
#include <kapplication.h>
#else
#include <kapp.h>
#endif // KDE 3.x

#include <kkeydialog.h>
#include <kinputdialog.h>
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
#include <kwin.h>

#if KDE_VERSION < 220
#include <tqprinter.h>
#else
#include <kprinter.h>
#endif

// application specific includes
#include "kmag.moc"
#include "kmagzoomview.h"
#include "kmagselrect.h"


#ifdef KDE_IS_VERSION
#if KDE_IS_VERSION(3,2,90)
   #define havesetCheckedState
#endif
#endif

KmagApp::KmagApp(TQWidget* , const char* name)
  : KMainWindow(0, name, WStyle_MinMax | WType_TopLevel | WStyle_StaysOnTop | WDestructiveClose | WStyle_ContextHelp),
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

  rotationArrayString << i18n("&No Rotation (0 Degrees)") << i18n("&Left (90 Degrees)") << i18n("&Upside Down (180 Degrees)") << i18n("&Right (270 Degrees)");

  rotationArray.push_back(0); // no rotation
  rotationArray.push_back(90); // left
  rotationArray.push_back(180); // upside down
  rotationArray.push_back(270); // right

  // call inits to invoke all other construction parts
  initView();
  initActions();
  initConnections();

  // Register all KIO image formats - to be used when saving image.
  KImageIO::registerFormats();

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
  fileNewWindow = new KAction(i18n("New &Window"), "window_new", KStdAccel::openNew(), TQT_TQOBJECT(this),
                              TQT_SLOT(slotFileNewWindow()), actionCollection(),"new_window");
  fileNewWindow->setToolTip(i18n("Open a new KMagnifier window"));

  refreshSwitch = new KAction(i18n("&Stop"), "stop", KStdAccel::reload(), TQT_TQOBJECT(this),
                              TQT_SLOT(slotToggleRefresh()), actionCollection(), "start_stop_refresh");
  refreshSwitch->setToolTip(i18n("Click to stop window refresh"));
  refreshSwitch->setWhatsThis(i18n("Clicking on this icon will <b>start</b> / <b>stop</b>\
  updating of the display. Stopping the update will zero the processing power\
  required (CPU usage)"));

  m_pSnapshot = new KAction(i18n("&Save Snapshot As..."), "ksnapshot", KStdAccel::save(), TQT_TQOBJECT(this),
                            TQT_SLOT(saveZoomPixmap()), actionCollection(),"snapshot");
  m_pSnapshot->setWhatsThis(i18n("Saves the zoomed view to an image file."));
  m_pSnapshot->setToolTip(i18n("Save image to a file"));

  m_pPrint = KStdAction::print(TQT_TQOBJECT(this), TQT_SLOT(slotFilePrint()), actionCollection(), "print");
  m_pPrint->setWhatsThis(i18n("Click on this button to print the current zoomed view."));

  m_pQuit = KStdAction::quit(TQT_TQOBJECT(this), TQT_SLOT(slotFileQuit()), actionCollection(), "quit");
  m_pQuit->setStatusText(i18n("Quits the application"));
  m_pQuit->setWhatsThis (i18n("Quits the application"));

  m_pCopy = KStdAction::copy(TQT_TQOBJECT(this), TQT_SLOT(copyToClipBoard()), actionCollection(), "copy");
  m_pCopy->setWhatsThis(i18n("Click on this button to copy the current zoomed view to the clipboard which you can paste in other applications."));
  m_pCopy->setToolTip(i18n("Copy zoomed image to clipboard"));

  m_pShowMenu = new KToggleAction(i18n("Show &Menu"), "showmenu", CTRL+Key_M, TQT_TQOBJECT(this),
                            TQT_SLOT(slotShowMenu()), actionCollection(),"show_menu");
  #ifdef havesetCheckedState
  m_pShowMenu->setCheckedState(i18n("Hide &Menu"));
  #endif
  m_pShowMainToolBar = new KToggleAction(i18n("Show Main &Toolbar"), 0, 0, TQT_TQOBJECT(this),
                            TQT_SLOT(slotShowMainToolBar()), actionCollection(),"show_mainToolBar");
  #ifdef havesetCheckedState
  m_pShowMainToolBar->setCheckedState(i18n("Hide Main &Toolbar"));
  #endif
  m_pShowViewToolBar = new KToggleAction(i18n("Show &View Toolbar"), 0, 0, TQT_TQOBJECT(this),
                            TQT_SLOT(slotShowViewToolBar()), actionCollection(),"show_viewToolBar");
  #ifdef havesetCheckedState
  m_pShowViewToolBar->setCheckedState(i18n("Hide &View Toolbar"));
  #endif
  m_pShowSettingsToolBar = new KToggleAction(i18n("Show &Settings Toolbar"), 0, 0, TQT_TQOBJECT(this),
                            TQT_SLOT(slotShowSettingsToolBar()), actionCollection(),"show_settingsToolBar");
  #ifdef havesetCheckedState
  m_pShowSettingsToolBar->setCheckedState(i18n("Hide &Settings Toolbar"));
  #endif

  m_modeFollowMouse = new KRadioAction(i18n("&Follow Mouse Mode"), "followmouse", Key_F1, TQT_TQOBJECT(this),
                            TQT_SLOT(slotModeFollowMouse()), actionCollection(), "mode_followmouse");
  m_modeFollowMouse->setToolTip(i18n("Magnify mouse area into window"));
  m_modeFollowMouse->setWhatsThis(i18n("In this mode the area around the mouse cursor is shown in a normal window."));

  m_modeSelWin = new KRadioAction(i18n("S&election Window Mode"), "window", Key_F2, TQT_TQOBJECT(this),
                            TQT_SLOT(slotModeSelWin()), actionCollection(), "mode_selectionwindow");
  m_modeSelWin->setToolTip(i18n("Magnify selected area into window"));
  m_modeSelWin->setWhatsThis(i18n("In this mode a selection window is opened. The selected area is shown in a normal window."));

  m_modeEdgeTop = new KRadioAction(i18n("&Top Screen Edge Mode"), 0, 0, TQT_TQOBJECT(this),
                              TQT_SLOT(slotModeEdgeTop()), actionCollection(),"mode_edgetop");
  m_modeEdgeTop->setToolTip(i18n("Magnify mouse area to top screen edge"));
  m_modeEdgeTop->setWhatsThis(i18n("In this mode the area around the mouse is magnified to the top screen edge."));

  m_modeEdgeLeft = new KRadioAction(i18n("&Left Screen Edge Mode"), 0, 0, TQT_TQOBJECT(this),
                              TQT_SLOT(slotModeEdgeLeft()), actionCollection(),"mode_edgeleft");
  m_modeEdgeLeft->setToolTip(i18n("Magnify mouse area to left screen edge"));
  m_modeEdgeLeft->setWhatsThis(i18n("In this mode the area around the mouse is magnified to the left screen edge."));

  m_modeEdgeRight = new KRadioAction(i18n("&Right Screen Edge Mode"), 0, 0, TQT_TQOBJECT(this),
                              TQT_SLOT(slotModeEdgeRight()), actionCollection(),"mode_edgeright");
  m_modeEdgeRight->setToolTip(i18n("Magnify mouse area to right screen edge"));
  m_modeEdgeRight->setWhatsThis(i18n("In this mode the area around the mouse is magnified to the right screen edge."));

  m_modeEdgeBottom = new KRadioAction(i18n("&Bottom Screen Edge Mode"), 0, 0, TQT_TQOBJECT(this),
                              TQT_SLOT(slotModeEdgeBottom()), actionCollection(),"mode_edgebottom");
  m_modeEdgeBottom->setToolTip(i18n("Magnify mouse area to bottom screen edge"));
  m_modeEdgeBottom->setWhatsThis(i18n("In this mode the area around the mouse is magnified to the bottom screen edge."));

  m_hideCursor = new KToggleAction(i18n("Hide Mouse &Cursor"), "hidemouse", Key_F4, TQT_TQOBJECT(this),
                            TQT_SLOT(slotToggleHideCursor()), actionCollection(), "hidecursor");
  #ifdef havesetCheckedState
  m_hideCursor->setCheckedState(i18n("Show Mouse &Cursor"));
  #endif
  m_hideCursor->setToolTip(i18n("Hide the mouse cursor"));

  m_pZoomIn = KStdAction::zoomIn(TQT_TQOBJECT(this), TQT_SLOT(zoomIn()), actionCollection(), "zoom_in");
  m_pZoomIn->setWhatsThis(i18n("Click on this button to <b>zoom-in</b> on the selected region."));

  m_pZoomBox = new KSelectAction(i18n("&Zoom"),0,actionCollection(),"zoom");
  m_pZoomBox->setItems(zoomArrayString);
  m_pZoomBox->setWhatsThis(i18n("Select the zoom factor."));
  m_pZoomBox->setToolTip(i18n("Zoom factor"));

  m_pZoomOut = KStdAction::zoomOut(TQT_TQOBJECT(this), TQT_SLOT(zoomOut()), actionCollection(), "zoom_out");
  m_pZoomOut->setWhatsThis(i18n("Click on this button to <b>zoom-out</b> on the selected region."));

  m_pInvert = new KToggleAction(i18n("&Invert Colors"), 0, Key_F6, TQT_TQOBJECT(this),
                            TQT_SLOT(slotToggleInvert()), actionCollection(), "invert");

  m_pRotationBox = new KSelectAction(i18n("&Rotation"),0,actionCollection(),"rotation");
  m_pRotationBox->setItems(rotationArrayString);
  m_pRotationBox->setWhatsThis(i18n("Select the rotation degree."));
  m_pRotationBox->setToolTip(i18n("Rotation degree"));

  // KHelpMenu *newHelpMenu = new KHelpMenu(this, KGlobal::instance()->aboutData());

  m_keyConf = KStdAction::keyBindings( TQT_TQOBJECT(this), TQT_SLOT( slotConfKeys() ), actionCollection(), "key_conf");
  m_toolConf = KStdAction::configureToolbars( TQT_TQOBJECT(this), TQT_SLOT( slotEditToolbars() ),
                                              actionCollection(), "toolbar_conf");

  m_pFPSBox = new KSelectAction(i18n("Re&fresh"),0,actionCollection(),"fps_selector");
  m_pFPSBox->setItems(fpsArrayString);
  m_pFPSBox->setWhatsThis(i18n("Select the refresh rate. The higher the rate, the more computing power (CPU) will be needed."));
  m_pFPSBox->setToolTip(i18n("Refresh rate"));

  createGUI();
}

void KmagApp::initView()
{
  m_zoomView = new KMagZoomView( this, "ZoomView" );
  m_zoomView->tqsetSizePolicy( TQSizePolicy( (TQSizePolicy::SizeType)7, (TQSizePolicy::SizeType)7, m_zoomView->sizePolicy().hasHeightForWidth() ) );
  m_zoomView->setFrameShape( TQFrame::StyledPanel );
  m_zoomView->setFrameShadow( TQFrame::Raised );

  setCentralWidget(m_zoomView);
}

/**
 * Initialize all connections.
 */
void KmagApp::initConnections()
{
  // change in zoom value -> update the view
  connect(this, TQT_SIGNAL(updateZoomValue(float)), m_zoomView, TQT_SLOT(setZoom(float)));
  connect(this, TQT_SIGNAL(updateRotationValue(int)), m_zoomView, TQT_SLOT(setRotation(int)));
  connect(this, TQT_SIGNAL(updateFPSValue(float)), m_zoomView, TQT_SLOT(setRefreshRate(float)));

  // change in zoom index -> update the selector
  connect(this, TQT_SIGNAL(updateZoomIndex(int)), m_pZoomBox, TQT_SLOT(setCurrentItem(int)));
  connect(this, TQT_SIGNAL(updateRotationIndex(int)), m_pRotationBox, TQT_SLOT(setCurrentItem(int)));
  connect(this, TQT_SIGNAL(updateFPSIndex(int)), m_pFPSBox, TQT_SLOT(setCurrentItem(int)));

  // selector selects a zoom index -> set the zoom index
  connect(m_pZoomBox, TQT_SIGNAL(activated(int)), TQT_TQOBJECT(this), TQT_SLOT(setZoomIndex(int)));
  connect(m_pRotationBox, TQT_SIGNAL(activated(int)), TQT_TQOBJECT(this), TQT_SLOT(setRotationIndex(int)));
  connect(m_pFPSBox, TQT_SIGNAL(activated(int)), TQT_TQOBJECT(this), TQT_SLOT(setFPSIndex(int)));

  connect(m_zoomView, TQT_SIGNAL(contextMenu(TQPoint)), TQT_TQOBJECT(this), TQT_SLOT(contextMenu(TQPoint)));
}

/**
 * Save options to config file.
 */
void KmagApp::saveOptions()
{
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
  config->writeEntry("ZoomIndex", m_zoomIndex);
  config->writeEntry("Invertation", m_pInvert->isChecked());
  config->writeEntry("RotationIndex", m_rotationIndex);
  config->writeEntry("FPSIndex", m_fpsIndex);
  config->writeEntry("SelRect", m_zoomView->getSelRectPos());
  config->writeEntry("ShowMouse", m_zoomView->getShowMouseType());

  if (m_modeFollowMouse->isChecked())
     config->writeEntry("Mode", "followmouse");
  else if (m_modeSelWin->isChecked())
     config->writeEntry("Mode", "selectionwindow");
  else if (m_modeEdgeTop->isChecked())
     config->writeEntry("Mode", "edgetop");
  else if (m_modeEdgeLeft->isChecked())
     config->writeEntry("Mode", "edgeleft");
  else if (m_modeEdgeRight->isChecked())
     config->writeEntry("Mode", "edgeright");
  else if (m_modeEdgeBottom->isChecked())
     config->writeEntry("Mode", "edgebottom");

  config->writeEntry ("EdgeSize", edgesize);

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
  TQColor blue (0,0,128);
  TQColor yellow (255,255,0);
  TQColor white (255,255,255);

  config->setGroup ("WM");
  setTitleColors (
      config->readColorEntry("inactiveBackground", &blue),
      config->readColorEntry("inactiveForeground", &white),
      config->readColorEntry("inactiveTitleBtnBg", &yellow));

  config->setGroup("General Options");

  TQSize defSize(460,390);
  TQSize size=config->readSizeEntry("Geometry", &defSize);
  if(!size.isEmpty())
  {
    resize(size);
  }

  // set zoom - defaults to 2x
  unsigned int zoomIndex = config->readUnsignedNumEntry("ZoomIndex", 4);
  setZoomIndex(zoomIndex);
  emit updateZoomIndex(m_zoomIndex);

  m_pInvert->setChecked (config->readBoolEntry("Invertation", false));
  slotToggleInvert();

  unsigned int rotationIndex = config->readUnsignedNumEntry("RotationIndex", 0);
  setRotationIndex(rotationIndex);
  emit updateRotationIndex(m_rotationIndex);

  unsigned int fpsIndex = config->readUnsignedNumEntry("FPSIndex", 2);
  setFPSIndex(fpsIndex);
  emit updateFPSIndex(m_fpsIndex);

  TQRect defaultRect(0,0,211,164);
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

  config->setGroup("General Options");

  edgesize = config->readNumEntry("EdgeSize", 0);
  TQString mode = config->readEntry("Mode", "followmouse");
  if (mode == "selectionwindow")
    slotModeSelWin();
  else if (mode == "edgetop")
    setEdgeMode (m_modeEdgeTop);
  else if (mode == "edgeleft")
    setEdgeMode (m_modeEdgeLeft);
  else if (mode == "edgeright")
    setEdgeMode (m_modeEdgeRight);
  else if (mode == "edgebottom")
    setEdgeMode (m_modeEdgeBottom);
  else
    slotModeFollowMouse();
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


/////////////////////////////////////////////////////////////////////
// TQT_SLOT IMPLEMENTATION
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
 * Shows/hides the mouse cursor
 */
void KmagApp::slotToggleInvert ()
{
  m_zoomView->setInvertation (m_pInvert->isChecked());
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
  if (m_zoomView->getRefreshtqStatus()) {
    slotToggleRefresh();
    toggled = true;
  }

  KURL url = KFileDialog::getSaveURL(TQString(),
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
        if(!KIO::NetAccess::upload(tempFile.name(), url, this)) {
          KMessageBox::error(0, i18n("Unable to upload file over the network."),
                            i18n("Error Writing File"));
        } else {
          KMessageBox::information(0, i18n("Current zoomed image saved to\n%1").tqarg(url.prettyURL()),
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
        KMessageBox::information(0, i18n("Current zoomed image saved to\n%1").tqarg(url.prettyURL()),
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
  if(m_zoomView->getRefreshtqStatus()) {
    refreshSwitch->setIcon("stop.png");
    refreshSwitch->setText(i18n("Stop"));
    refreshSwitch->setToolTip(i18n("Click to stop window update"));
  } else {
    refreshSwitch->setIcon("reload.png");
    refreshSwitch->setText(i18n("Start"));
    refreshSwitch->setToolTip(i18n("Click to start window update"));
  }
}


void KmagApp::slotModeSelWin()
{
  unsetEdgeMode(m_modeSelWin);

  m_zoomView->fitToWindow();
  m_zoomView->setFitToWindow (false);
  m_zoomView->followMouse(false);
  m_zoomView->showSelRect(true);
}


void KmagApp::slotModeFollowMouse()
{
  unsetEdgeMode(m_modeFollowMouse);

  m_zoomView->setFitToWindow (true);
  m_zoomView->followMouse(true);
  m_zoomView->showSelRect(false);
}


void KmagApp::slotModeEdgeTop()
{
  // ask for edgesize
  int newedgesize; bool ok;
  newedgesize = KInputDialog::getInteger (i18n ("Magnify to Screen Edge - Select Size"),
                                          i18n ("Size:"), edgesize > 0 ? edgesize : 300, 200,
                                          TQApplication::desktop()->screenGeometry( this ).height()/2,
                                          25, 10, &ok, 0, "getedgesize");

  if (ok) {
    edgesize = newedgesize;
    setEdgeMode(m_modeEdgeTop);
  }
}


void KmagApp::slotModeEdgeLeft()
{
  // ask for edgesize
  int newedgesize; bool ok;
  newedgesize = KInputDialog::getInteger (i18n ("Magnify to Left Screen Edge - Select Size"),
                                          i18n ("Size:"), edgesize > 0 ? edgesize : 300, 200,
                                          TQApplication::desktop()->screenGeometry( this ).width()/2,
                                          25, 10, &ok, 0, "getedgesize");

  if (ok) {
    edgesize = newedgesize;
    setEdgeMode(m_modeEdgeLeft);
  }
}


void KmagApp::slotModeEdgeRight()
{
  // ask for edgesize
  int newedgesize; bool ok;
  newedgesize = KInputDialog::getInteger (i18n ("Magnify to Right Screen Edge - Select Size"),
                                          i18n ("Size:"), edgesize > 0 ? edgesize : 300, 200,
                                          TQApplication::desktop()->screenGeometry( this ).width()/2,
                                          25, 10, &ok, 0, "getedgesize");

  if (ok) {
    edgesize = newedgesize;
    setEdgeMode(m_modeEdgeRight);
  }
}


void KmagApp::slotModeEdgeBottom()
{
  // ask for edgesize
  int newedgesize; bool ok;
  newedgesize = KInputDialog::getInteger (i18n ("Magnify to Bottom Screen Edge - Select Size"),
                                          i18n ("Size:"), edgesize > 0 ? edgesize : 300, 200,
                                          TQApplication::desktop()->screenGeometry( this ).height()/2,
                                          25, 10, &ok, 0, "getedgesize");

  if (ok) {
    edgesize = newedgesize;
    setEdgeMode(m_modeEdgeBottom);
  }
}


void KmagApp::setEdgeMode (KToggleAction *mode)
{
  if (m_modeEdgeLeft || mode == m_modeEdgeRight) {
    if (edgesize < 200 || edgesize > TQApplication::desktop()->screenGeometry( this ).width()/2)
      edgesize = TQApplication::desktop()->screenGeometry( this ).width()/4;
  } else {
    if (edgesize < 200 || edgesize > TQApplication::desktop()->screenGeometry( this ).height()/2)
      edgesize = TQApplication::desktop()->screenGeometry( this ).height()/4;
  }

  m_modeFollowMouse->setChecked(false);
  m_modeSelWin->setChecked(false);
  m_modeEdgeTop->setChecked(false);
  m_modeEdgeLeft->setChecked(false);
  m_modeEdgeRight->setChecked(false);
  m_modeEdgeBottom->setChecked(false);
  mode->setChecked (true);

  m_zoomView->setFitToWindow (true);
  m_zoomView->followMouse(true);
  m_zoomView->showSelRect(false);

  m_zoomView->reparent (0, TQPoint(), false);
  KWin::setType(m_zoomView->winId(), NET::Dock);
  KWin::setState(m_zoomView->winId(), NET::Sticky | NET::KeepBelow | NET::SkipTaskbar | NET::SkipPager);
  KWin::setOnAllDesktops(m_zoomView->winId(), true);

  m_pShowMenu->setEnabled (false);
  m_pShowMainToolBar->setEnabled (false);
  m_pShowViewToolBar->setEnabled (false);
  m_pShowSettingsToolBar->setEnabled (false);

  hide();

  if (mode == m_modeEdgeTop) {
    TQRect r = TQApplication::desktop()->screenGeometry( this );
    r.setBottom( r.top() + edgesize );
    m_zoomView->setGeometry ( r );
    KWin::setExtendedStrut (m_zoomView->winId(), 0, 0, 0, 0, 0, 0,
                            edgesize, r.left(), r.right(), 0, 0, 0);
  } else if (mode == m_modeEdgeLeft) {
    TQRect r = TQApplication::desktop()->screenGeometry( this );
    r.setRight( r.left() + edgesize );
    m_zoomView->setGeometry ( r );
    KWin::setExtendedStrut (m_zoomView->winId(),
                            edgesize, r.top(), r.bottom(),
                            0, 0, 0, 0, 0, 0, 0, 0, 0);
  } else if (mode == m_modeEdgeRight) {
    TQRect r = TQApplication::desktop()->screenGeometry( this );
    r.setLeft( r.right() - edgesize );
    m_zoomView->setGeometry ( r );
    KWin::setExtendedStrut (m_zoomView->winId(), 0, 0, 0,
                            edgesize, r.top(), r.bottom(),
                            0, 0, 0, 0, 0, 0);
  } else {
    TQRect r = TQApplication::desktop()->screenGeometry( this );
    r.setTop( r.bottom() - edgesize );
    m_zoomView->setGeometry ( r );
    KWin::setExtendedStrut (m_zoomView->winId(), 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            edgesize, r.left(), r.right());
  }
  m_zoomView->show();
}


void KmagApp::unsetEdgeMode (KToggleAction *mode)
{
  edgesize = 0;

  m_modeFollowMouse->setChecked(false);
  m_modeSelWin->setChecked(false);
  m_modeEdgeTop->setChecked(false);
  m_modeEdgeLeft->setChecked(false);
  m_modeEdgeRight->setChecked(false);
  m_modeEdgeBottom->setChecked(false);
  mode->setChecked (true);

  m_zoomView->reparent (this, TQPoint(), true);
  setCentralWidget(m_zoomView);
  KWin::setExtendedStrut (winId(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  m_pShowMenu->setEnabled (true);
  m_pShowMainToolBar->setEnabled (true);
  m_pShowViewToolBar->setEnabled (true);
  m_pShowSettingsToolBar->setEnabled (true);

  show();
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
#ifndef TQT_NO_PRINTER

  bool toggled(false);

#if KDE_VERSION < 220
  TQPrinter printer;
#else
  KPrinter printer;
#endif

  // stop refresh temporarily
  if (m_zoomView->getRefreshtqStatus()) {
    slotToggleRefresh();
    toggled = true;
  }

  const TQPixmap pixmap(m_zoomView->getPixmap());

#if KDE_VERSION >= 220
  // use some AI to get the best orientation
  if(pixmap.width() > pixmap.height()) {
    printer.setOrientation(KPrinter::Landscape);
  } else {
    printer.setOrientation(KPrinter::Portrait);
  }
#endif

  if (printer.setup(this)) {
    TQPainter paint;

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
#endif // TQT_NO_PRINTER
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
  TQClipboard *cb=KApplication::tqclipboard();
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
  if(!m_pShowSettingsToolBar->isChecked() || edgesize > 0)
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
  KKeyDialog::configure( actionCollection() );
}

void KmagApp::slotEditToolbars()
{
  saveMainWindowSettings( KGlobal::config(), "MainWindow" );
  KEditToolbar dlg( actionCollection() );
  connect( &dlg, TQT_SIGNAL( newToolbarConfig() ), TQT_TQOBJECT(this), TQT_SLOT( slotNewToolbarConfig() ) );
  if ( dlg.exec() )
    createGUI();
}


void KmagApp::contextMenu (TQPoint pos)
{
 // show popup
 KXMLGUIFactory *factory = this->factory();
 TQPopupMenu *popup = (TQPopupMenu *)factory->container("mainPopUp",this);
 if (popup != 0)
   popup->popup(pos, 0);
}


void KmagApp::slotNewToolbarConfig()
{
  applyMainWindowSettings( KGlobal::config(), "MainWindow" );
  createGUI();
}
