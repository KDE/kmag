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


// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>

// include files for KDE
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>

// application specific includes
#include "kmag.moc"
#include "kmagselrect.h"
#include "kmagview.h"
#include "kmagdoc.h"

#define ID_STATUS_MSG 1

KmagApp::KmagApp(QWidget* , const char* name):KMainWindow(0, name, WStyle_MinMax | WType_TopLevel | WDestructiveClose | WStyle_ContextHelp | WStyle_StaysOnTop)
{
  config=kapp->config();

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
  initDocument();
  initView();
  initActions();


	
  readOptions();

  ///////////////////////////////////////////////////////////////////
  // disable actions at startup

  filePrint->setEnabled(false);
  editCopy->setEnabled(false);
}

KmagApp::~KmagApp()
{

}

void KmagApp::initActions()
{
  fileNewWindow = new KAction(i18n("New &Window"), 0, 0, this, SLOT(slotFileNewWindow()), actionCollection(),"file_new_window");
  filePrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  fileQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  editCopy = KStdAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  zoomIn = KStdAction::zoomIn(this, SLOT(slotZoomIn()), actionCollection());
  zoomOut = KStdAction::zoomOut(this, SLOT(slotZoomOut()), actionCollection());
  refreshSwitch = KStdAction::zoom(this, SLOT(slotToggleRefresh()), actionCollection());
  viewToolBar = KStdAction::showToolbar(this, SLOT(slotViewToolBar()), actionCollection());
  viewStatusBar = KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()), actionCollection());

  fileNewWindow->setStatusText(i18n("Opens a new application window"));
  filePrint ->setStatusText(i18n("Print the contents [in future!]"));
  fileQuit->setStatusText(i18n("Quits the application"));
  editCopy->setStatusText(i18n("Copies the magnified contents to the clipboard [in future!]"));
  viewToolBar->setStatusText(i18n("Enables/disables the toolbar"));
  viewStatusBar->setStatusText(i18n("Enables/disables the statusbar"));

  refreshSwitch->setIcon("stop.png");
  refreshSwitch->setText(i18n("Stop Update"));
  refreshSwitch->setToolTip(i18n("Click to stop window refresh"));
  refreshSwitch->setWhatsThis(i18n("Clicking on this icon will <b>start</b> / <b>stop</b>\
  updating of the display. Stopping the update will zero the processing power\
  required (CPU usage)."));

  zoomIn->setWhatsThis(i18n("Click on this button to <b>zoom-in</b> on the selected region."));
  zoomOut->setWhatsThis(i18n("Click on this button to <b>zoom-out</b> on the selected region."));

  // use the absolute path to your kmagui.rc file for testing purpose in createGUI();
  createGUI();
  // add zoomIn and zoomOut to the toolbar
  zoomIn->plug(toolBar());
  zoomOut->plug(toolBar());
  // create toolbar with the slider
  //m_zoomSlider = new KIntNumInput(2, toolBar(0), 10, "Zoom");
  //m_zoomSlider->setFixedWidth(256);
  //m_zoomSlider->setRange(1, 16, 1, true);
  //m_zoomSlider->setLabel(QString("Zoom"), AlignLeft | AlignVCenter);
  //toolBar("mainToolBar")->insertWidget(0, m_zoomSlider->sizeHint().width(), m_zoomSlider);
}


void KmagApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  // TODO: add your own items you need for displaying current application status.
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG);
}

void KmagApp::initDocument()
{
  doc = new KmagDoc(this);
  doc->newDocument();
}

void KmagApp::initView()
{ 
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KTMainWindow's view-region and
  // connect the widget to your document to display document contents.

  view = new KmagView(this);
  doc->addView(view);
  setCentralWidget(view);	
  //setCaption(doc->URL().fileName(),false);

}

void KmagApp::openDocumentFile(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."));

  doc->openDocument( url);
//  fileOpenRecent->addURL( url );
  slotStatusMsg(i18n("Ready."));
}


KmagDoc *KmagApp::getDocument() const
{
  return doc;
}

void KmagApp::saveOptions()
{	
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
  config->writeEntry("Show Toolbar", viewToolBar->isChecked());
  config->writeEntry("Show Statusbar",viewStatusBar->isChecked());
  config->writeEntry("ToolBarPos", (int) toolBar("mainToolBar")->barPos());
  //fileOpenRecent->saveEntries(config,"Recent Files");
}


void KmagApp::readOptions()
{
	
  config->setGroup("General Options");

  // bar status settings
  bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
  viewToolBar->setChecked(bViewToolbar);
  slotViewToolBar();

  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  viewStatusBar->setChecked(bViewStatusbar);
  slotViewStatusBar();


  // bar position settings
  KToolBar::BarPosition toolBarPos;
  toolBarPos=(KToolBar::BarPosition) config->readNumEntry("ToolBarPos", KToolBar::Top);
  toolBar("mainToolBar")->setBarPos(toolBarPos);
	
  // initialize the recent file list
  //fileOpenRecent->loadEntries(config,"Recent Files");

  QSize size=config->readSizeEntry("Geometry");
  if(!size.isEmpty())
  {
    resize(size);
  }
}

void KmagApp::saveProperties(KConfig *_cfg)
{
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
}


void KmagApp::readProperties(KConfig* _cfg)
{
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
  }
}		

bool KmagApp::queryClose()
{
  return doc->saveModified();
}

bool KmagApp::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KmagApp::slotSetZoom(float zoom)
{
  view->slotSetZoom(zoom);

  // check if zoomIn icon should be enabled or disabled
  if(!view->zoomInOk()) {
    // meaning that no more zooming-in is possible
    // -> disable zoom-in icon
    zoomIn->setEnabled(false);
  } else
    zoomIn->setEnabled(true);

  // check if zoomIn icon should be enabled or disabled
  if(!view->zoomOutOk()) {
    // meaning that no more zooming-out is possible
    // -> disable zoom-in icon
    zoomOut->setEnabled(false);
  } else
    zoomOut->setEnabled(true);
}


void KmagApp::slotZoomIn()
{
  view->slotZoomIn();

  if(!view->zoomInOk()) {
    // meaning that no more zooming-in is possible
    // -> disable zoom-in icon
    zoomIn->setEnabled(false);
  }

  // check if zoomOut icon was disabled.. enable it
  if(!zoomOut->isEnabled())
    zoomOut->setEnabled(true);

}

void KmagApp::slotZoomOut()
{
  view->slotZoomOut();

  if(!view->zoomOutOk()) {
    // meaning that no more zooming-in is possible
    // -> disable zoom-in icon
    zoomOut->setEnabled(false);
  }

  // check if zoomIn icon was disabled.. enable it
  if(!zoomIn->isEnabled())
    zoomIn->setEnabled(true);
}


void KmagApp::slotToggleRefresh()
{
  view->toggleRefresh();
  if(view->getRefreshStatus()) {
    refreshSwitch->setIcon("stop.png");
    refreshSwitch->setText(i18n("Stop Update"));
    refreshSwitch->setToolTip(i18n("Click to stop window update"));
    slotStatusMsg(i18n("Ready."));
  } else {
    refreshSwitch->setIcon("reload.png");
    refreshSwitch->setText(i18n("Start Update"));
    refreshSwitch->setToolTip(i18n("Click to start window update"));
    slotStatusMsg(i18n("Window update stopped."));
  }
}


void KmagApp::slotFileNewWindow()
{
  slotStatusMsg(i18n("Opening a new application window..."));
	
  KmagApp *new_window= new KmagApp();
  new_window->show();

  slotStatusMsg(i18n("Ready."));
}


void KmagApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));

  QPrinter printer;
  if (printer.setup(this))
  {
    view->print(&printer);
  }

  slotStatusMsg(i18n("Ready."));
}

void KmagApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
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
  slotStatusMsg(i18n("Ready."));
}

void KmagApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to clipboard..."));

  slotStatusMsg(i18n("Ready."));
}

void KmagApp::slotViewToolBar()
{
  slotStatusMsg(i18n("Toggling toolbar..."));
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

  slotStatusMsg(i18n("Ready."));
}

void KmagApp::slotViewStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if(!viewStatusBar->isChecked())
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  slotStatusMsg(i18n("Ready."));
}


void KmagApp::slotStatusMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

