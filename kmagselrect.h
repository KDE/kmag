/***************************************************************************
                          kmagselrect.h  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sf.net
    copyright            : (C) 2003 by Olaf Schmidt
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

#ifndef KMAGSELRECT_H
#define KMAGSELRECT_H

#include <stdlib.h>

// Qt includes
#include <qapplication.h>
#include <qrect.h>
#include <qwidget.h>
#include <qcursor.h>

// X11 includes
#include <X11/Xlib.h>

// Min function
#define min(a,b) ((a) < (b) ? (a) : (b))

class KMagSelWin : public QWidget
{
    Q_OBJECT

public:
    
    KMagSelWin ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );

    virtual ~KMagSelWin();

protected:
    
    virtual void windowActivationChange ( bool oldActive );
    virtual void closeEvent ( QCloseEvent * e );
};

/**
 * This class stores the selected rectangular area for grabbing. It also displays the
 * rectangular area on demand.
 *
 * @author Original : Michael Forster
 * @author Current : Sarang Lakare
 */
class KMagSelRect : public QObject, public QRect
{
    Q_OBJECT

public:
    KMagSelRect(QWidget *parent=0);
    KMagSelRect(const QPoint &topleft, const QPoint &bottomright,
      QWidget *parent=0);
    KMagSelRect(const QPoint &topleft, const QSize &size,
      QWidget *parent=0);
    KMagSelRect(int left, int top, int width, int height,
      QWidget *parent=0);

    virtual ~KMagSelRect();

    bool visible();

    /// Makes the rectangle always visible
    void alwaysVisible(bool visible=true);

    /// Returns true if always visible is set
    bool getAlwaysVisible() const {
      return (m_alwaysVisible);
    };

public slots:

    void show();
    void hide();
    void update();

signals:

    void updated();

protected:

    void init(QWidget *);

    QWidget *selWindowParent;
    KMagSelWin *selectionwindow;
    bool m_alwaysVisible;

};

#endif // KMAGSELRECT_H
