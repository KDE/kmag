/***************************************************************************
                          kmagselrect.cpp  -  description
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

#include "kmagselrect.h"
#include "kmagselrect.moc"

#include <tqapplication.h>
#include <tqcursor.h>
#include <tqdesktopwidget.h>
#include <tqpixmap.h>
#include <tqbitmap.h>

#include <kapplication.h>
#include <klocale.h>

static uchar line_bits[] = {0x2d, 0x96, 0x4b, 0xa5, 0xd2, 0x69, 0xb4, 0x5a};

static TQColor titleColor = TQColor (0,0,128);
static TQColor titleBtnColor = TQColor (255,255,0);
static TQColor textColor = TQColor (255,255,255);

static int frameSize = 10;
static int titleSize = 24;

void setTitleColors (TQColor title, TQColor text, TQColor titleBtn)
{
  titleColor = title;
  titleBtnColor = titleBtn;
  textColor = text;
}

void setFrameSize (int size)
{
  frameSize = size;
}

void setTitleSize (int size)
{
  titleSize = size;
}

TQColor getTitleColor ()
{
  return titleColor;
}

TQColor getTitleBtnColor ()
{
  return titleBtnColor;
}

TQColor getTextColor ()
{
  return textColor;
}

int getFrameSize ()
{
  return frameSize;
}

int getTitleSize ()
{
  if (titleSize > frameSize)
    return titleSize;
  else
    return frameSize;
}

//--------------------------------------------------------------------------
//   Construction
//--------------------------------------------------------------------------

KMagSelRect::KMagSelRect(TQWidget *parent) :
  TQRect()
{
  init(parent);
}

KMagSelRect::KMagSelRect(const TQPoint &topLeft, const TQPoint &bottomRight,
                 TQWidget *parent) :
TQRect(topLeft, bottomRight)
{
  init(parent);
}

KMagSelRect::KMagSelRect(const TQPoint &topLeft, const TQSize &size,
                 TQWidget *parent) :
TQRect(topLeft, size)
{
  init(parent);
}

KMagSelRect::KMagSelRect(int left, int top, int width, int height,
                 TQWidget *parent) :
TQRect(left, top, width, height)
{
  init(parent);
}

void KMagSelRect::init(TQWidget *parent)
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

WId KMagSelRect::winId()
{
  if (selectionwindow)
    return selectionwindow->winId();
  else
    return 0;
}

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
    selectionwindow = new KMagSelWin (selWindowParent, "selectionwindow");
    connect (selectionwindow, TQT_SIGNAL (resized ()), this, TQT_SLOT (selWinResized ()));

    update();
    selectionwindow->show();
    selWindowParent->setActiveWindow();
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
  // make sure the selection window does not go outside of the display
  if (height() > TQApplication::desktop()->tqgeometry().height())
    setHeight (TQApplication::desktop()->tqgeometry().height());
  if (width() > TQApplication::desktop()->tqgeometry().width())
    setWidth (TQApplication::desktop()->tqgeometry().width());

  if (top() < 0)
    moveTop (0);
  if (left() < 0)
    moveLeft (0);
  if (bottom() > TQApplication::desktop()->tqgeometry().bottom())
    moveBottom (TQApplication::desktop()->tqgeometry().bottom());
  if (right() > TQApplication::desktop()->tqgeometry().right())
    moveRight (TQApplication::desktop()->tqgeometry().right());

  if (selectionwindow != 0)
    selectionwindow->setSelRect (TQRect (topLeft(), bottomRight()));
}

void KMagSelRect::selWinResized()
{
  if (selectionwindow != 0)
  {
    TQRect newRect = selectionwindow->getSelRect();
    setRect (newRect.x(), newRect.y(), newRect.width(), newRect.height());
  }
}

//--------------------------------------------------------------------------
//   KMagSelWin
//--------------------------------------------------------------------------

KMagSelWin::KMagSelWin ( TQWidget * parent, const char * name, WFlags ) :
    TQWidget (parent, name, WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WType_TopLevel | WX11BypassWM)
{
  TQBitmap line (8, 8, line_bits, true);
  setPaletteBackgroundPixmap (line);
  setBackgroundOrigin (TQWidget::WindowOrigin);

  titleBar = new KMagSelWinCorner (this, "titlebar");
  titleBar->setPaletteBackgroundColor (getTitleColor ());
  titleBar->setPaletteForegroundColor (getTextColor ());
  titleBar->setText(i18n("Selection Window")+" - "+i18n("KMagnifier"));
  connect (titleBar, TQT_SIGNAL (startResizing ()), this, TQT_SLOT (startResizing ()));
  connect (titleBar, TQT_SIGNAL (resized (TQPoint)), this, TQT_SLOT (titleMoved (TQPoint)));

  topLeftCorner = new KMagSelWinCorner (this, "topleft");
  topLeftCorner->setCursor (TQt::SizeFDiagCursor);
  topLeftCorner->setPaletteBackgroundColor (getTitleBtnColor ());
  connect (topLeftCorner, TQT_SIGNAL (startResizing ()), this, TQT_SLOT (startResizing ()));
  connect (topLeftCorner, TQT_SIGNAL (resized (TQPoint)), this, TQT_SLOT (topLeftResized (TQPoint)));

  topRightCorner = new KMagSelWinCorner (this, "topright");
  topRightCorner->setCursor (TQt::SizeBDiagCursor);
  topRightCorner->setPaletteBackgroundColor (getTitleBtnColor ());
  connect (topRightCorner, TQT_SIGNAL (startResizing ()), this, TQT_SLOT (startResizing ()));
  connect (topRightCorner, TQT_SIGNAL (resized (TQPoint)), this, TQT_SLOT (topRightResized (TQPoint)));

  bottomLeftCorner = new KMagSelWinCorner (this, "bottomleft");
  bottomLeftCorner->setCursor (TQt::SizeBDiagCursor);
  bottomLeftCorner->setPaletteBackgroundColor (getTitleBtnColor ());
  connect (bottomLeftCorner, TQT_SIGNAL (startResizing ()), this, TQT_SLOT (startResizing ()));
  connect (bottomLeftCorner, TQT_SIGNAL (resized (TQPoint)), this, TQT_SLOT (bottomLeftResized (TQPoint)));

  bottomRightCorner = new KMagSelWinCorner (this, "bottomright");
  bottomRightCorner->setCursor (TQt::SizeFDiagCursor);
  bottomRightCorner->setPaletteBackgroundColor (getTitleBtnColor ());
  connect (bottomRightCorner, TQT_SIGNAL (startResizing ()), this, TQT_SLOT (startResizing ()));
  connect (bottomRightCorner, TQT_SIGNAL (resized (TQPoint)), this, TQT_SLOT (bottomRightResized (TQPoint)));
}

KMagSelWin::~KMagSelWin()
{
  delete titleBar;
  delete topLeftCorner;
  delete topRightCorner;
  delete bottomLeftCorner;
  delete bottomRightCorner;
}

void KMagSelWin::setSelRect (TQRect selRect)
{
  selRect = selRect.normalize();

  if (selRect.left() < 0)
    selRect.setLeft (0);
  if (selRect.top() < 0)
    selRect.setTop (0);
  if (selRect.right() > TQApplication::desktop()->width())
    selRect.setRight (TQApplication::desktop()->width());
  if (selRect.bottom() > TQApplication::desktop()->height())
    selRect.setBottom (TQApplication::desktop()->height());

  setGeometry (
      selRect.left() - getFrameSize(),
      selRect.top() - getTitleSize() - 2,
      selRect.width() + getFrameSize() + getFrameSize(),
      selRect.height() + getFrameSize() + getTitleSize()+2);

  int w = getFrameSize();
  if (selRect.width() < w+w)
    w = static_cast<int>(selRect.width()/2);

  int h = getFrameSize();
  if (selRect.height() < h+h)
    h = static_cast<int>(selRect.height()/2);

  setMask (TQRegion (TQRect (0, 0, width(), height ()))
           - TQRegion (TQRect (getFrameSize(), getTitleSize()+2, selRect.width(), selRect.height()))
           - TQRegion (TQRect (0, 0, getFrameSize()+w, getTitleSize()+2-getFrameSize()))
           - TQRegion (TQRect (width()-getFrameSize()-w, 0, getFrameSize()+w, getTitleSize()+2-getFrameSize()))
           - TQRegion (TQRect (0, getTitleSize()+2+h, getFrameSize()-2, selRect.height()-h-h))
           - TQRegion (TQRect (width()-getFrameSize()+2, getTitleSize()+2+h, getFrameSize()-2, selRect.height()-h-h))
           - TQRegion (TQRect (getFrameSize()+w, height()-getFrameSize()+2, selRect.width()-w-w, getFrameSize()-2)));

  titleBar->setGeometry (getFrameSize()+w, 0, selRect.width()-h-h, getTitleSize());
  topLeftCorner->setGeometry (0, getTitleSize()+2-getFrameSize(), getFrameSize()+w, getFrameSize()+h);
  topRightCorner->setGeometry (width()-getFrameSize()-w, getTitleSize()+2-getFrameSize(), getFrameSize()+w, getFrameSize()+h);
  bottomLeftCorner->setGeometry (0, height()-getFrameSize()-h, getFrameSize()+w, getFrameSize()+h);
  bottomRightCorner->setGeometry (width()-getFrameSize()-w, height()-getFrameSize()-h, getFrameSize()+w, getFrameSize()+h);
}

TQRect KMagSelWin::getSelRect ()
{
  return TQRect (
      x() + getFrameSize(),
      y() + getTitleSize()+2,
      width() - getFrameSize() - getFrameSize(),
      height() - getFrameSize() - getTitleSize()-2);
}

void KMagSelWin::startResizing ()
{
  oldSelRect = getSelRect();
}

void KMagSelWin::titleMoved ( TQPoint offset )
{
  TQRect selRect = oldSelRect;
  selRect.moveBy (offset.x(), offset.y());
  setSelRect (selRect);
  emit resized ();
}

void KMagSelWin::topLeftResized ( TQPoint offset )
{
  setSelRect (TQRect(oldSelRect.topLeft() + offset, oldSelRect.bottomRight ()));
  emit resized();
}

void KMagSelWin::topRightResized ( TQPoint offset )
{
  setSelRect (TQRect(oldSelRect.topRight() + offset, oldSelRect.bottomLeft ()));
  emit resized();
}

void KMagSelWin::bottomLeftResized ( TQPoint offset )
{
  setSelRect (TQRect(oldSelRect.bottomLeft() + offset, oldSelRect.topRight ()));
  emit resized();
}

void KMagSelWin::bottomRightResized ( TQPoint offset )
{
  setSelRect (TQRect(oldSelRect.bottomRight() + offset, oldSelRect.topLeft()));
  emit resized();
}


//--------------------------------------------------------------------------
//   KMagSelWinCorner
//--------------------------------------------------------------------------

KMagSelWinCorner::KMagSelWinCorner ( TQWidget * parent, const char * name, WFlags f ) :
    TQLabel (parent, name, f)
{
  setFrameStyle (TQFrame::WinPanel | TQFrame::Raised);
  setLineWidth (1);
}

KMagSelWinCorner::~KMagSelWinCorner()
{
}

void KMagSelWinCorner::mousePressEvent ( TQMouseEvent * e )
{
  oldPos = e->globalPos ();
  emit startResizing ();
}

void KMagSelWinCorner::mouseReleaseEvent ( TQMouseEvent * e )
{
  setFrameShadow (TQFrame::Raised);
  emit resized (e->globalPos () - oldPos);
}

void KMagSelWinCorner::mouseMoveEvent ( TQMouseEvent * e )
{
  emit resized (e->globalPos () - oldPos);
}
