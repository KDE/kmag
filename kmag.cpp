//$Id$
/***************************************************************************
                          kmag.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001 by Sarang Lakare
    email                : sarang@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <iostream>

// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qlayout.h>

// include files for KDE
#include <kapp.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kiconloader.h>
#include <khelpmenu.h>
#include <kimageio.h>
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
	: KMainWindow(0, name, WStyle_MinMax | WType_TopLevel | WDestructiveClose | WStyle_ContextHelp | WStyle_StaysOnTop)
{
  config=kapp->config();

	zoomArrayString << "[5:1] 20%"  << "[2:1] 50%"  << "[1.33:1] 75%"  << "[1:1] 100%"
    << "[1:1.25] 125%"  << "[1:1.5] 150%"  << "[1:2] 200%"  << "[1:3] 300%"
    << "[1:4] 400%"  << "[1:5] 500%" << "[1:6] 600%" << "[1:7] 700%"
    << "[1:8] 800%" << "[1:12] 1200%" << "[1:16] 1600%" << "[1:20] 2000%";

	// Is there a better way to initialize a vector array?
	zoomArray.push_back(0.2); zoomArray.push_back(0.5); zoomArray.push_back(0.75); zoomArray.push_back(1.0);
	zoomArray.push_back(1.25); zoomArray.push_back(1.5); zoomArray.push_back(2.0); zoomArray.push_back(3.0);
	zoomArray.push_back(4.0); zoomArray.push_back(5.0); zoomArray.push_back(6.0); zoomArray.push_back(7.0);
	zoomArray.push_back(8.0); zoomArray.push_back(12.0); zoomArray.push_back(16.0); zoomArray.push_back(20.0);

	if(zoomArrayString.count() != zoomArray.size()) {
		cerr << "Check the zoom array in the constructor." << endl;
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
}

void KmagApp::initActions()
{
  fileNewWindow = new KAction(i18n("New &Window"), "window_new", 0, this, SLOT(slotFileNewWindow()), actionCollection(),"file_new_window");
  fileNewWindow->setToolTip(i18n("Opens a new KMagnifier window"));

	refreshSwitch = new KAction(i18n("Stop"), "stop", Key_F5, this, SLOT(slotToggleRefresh()), actionCollection(), "start_stop_refresh");
  refreshSwitch->setToolTip(i18n("Click to stop window refresh [F5]"));
  refreshSwitch->setWhatsThis(i18n("Clicking on this icon will <b>start</b> / <b>stop</b>\
  updating of the display. Stopping the update will zero the processing power\
  required (CPU usage). You can use the F5 key on your keyboard to perform the same operation."));

  m_pSnapshot = new KAction(i18n("&SnapShot"), "ksnapshot", Key_F2, this, SLOT(saveZoomPixmap()), actionCollection(),"snapshot");
	m_pSnapshot->setWhatsThis(i18n("Click to save the image being displayed to a file. This can also by done using the F2 key on your keyboard."));
	m_pSnapshot->setToolTip(i18n("Save image to a file [F2]"));

  m_pPrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection(), "print");
  m_pPrint->setWhatsThis(i18n("Click on this button to print the current zommed image."));

  m_pZoomIn = KStdAction::zoomIn(this, SLOT(zoomIn()), actionCollection(), "zoom_in");
  m_pZoomIn->setWhatsThis(i18n("Click on this button to <b>zoom-in</b> on the selected region."));

  m_pZoomBox = new KSelectAction(i18n("&Zoom"),0,actionCollection(),"zoom");
  m_pZoomBox->setItems(zoomArrayString);
	m_pZoomBox->setComboWidth(50);

  m_pZoomOut = KStdAction::zoomOut(this, SLOT(zoomOut()), actionCollection(), "zoom_out");
  m_pZoomOut->setWhatsThis(i18n("Click on this button to <b>zoom-out</b> on the selected region."));

	KToolBarPopupAction *helpAction = new KToolBarPopupAction(i18n("&Help"), "help",
																					0, actionCollection(), "help");

	KHelpMenu *newHelpMenu = new KHelpMenu(this, KGlobal::instance()->aboutData());

	helpAction->setDelayed(false);
	KAction *action = KStdAction::helpContents(newHelpMenu, SLOT(appHelpActivated()), actionCollection());
  action->plug(helpAction->popupMenu());

	action = KStdAction::reportBug(newHelpMenu, SLOT(reportBug()), actionCollection());
  action->plug(helpAction->popupMenu());

	action = KStdAction::aboutApp(newHelpMenu, SLOT(aboutApplication()), actionCollection());
  action->plug(helpAction->popupMenu());

	action = KStdAction::aboutKDE(newHelpMenu, SLOT(aboutKDE()), actionCollection());
  action->plug(helpAction->popupMenu());	

	// plug things into the toolbar
	fileNewWindow->plug(toolBar());
  refreshSwitch->plug(toolBar());
  m_pZoomIn->plug(toolBar());
	m_pZoomBox->plug(toolBar());
  m_pZoomOut->plug(toolBar());
	m_pPrint->plug(toolBar());
	m_pSnapshot->plug(toolBar());
	helpAction->plug(toolBar());
}


void KmagApp::initView()
{
	QVBox *mainView = new QVBox(this);	

  m_zoomView = new KMagZoomView( mainView, "ZoomView" );
  m_zoomView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, m_zoomView->sizePolicy().hasHeightForWidth() ) );
  m_zoomView->setMouseTracking(true);
  m_zoomView->setFrameShape( QFrame::StyledPanel );
  m_zoomView->setFrameShadow( QFrame::Raised );

  m_settingsGroup = new QButtonGroup( mainView, "m_settingsGroup" );
  m_settingsGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, m_settingsGroup->sizePolicy().hasHeightForWidth() ) );
  m_settingsGroup->setFrameShape( QButtonGroup::NoFrame );
  m_settingsGroup->setTitle("");
  m_settingsGroup->setColumnLayout(0, Qt::Vertical );
  m_settingsGroup->layout()->setSpacing( 0 );
  m_settingsGroup->layout()->setMargin( 0 );
  QHBoxLayout *settingsGroupLayout = new QHBoxLayout( m_settingsGroup->layout() );
  settingsGroupLayout->setAlignment( Qt::AlignTop );
  settingsGroupLayout->setSpacing( 6 );
  settingsGroupLayout->setMargin( 0 );

  m_followMouseButton = new QCheckBox( m_settingsGroup, "m_followMouseButton" );
  m_followMouseButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, m_followMouseButton->sizePolicy().hasHeightForWidth() ) );
  m_followMouseButton->setText( i18n( "Follow Mouse" ) );
  settingsGroupLayout->addWidget( m_followMouseButton );
	connect(m_followMouseButton, SIGNAL(toggled(bool)), m_zoomView, SLOT(followMouse(bool)));

  m_showSelRectButton = new QCheckBox( m_settingsGroup, "m_showSelRectButton" );
  m_showSelRectButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, m_showSelRectButton->sizePolicy().hasHeightForWidth() ) );
  m_showSelRectButton->setText( i18n( "Show Selected Area" ) );
  settingsGroupLayout->addWidget( m_showSelRectButton );
  connect(m_showSelRectButton, SIGNAL(toggled(bool)), m_zoomView, SLOT(showSelRect(bool)));


  setCentralWidget(mainView);	
}

void KmagApp::initConnections()
{
	// change in zoom value -> update the view
	connect(this, SIGNAL(updateZoomValue(float)), m_zoomView, SLOT(setZoom(float)));
	// change in zoom index -> update the selector
	connect(this, SIGNAL(updateZoomIndex(int)), m_pZoomBox, SLOT(setCurrentItem(int)));
	// selector selects a zoom index -> set the zoom index
	connect(m_pZoomBox, SIGNAL(activated(int)), this, SLOT(setZoomIndex(int)));
}

void KmagApp::saveOptions()
{	
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
	config->writeEntry("ZoomIndex", m_zoomIndex);
	config->writeEntry("FollowMouse", m_zoomView->getFollowMouse());
	config->writeEntry("SelRect", m_zoomView->getSelRectPos());

  toolBar("mainToolBar")->saveSettings(config,"Main ToolBar");
}


void KmagApp::readOptions()
{
  config->setGroup("General Options");
	
  QSize size=config->readSizeEntry("Geometry");
  if(!size.isEmpty())
  {
    resize(size);
  }

	// set zoom - defaults to 2x
	unsigned int zoomIndex = config->readUnsignedNumEntry("ZoomIndex", 7);
	setZoomIndex(zoomIndex);
	emit updateZoomIndex(m_zoomIndex);

	bool followMouse = config->readBoolEntry("FollowMouse", false);
	m_zoomView->followMouse(followMouse);
	m_followMouseButton->setChecked(followMouse);

	QRect defaultRect(0, 0, 128, 128);
	m_zoomView->setSelRectPos(config->readRectEntry("SelRect", &defaultRect));	

  toolBar("mainToolBar")->applySettings(config,"Main ToolBar");
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
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

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
		cerr << "Invalid index!" << endl;
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
             0,i18n("Save zoomed region"));

	if(!url.filename().isEmpty()) {
		if(!m_zoomView->getPixmap().save(url.fileName(), KImageIO::type(url.fileName()).latin1())) {
    	KMessageBox::error(0, i18n("Unable to save file. Please check if you have permission to write to the directory."),
													i18n("Error writing file"));
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
    refreshSwitch->setToolTip(i18n("Click to stop window update [F5]"));
  } else {
    refreshSwitch->setIcon("reload.png");
    refreshSwitch->setText(i18n("Start"));
    refreshSwitch->setToolTip(i18n("Click to start window update [F5]"));
  }
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

	const QPixmap &pixmap(m_zoomView->getPixmap());

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

void KmagApp::slotEditCopy()
{
// TODO : Copy contents (zoomed image) to clipboard
}

void KmagApp::slotViewToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if(!viewToolBar->isChecked())
  {
    toolBar("mainToolBar")->hide();
  }
  else
  {
    toolBar("mainToolBar")->show();
  }		
}


