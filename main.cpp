/***************************************************************************
                          main.cpp  -  description
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


#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "kmag.h"

static const char *description =
	I18N_NOOP("Kmag");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("file to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
	// about the application
  KAboutData *aboutData = new KAboutData("kmag", I18N_NOOP("K Magnifier"), "0.3",
                                         I18N_NOOP("Screen Magnifier for KDE2"),
                                         KAboutData::License_GPL,
                                         "(C) 2001, Sarang Lakare","",
                                         "http://www.cs.sunysb.edu/~lsarang");

  // about the authors
  aboutData->addAuthor("Sarang Lakare",
                       "KDE2 port + ReWrite + Current Maintainer","sarang@users.sourceforge.net",
                       "http://www.cs.sunysb.edu/~lsarang");

  aboutData->addAuthor("Santanu Chaudhari",
                       "Current Maintainer","sachaudh@ic..sunysb.edu", NULL);

  aboutData->addAuthor("Michael Forster",
                       "Original Idea & Author (KDE1)", "forster@fmi.uni-passau.de");

	KCmdLineArgs::init( argc, argv, aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication app;
 
  if (app.isRestored())
  {
    RESTORE(KmagApp);
  }
  else 
  {
    KmagApp *kmag = new KmagApp();
    kmag->show();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
		
		if (args->count())
		{
        kmag->openDocumentFile(args->arg(0));
		}
		else
		{
		  kmag->openDocumentFile();
		}
		args->clear();
  }

  return app.exec();
}  
