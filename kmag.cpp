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
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kiconloader.h>
#include <khelpmenu.h>

// application specific includes
#include "kmag.moc"
#include "kmagzoomview.h"
#include "kmagselrect.h"

#define ID_STATUS_MSG 1

KmagApp::KmagApp(QWidget* , const char* name)
	: KMainWindow(0, name, WStyle_MinMax | WType_TopLevel | WDestructiveClose | WStyle_ContextHelp | WStyle_StaysOnTop),
		m_zoomIndex(4)
{
  config=kapp->config();

	zoomArrayString << "20% [5:1]"  << "50% [2:1]"  << "75% [1.33:1]"  << "100% [1:1]"
    << "125% [1:1.25]"  << "150% [1:1.5]"  << "200% [1:2]"  << "300% [1:3]"
    << "400% [1:4]"  << "500% [1:5]" << "600% [1:6]" << "700% [1:7]"
    << "800% [1:8]" << "1200% [1:12]" << "1600% [1:16]" << "2000% [1:20]";

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
	
  readOptions();

	// Intialize all values:

	// set initial zoom to 2x
	setZoomIndex(7);
	emit updateZoomIndex(m_zoomIndex);

	// set mouse following to be off by default
	m_zoomView->setFollowMouse(false);
}

/**
 * Default destructor.
 */
KmagApp::~KmagApp()
{
}

void KmagApp::initActions()
{
  fileNewWindow = new KAction(i18n("New &Window"), 0, 0, this, SLOT(slotFileNewWindow()), actionCollection(),"file_new_window");

  refreshSwitch = KStdAction::zoom(this, SLOT(slotToggleRefresh()), actionCollection());
  refreshSwitch->setIcon("stop.png");
  refreshSwitch->setText(i18n("Stop Update"));
  refreshSwitch->setToolTip(i18n("Click to stop window refresh"));
  refreshSwitch->setWhatsThis(i18n("Clicking on this icon will <b>start</b> / <b>stop</b>\
  updating of the display. Stopping the update will zero the processing power\
  required (CPU usage)."));

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
  refreshSwitch->plug(toolBar());
  m_pZoomIn->plug(toolBar());
	m_pZoomBox->plug(toolBar());
  m_pZoomOut->plug(toolBar());
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
	connect(m_followMouseButton, SIGNAL(toggled(bool)), m_zoomView, SLOT(setFollowMouse(bool)));

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
//  config->writeEntry("Show Toolbar", viewToolBar->isChecked());
  config->writeEntry("ToolBarPos", (int) toolBar("mainToolBar")->barPos());
}


void KmagApp::readOptions()
{
	
  config->setGroup("General Options");

  // bar status settings
//  bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
//  viewToolBar->setChecked(bViewToolbar);
 // slotViewToolBar();

  // bar position settings
  KToolBar::BarPosition toolBarPos;
  toolBarPos=(KToolBar::BarPosition) config->readNumEntry("ToolBarPos", KToolBar::Top);
  toolBar("mainToolBar")->setBarPos(toolBarPos);
	
  QSize size=config->readSizeEntry("Geometry");
  if(!size.isEmpty())
  {
    resize(size);
  }
}

void KmagApp::saveProperties(KConfig *_cfg)
{
/*
  if(doc->URL().fileName()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    KURL url=doc->URL();	
    _cfg->writeEntry("filename", url.url());
    _cfg->writeEntry("modified", doc->isModified());
    QString tempname = kapp->tempSaveName(url.url());
    QString tempurl= KURL::encode_string(tempname);
    KURL _url(tempurl);
    doc->saveDocument(_url);
  }
*/
}


void KmagApp::readProperties(KConfig* _cfg)
{
/*
  QString filename = _cfg->readEntry("filename", "");
  KURL url(filename);
  bool modified = _cfg->readBoolEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);
    KURL _url(tempname);
  	
    if(canRecover)
    {
      doc->openDocument(_url);
      doc->setModified();
      setCaption(_url.fileName(),true);
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->openDocument(url);
      setCaption(url.fileName(),false);
    }
  }*/
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


void KmagApp::slotToggleRefresh()
{
  m_zoomView->toggleRefresh();
  if(m_zoomView->getRefreshStatus()) {
    refreshSwitch->setIcon("stop.png");
    refreshSwitch->setText(i18n("Stop Update"));
    refreshSwitch->setToolTip(i18n("Click to stop window update"));
  } else {
    refreshSwitch->setIcon("reload.png");
    refreshSwitch->setText(i18n("Start Update"));
    refreshSwitch->setToolTip(i18n("Click to start window update"));
  }
}


void KmagApp::slotFileNewWindow()
{	
  KmagApp *new_window= new KmagApp();
  new_window->show();
}


void KmagApp::slotFilePrint()
{
/*
  QPrinter printer;
  if (printer.setup(this))
  {
    view->print(&printer);
  }
*/
}

void KmagApp::slotFileQuit()
{
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if(memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
	break;
    }
  }	
}

void KmagApp::slotEditCopy()
{

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


