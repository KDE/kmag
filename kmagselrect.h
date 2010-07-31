/***************************************************************************
                          kmagselrect.h  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sf.net
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

#ifndef KMAGSELRECT_H
#define KMAGSELRECT_H

#include <stdlib.h>

// Qt includes
#include <tqrect.h>
#include <tqwidget.h>
#include <tqlabel.h>

class KMagSelWinCorner : public QLabel
{
    Q_OBJECT

public:

    KMagSelWinCorner ( TQWidget * parent = 0, const char * name = 0, WFlags f = 0 );

    virtual ~KMagSelWinCorner();

signals:

    void startResizing ();
    void resized ( TQPoint offset );

protected:

    TQPoint oldPos;

    virtual void mousePressEvent ( TQMouseEvent * e );
    virtual void mouseReleaseEvent ( TQMouseEvent * e );
    virtual void mouseMoveEvent ( TQMouseEvent * e );
};

class KMagSelWin : public QWidget
{
    Q_OBJECT

public:

    KMagSelWin ( TQWidget * parent = 0, const char * name = 0, WFlags f = 0 );

    virtual ~KMagSelWin();

    void setSelRect ( TQRect selRect );
    TQRect getSelRect ();

public slots:

    void startResizing ();
    void titleMoved ( TQPoint offset );
    void topLeftResized ( TQPoint offset );
    void topRightResized ( TQPoint offset );
    void bottomLeftResized ( TQPoint offset );
    void bottomRightResized ( TQPoint offset );

signals:

    void resized();

protected:

    TQRect oldSelRect;

    KMagSelWinCorner *titleBar;
    KMagSelWinCorner *topLeftCorner;
    KMagSelWinCorner *topRightCorner;
    KMagSelWinCorner *bottomLeftCorner;
    KMagSelWinCorner *bottomRightCorner;
};

/**
 * This class stores the selected rectangular area for grabbing. It also displays the
 * rectangular area on demand.
 *
 * @author Original : Michael Forster
 * @author Current : Sarang Lakare
 */
class KMagSelRect : public TQObject, public QRect
{
    Q_OBJECT

public:
    KMagSelRect(TQWidget *parent=0);
    KMagSelRect(const TQPoint &topLeft, const TQPoint &bottomRight,
      TQWidget *parent=0);
    KMagSelRect(const TQPoint &topLeft, const TQSize &size,
      TQWidget *parent=0);
    KMagSelRect(int left, int top, int width, int height,
      TQWidget *selWindowParent=0);

    virtual ~KMagSelRect();

    WId winId();

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

    void selWinResized();

protected:

    void init(TQWidget *);

    TQWidget *selWindowParent;
    KMagSelWin *selectionwindow;
    bool m_alwaysVisible;

};

void setTitleColors (TQColor title, TQColor text, TQColor titleBtn);
void setFrameSize (int size);

#endif // KMAGSELRECT_H
