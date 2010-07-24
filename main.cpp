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
#include <ktoggleaction.h>
#include <kselectaction.h>
#include <kapplication.h>
#include "kmag.h"

#include "version.h"

KmagApp *kmagapp;

// Not needed, not used.
//static const char description[] =
//  I18N_NOOP("Kmag");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE


int main(int argc, char *argv[])
{
  // about the application
  KAboutData *aboutData = new KAboutData("kmag", 0, ki18n("KMagnifier"), KMAG_VERSION,
                                         ki18n("Screen magnifier for the K Desktop Environment (KDE)"),
                                         KAboutData::License_GPL,
                                         ki18n("Copyright 2001-2003 Sarang Lakare\nCopyright 2003-2004 Olaf Schmidt\nCopyright 2008 Matthew Woehlke"), KLocalizedString(),
                                         "http://accessibility.kde.org/");

  // about the authors
  aboutData->addAuthor(ki18n("Sarang Lakare"),
                       ki18n("Rewrite"),"sarang@users.sf.net",
                       "http://www.cs.sunysb.edu/~lsarang/linux");
  aboutData->addAuthor(ki18n("Michael Forster"),
                       ki18n("Original idea and author (KDE1)"), "forster@fmi.uni-passau.de");
  aboutData->addAuthor(ki18n("Olaf Schmidt"), ki18n("Rework of the user interface, improved selection window, speed optimization, rotation, bug fixes"), "ojschmidt@kde.org");
  aboutData->addCredit(ki18n("Matthew Woehlke"), ki18n("Color-blindness simulation"), "mw_triad@users.sourceforge.net");
  aboutData->addCredit(ki18n("Sebastian Sauer"), ki18n("Focus tracking"), "sebsauer@kdab.com");
  aboutData->addCredit(ki18n("Claudiu Costin"), ki18n("Some tips"), "claudiuc@work.ro", "http://www.ro.kde.org");

  KCmdLineArgs::init( argc, argv, aboutData );

  KCmdLineOptions options;
  options.add("+[File]", ki18n("File to open"));
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
