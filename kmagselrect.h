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

// Qt
#include <QWidget>
#include <QLabel>

class QMouseEvent;

class KMagSelWinCorner : public QLabel
{
    Q_OBJECT

public:

    explicit KMagSelWinCorner ( QWidget * parent = nullptr );

    virtual ~KMagSelWinCorner();

Q_SIGNALS:

    void startResizing ();
    void resized ( QPoint offset );

protected:

    QPoint oldPos;

    void mousePressEvent ( QMouseEvent * e ) override;
    void mouseReleaseEvent ( QMouseEvent * e ) override;
    void mouseMoveEvent ( QMouseEvent * e ) override;
};

class KMagSelWin : public QWidget
{
    Q_OBJECT

public:

    explicit KMagSelWin ( QWidget * parent = nullptr );

    virtual ~KMagSelWin();

    void setSelRect ( const QRect & selRect );
    QRect getSelRect ();

public Q_SLOTS:

    void startResizing ();
    void titleMoved ( const QPoint & offset );
    void topLeftResized ( const QPoint & offset );
    void topRightResized ( const QPoint & offset );
    void bottomLeftResized ( const QPoint & offset );
    void bottomRightResized ( const QPoint & offset );

Q_SIGNALS:

    void resized();

protected:

    QRect oldSelRect;

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
class KMagSelRect : public QObject, public QRect
{
    Q_OBJECT

public:
    explicit KMagSelRect(QWidget *parent=nullptr);
    KMagSelRect(const QPoint &topLeft, const QPoint &bottomRight,
      QWidget *parent=nullptr);
    KMagSelRect(const QPoint &topLeft, const QSize &size,
      QWidget *parent=nullptr);
    KMagSelRect(int left, int top, int width, int height,
      QWidget *selWindowParent=nullptr);

    virtual ~KMagSelRect();

    bool visible();

    /// Makes the rectangle always visible
    void alwaysVisible(bool visible=true);

    /// Returns true if always visible is set
    bool getAlwaysVisible() const {
      return (m_alwaysVisible);
    }

public Q_SLOTS:

    void show();
    void hide();
    void update();

    void selWinResized();

protected:

    void init(QWidget *);

    QWidget *selWindowParent;
    KMagSelWin *selectionwindow;
    bool m_alwaysVisible;

};

void setTitleColors (const QColor & title, const QColor & text, const QColor & titleBtn);
void setFrameSize (int size);

#endif // KMAGSELRECT_H
