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

#include <QApplication>
#include <QCommandLineParser>

#include <KAboutData>
#include <KLocalizedString>

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
  KAboutData aboutData(QLatin1String("kmag"), i18n("KMagnifier"), QLatin1String(KMAG_VERSION),
                                         i18n("Screen magnifier for the K Desktop Environment (KDE)"),
                                         KAboutLicense::GPL,
                                         i18n("Copyright 2001-2003 Sarang Lakare\nCopyright 2003-2004 Olaf Schmidt\nCopyright 2008 Matthew Woehlke"), QString(),
                                         QLatin1String("http://accessibility.kde.org/"));

  // about the authors
  aboutData.addAuthor(i18n("Sarang Lakare"),
                       i18n("Rewrite"),QLatin1String("sarang@users.sf.net"),
                       QLatin1String("http://www.cs.sunysb.edu/~lsarang/linux"));
  aboutData.addAuthor(i18n("Michael Forster"),
                       i18n("Original idea and author (KDE1)"), QLatin1String("forster@fmi.uni-passau.de"));
  aboutData.addAuthor(i18n("Olaf Schmidt"), i18n("Rework of the user interface, improved selection window, speed optimization, rotation, bug fixes"), QLatin1String("ojschmidt@kde.org"));
  aboutData.addCredit(i18n("Matthew Woehlke"), i18n("Color-blindness simulation"), QLatin1String("mw_triad@users.sourceforge.net"));
  aboutData.addCredit(i18n("Sebastian Sauer"), i18n("Focus tracking"), QLatin1String("sebsauer@kdab.com"));
  aboutData.addCredit(i18n("Claudiu Costin"), i18n("Some tips"), QLatin1String("claudiuc@work.ro"), QLatin1String("http://www.ro.kde.org"));

  QApplication app(argc, argv);
  KAboutData::setApplicationData(aboutData);

  if (app.isSessionRestored())
  {
    RESTORE(KmagApp);
  }
  else
  {
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    kmagapp = new KmagApp();
    kmagapp->show();
  }

  return app.exec();
}
