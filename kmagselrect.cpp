/***************************************************************************
                          kmagselrect.cpp  -  description
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

#include "kmagselrect.h"

#include "qpixmap.h"
#include "qbitmap.h"

//--------------------------------------------------------------------------
//   Construction
//--------------------------------------------------------------------------

static uchar line_bits[] = {0x2d, 0x96, 0x4b, 0xa5, 0xd2, 0x69, 0xb4, 0x5a};

KMagSelRect::KMagSelRect(QWidget *parent) :
  QRect()
{
  init(parent);
}

KMagSelRect::KMagSelRect(const QPoint &topleft, const QPoint &bottomright,
                 QWidget *parent) :
QRect(topleft, bottomright)
{
  init(parent);
}

KMagSelRect::KMagSelRect(const QPoint &topleft, const QSize &size,
                 QWidget *parent) :
QRect(topleft, size)
{
  init(parent);
}

KMagSelRect::KMagSelRect(int left, int top, int width, int height,
                 QWidget *parent) :
QRect(left, top, width, height)
{
  init(parent);
}

void KMagSelRect::init(QWidget *parent)
{
  // Make sure parent is the window itself, not a widget within the window
  if (parent != 0)
    while (parent->parentWidget (true) != 0)
      parent=parent->parentWidget (true);

  selectionwindow = 0;
  selWindowParent = parent;

  m_alwaysVisible = false;
}

KMagSelRect::~KMagSelRect()
{
}

//--------------------------------------------------------------------------
//   
//--------------------------------------------------------------------------

bool KMagSelRect::visible()
{
  return (selectionwindow != 0);
}

void KMagSelRect::alwaysVisible(bool visible)
{
  m_alwaysVisible = visible;
}


//--------------------------------------------------------------------------
//   Slots
//--------------------------------------------------------------------------

void KMagSelRect::show()
{
  if (selectionwindow == 0) {
    selectionwindow = new KMagSelWin (selWindowParent, "selectionwindow", WStyle_Customize | WStyle_NoBorder | WStyle_Tool | WType_TopLevel | WDestructiveClose);
    QBitmap line (8, 8, line_bits, true);
    selectionwindow->setPaletteBackgroundPixmap (line);
    update();
    selectionwindow->show();
  }
}

void KMagSelRect::hide()
{
  if(m_alwaysVisible)
    return;
  if (selectionwindow != 0) {
    selectionwindow->hide();
    delete selectionwindow;
    selectionwindow = 0;
  }
}

void KMagSelRect::update()
{
  if (selectionwindow != 0) {
    QRect normRect = this->normalize();
    QRect geometry (min(normRect.left(), normRect.right()) - 2,
                    min(normRect.top(), normRect.bottom()) - 2,
                    abs(normRect.width()) + 4,
                    abs(normRect.height()) + 4);
    selectionwindow->setGeometry (geometry);

    normRect.moveTopLeft (QPoint (2,2));
    geometry.moveTopLeft (QPoint (0,0));
    selectionwindow->setMask (QRegion (geometry) - QRegion (normRect));
  }
  emit updated();
}

//--------------------------------------------------------------------------
//   KMagSelWin
//--------------------------------------------------------------------------

KMagSelWin::KMagSelWin ( QWidget * parent, const char * name, WFlags f ) :
    QWidget (parent, name, f)
{
}

KMagSelWin::~KMagSelWin()
{
}

void KMagSelWin::windowActivationChange ( bool )
{
  if (isActiveWindow())
    parentWidget ()->setActiveWindow();
}

void KMagSelWin::closeEvent ( QCloseEvent * e )
{
  e->ignore();
}
