/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sf.net
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

#include "version.h"

KmagApp *kmagapp;

// Not needed, not used.
//static const char description[] =
//  I18N_NOOP("Kmag");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("File to open"), 0 },
  KCmdLineLastOption
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
  // about the application
  KAboutData *aboutData = new KAboutData("kmag", I18N_NOOP("KMagnifier"), KMAG_VERSION,
                                         I18N_NOOP("Screen magnifier for the K Desktop Environment (KDE)"),
                                         KAboutData::License_GPL,
                                         "(C) 2001-2003, Sarang Lakare","",
                                         "http://kmag.sourceforge.net");

  // about the authors
  aboutData->addAuthor("Sarang Lakare",
                       I18N_NOOP("Rewrite and current maintainer"),"sarang@users.sf.net",
                       "http://www.cs.sunysb.edu/~lsarang/linux");
  aboutData->addAuthor("Michael Forster",
                       I18N_NOOP("Original idea and author (KDE1)"), "forster@fmi.uni-passau.de");

  aboutData->addCredit("Olaf Schmidt", I18N_NOOP("Rework of the user interface, improved selection window, speed optimisation, rotation, bug fixes"), "ojschmidt@kde.org");
  aboutData->addCredit("Claudiu Costin", I18N_NOOP("Some tips"), "claudiuc@work.ro",
                       "http://www.ro.kde.org");

  KCmdLineArgs::init( argc, argv, aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication app;

  if (app.isSessionRestored())
  {
    RESTORE(KmagApp);
  }
  else
  {
    kmagapp = new KmagApp();
    kmagapp->show();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    args->clear();
  }

  return app.exec();
}
