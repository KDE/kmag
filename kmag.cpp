/***************************************************************************
                          kmag.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sourceforge.net
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
#include <kaction.h>

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

#define ID_STATUS_MSG 1

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

  fpsArrayString << i18n("Very Low") << i18n("Low") << i18n("Medium") << i18n("High") << i18n("Very High");

  fpsArray.push_back(2); // very low
  fpsArray.push_back(6); // low
  fpsArray.push_back(10); // medium
  fpsArray.push_back(15); // high
  fpsArray.push_back(25); // very high

  if(zoomArrayString.count() != zoomArray.size() || fpsArrayString.count() != fpsArray.size()) {
    kdWarning() << "Check the zoom or fps array in the constructor." << endl;
    exit(1);
  }

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

  refreshSwitch = new KAction(i18n("Stop"), "stop", KStdAccel::key(KStdAccel::Reload), this,
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
  m_pShowMainToolBar = new KToggleAction(i18n("Show Main &Toolbar"), 0, 0, this,
                            SLOT(slotShowMainToolBar()), actionCollection(),"show_mainToolBar");
  m_pShowViewToolBar = new KToggleAction(i18n("Show &View Toolbar"), 0, 0, this,
                            SLOT(slotShowViewToolBar()), actionCollection(),"show_viewToolBar");
  m_pShowMagnificationToolBar = new KToggleAction(i18n("Show M&agnification Toolbar"), 0, 0, this,
                            SLOT(slotShowMagnificationToolBar()), actionCollection(),"show_magnificationToolBar");


  m_alwaysFit = new KToggleAction(i18n("&Always Fit Window"), "", CTRL+SHIFT+Key_F, this,
                            SLOT(slotAlwaysFit()), actionCollection(),"always_fit");

  m_fitToWindow = new KAction(i18n("&Fit Window"), "window_fullscreen", CTRL+Key_F, m_zoomView,
                              SLOT(fitToWindow()), actionCollection(),"fit_to_window");
  m_fitToWindow->setWhatsThis(i18n("Click on this button to fit the zoom view to the zoom window."));
  m_fitToWindow->setToolTip(i18n("Maximize the use of the window"));

  m_followMouse = new KToggleAction(i18n("Follow mouse"), "followmouse", 0, this,
                            SLOT(slotToggleFollowMouse()), actionCollection(), "followmouse");
  m_followMouse->setToolTip(i18n("Magnify around the mouse cursor"));
  m_followMouse->setWhatsThis(i18n("If selected, the area around the mouse cursor is magnified"));

  m_hideCursor = new KToggleAction(i18n("Hide mouse cursor"), "hidemouse", 0, this,
                            SLOT(slotToggleHideCursor()), actionCollection(), "hidecursor");
  m_hideCursor->setToolTip(i18n("Hide the mouse cursor"));

  m_showSelRect = new KToggleAction(i18n("Selection window"), "window", 0, this,
                            SLOT(slotToggleShowSelRect()), actionCollection(), "selectionwindow");
  m_showSelRect->setToolTip(i18n("Show the selection window on the screen"));

  m_pZoomIn = KStdAction::zoomIn(this, SLOT(zoomIn()), actionCollection(), "zoom_in");
  m_pZoomIn->setWhatsThis(i18n("Click on this button to <b>zoom-in</b> on the selected region."));

  m_pZoomBox = new KSelectAction(i18n("&Zoom"),0,actionCollection(),"zoom");
  m_pZoomBox->setItems(zoomArrayString);
  m_pZoomBox->setWhatsThis(i18n("Select the zoom factor."));
  m_pZoomBox->setToolTip(i18n("Zoom factor"));

  m_pZoomOut = KStdAction::zoomOut(this, SLOT(zoomOut()), actionCollection(), "zoom_out");
  m_pZoomOut->setWhatsThis(i18n("Click on this button to <b>zoom-out</b> on the selected region."));

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
  connect(this, SIGNAL(updateFPSValue(float)), m_zoomView, SLOT(setRefreshRate(float)));

  // change in zoom index -> update the selector
  connect(this, SIGNAL(updateZoomIndex(int)), m_pZoomBox, SLOT(setCurrentItem(int)));
  connect(this, SIGNAL(updateFPSIndex(int)), m_pFPSBox, SLOT(setCurrentItem(int)));

  // selector selects a zoom index -> set the zoom index
  connect(m_pZoomBox, SIGNAL(activated(int)), this, SLOT(setZoomIndex(int)));
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
  config->writeEntry("FPSIndex", m_fpsIndex);
  config->writeEntry("FollowMouse", m_zoomView->getFollowMouse());
  config->writeEntry("SelRect", m_zoomView->getSelRectPos());
  config->writeEntry("ShowSelRect", m_zoomView->getShowSelRect());
  config->writeEntry("ShowMouse", m_zoomView->getShowMouseType());

  config->writeEntry("AlwaysFit", m_alwaysFit->isChecked());
  
  config->writeEntry("ShowMenu", m_pShowMenu->isChecked());
  config->writeEntry("ShowMainToolBar", m_pShowMainToolBar->isChecked());
  config->writeEntry("ShowViewToolBar", m_pShowViewToolBar->isChecked());
  config->writeEntry("ShowMagnificationToolBar", m_pShowMagnificationToolBar->isChecked());

  toolBar("mainToolBar")->saveSettings(config,"Main ToolBar");
  toolBar("viewToolBar")->saveSettings(config,"View ToolBar");
  toolBar("magnificationToolBar")->saveSettings(config,"Magnification ToolBar");
}


/**
 * Read settings from config file.
 */
void KmagApp::readOptions()
{
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

  unsigned int fpsIndex = config->readUnsignedNumEntry("FPSIndex", 2);
  setFPSIndex(fpsIndex);
  emit updateFPSIndex(m_fpsIndex);

  bool followMouse = config->readBoolEntry("FollowMouse", true);
  m_zoomView->followMouse(followMouse);
  m_followMouse->setChecked(followMouse);

  QRect defaultRect(0,0,211,164);
  m_zoomView->setSelRectPos(config->readRectEntry("SelRect", &defaultRect));

  bool showSelRect = config->readBoolEntry("ShowSelRect", false);
  m_zoomView->showSelRect(showSelRect);
  m_showSelRect->setChecked(showSelRect);

  m_mouseCursorType = config->readUnsignedNumEntry("ShowMouse", m_defaultMouseCursorType);
  m_zoomView->showMouse(m_mouseCursorType);
  if(m_mouseCursorType)
    m_hideCursor->setChecked(false);
  else
    m_hideCursor->setChecked(true);
  
  if(config->hasGroup("Magnification ToolBar"))
    toolBar("magnificationToolBar")->applySettings(config,"Magnification ToolBar");
  else {
    toolBar("magnificationToolBar")->setBarPos(KToolBar::Bottom);
    toolBar("magnificationToolBar")->setIconText (KToolBar::IconTextRight);
    toolBar("magnificationToolBar")->setIconSize (16);
  }

  if(config->hasGroup("Main ToolBar"))
    toolBar("mainToolBar")->applySettings(config,"Main ToolBar");
  else
    toolBar("mainToolBar")->setBarPos(KToolBar::Bottom);
  
  if(config->hasGroup("View ToolBar"))
    toolBar("viewToolBar")->applySettings(config,"View ToolBar");
  else
    toolBar("viewToolBar")->setBarPos(KToolBar::Bottom);

  m_alwaysFit->setChecked(config->readBoolEntry("AlwaysFit", true));
  slotAlwaysFit();

  m_pShowMenu->setChecked(config->readBoolEntry("ShowMenu", true));
  slotShowMenu();
  
  m_pShowMainToolBar->setChecked(config->readBoolEntry("ShowMainToolBar", true));
  slotShowMainToolBar();
  
  m_pShowViewToolBar->setChecked(config->readBoolEntry("ShowViewToolBar", true));
  slotShowViewToolBar();
  
  m_pShowMagnificationToolBar->setChecked(config->readBoolEntry("ShowMagnificationToolBar", true));
  slotShowMagnificationToolBar();
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

  // signal change in zoom value
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
             0,i18n("Save Zoomed Region"));

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

void KmagApp::slotToggleFollowMouse()
{
  m_zoomView->followMouse(m_followMouse->isChecked());
}

void KmagApp::slotToggleHideCursor()
{
  showMouseCursor(!m_hideCursor->isChecked());
}

void KmagApp::slotToggleShowSelRect()
{
  m_zoomView->showSelRect(m_showSelRect->isChecked());
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

void KmagApp::slotAlwaysFit()
{
  m_zoomView->setFitToWindow (m_alwaysFit->isChecked());
  m_fitToWindow->setEnabled (!m_alwaysFit->isChecked());
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

void KmagApp::slotShowMagnificationToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // turn viewToolbar on or off
  if(!m_pShowMagnificationToolBar->isChecked())
  {
    toolBar("magnificationToolBar")->hide();
  }
  else
  {
    toolBar("magnificationToolBar")->show();
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
