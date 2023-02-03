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

// Qt
#include <QBitmap>
#include <QScreen>
#include <QMouseEvent>
// KF5
#include <KLocalizedString>

static const uchar line_bits[] = {0x2d, 0x96, 0x4b, 0xa5, 0xd2, 0x69, 0xb4, 0x5a};

static QColor titleColor = QColor (0,0,128);
static QColor titleBtnColor = QColor (255,255,0);
static QColor textColor = QColor (255,255,255);

static int frameSize = 10;
static int titleSize = 24;

void setTitleColors (const QColor &title, const QColor &text, const QColor &titleBtn)
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

QColor getTitleColor ()
{
  return titleColor;
}

QColor getTitleBtnColor ()
{
  return titleBtnColor;
}

QColor getTextColor ()
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

KMagSelRect::KMagSelRect(QWidget *parent) :
  QRect()
{
  init(parent);
}

KMagSelRect::KMagSelRect(const QPoint &topLeft, const QPoint &bottomRight,
                 QWidget *parent) :
QRect(topLeft, bottomRight)
{
  init(parent);
}

KMagSelRect::KMagSelRect(const QPoint &topLeft, const QSize &size,
                 QWidget *parent) :
QRect(topLeft, size)
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
  if (parent != nullptr)
    while (parent->parentWidget() != nullptr)
      parent=parent->parentWidget();

  selectionwindow = nullptr;
  selWindowParent = parent;

  m_alwaysVisible = false;
}

KMagSelRect::~KMagSelRect()
{
}

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------

bool KMagSelRect::visible() const
{
  return (selectionwindow != nullptr);
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
  if (selectionwindow == nullptr) {
    selectionwindow = new KMagSelWin (selWindowParent);
    selectionwindow->setObjectName( QStringLiteral("selectionwindow" ));
    connect (selectionwindow, &KMagSelWin::resized, this, &KMagSelRect::selWinResized);

    update();
    selectionwindow->show();
    selWindowParent->activateWindow();
  }
}

void KMagSelRect::hide()
{
  if(m_alwaysVisible)
    return;
  if (selectionwindow != nullptr) {
    selectionwindow->hide();
    delete selectionwindow;
    selectionwindow = nullptr;
  }
}

void KMagSelRect::update()
{
  if (selectionwindow)
    selectionwindow->setSelRect (QRect (topLeft(), bottomRight()));
}

void KMagSelRect::selWinResized()
{
  if (selectionwindow)
  {
    const QRect newRect = selectionwindow->getSelRect();
    setRect (newRect.x(), newRect.y(), newRect.width(), newRect.height());
  }
}

//--------------------------------------------------------------------------
//   KMagSelWin
//--------------------------------------------------------------------------

void setPaletteColor(QWidget* w, QPalette::ColorRole r, const QColor& c)
{
  QPalette p = w->palette();
  p.setColor(r, c);
  w->setPalette(p);
}

KMagSelWin::KMagSelWin ( QWidget * parent ) :
    QWidget(parent) //Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_StaysOnTop | Qt::WType_TopLevel | Qt::WX11BypassWM)
{
  setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);

  QPalette p = palette();
  p.setBrush(backgroundRole(), QBrush(QBitmap::fromData( QSize(8,  8),  line_bits)));
  setPalette(p);

  titleBar = new KMagSelWinCorner (this);
  titleBar->setObjectName( QStringLiteral("titlebar" ));
  setPaletteColor(titleBar, QPalette::Window, getTitleColor());
  setPaletteColor(titleBar, QPalette::WindowText, getTextColor());
  titleBar->setText(i18n("Selection Window")+QLatin1String( " - " )+i18n("KMagnifier"));
  connect (titleBar, &KMagSelWinCorner::startResizing, this, &KMagSelWin::startResizing);
  connect (titleBar, &KMagSelWinCorner::resized, this, &KMagSelWin::titleMoved);

  topLeftCorner = new KMagSelWinCorner (this);
  topLeftCorner->setObjectName( QStringLiteral("topleft" ));
  topLeftCorner->setCursor (Qt::SizeFDiagCursor);
  setPaletteColor(topLeftCorner, QPalette::Window, getTitleBtnColor());
  connect (topLeftCorner, &KMagSelWinCorner::startResizing, this, &KMagSelWin::startResizing);
  connect (topLeftCorner, &KMagSelWinCorner::resized, this, &KMagSelWin::topLeftResized);

  topRightCorner = new KMagSelWinCorner (this);
  topRightCorner->setObjectName( QStringLiteral("topright" ));
  topRightCorner->setCursor (Qt::SizeBDiagCursor);
  setPaletteColor(topRightCorner, QPalette::Window, getTitleBtnColor ());
  connect (topRightCorner, &KMagSelWinCorner::startResizing, this, &KMagSelWin::startResizing);
  connect (topRightCorner, &KMagSelWinCorner::resized, this, &KMagSelWin::topRightResized);

  bottomLeftCorner = new KMagSelWinCorner (this);
  bottomLeftCorner->setObjectName( QStringLiteral("bottomleft" ));
  bottomLeftCorner->setCursor (Qt::SizeBDiagCursor);
  setPaletteColor(bottomLeftCorner, QPalette::Window, getTitleBtnColor());
  connect (bottomLeftCorner, &KMagSelWinCorner::startResizing, this, &KMagSelWin::startResizing);
  connect (bottomLeftCorner, &KMagSelWinCorner::resized, this, &KMagSelWin::bottomLeftResized);

  bottomRightCorner = new KMagSelWinCorner (this);
  bottomRightCorner->setObjectName( QStringLiteral("bottomright" ));
  bottomRightCorner->setCursor (Qt::SizeFDiagCursor);
  setPaletteColor(bottomRightCorner, QPalette::Window, getTitleBtnColor ());
  connect (bottomRightCorner, &KMagSelWinCorner::startResizing, this, &KMagSelWin::startResizing);
  connect (bottomRightCorner, &KMagSelWinCorner::resized, this, &KMagSelWin::bottomRightResized);
}

KMagSelWin::~KMagSelWin()
{
  delete titleBar;
  delete topLeftCorner;
  delete topRightCorner;
  delete bottomLeftCorner;
  delete bottomRightCorner;
}

void KMagSelWin::setSelRect (const QRect &_selRect)
{
  QRect selRect = _selRect.normalized();

  if (selRect.left() < 0)
    selRect.setLeft (0);
  if (selRect.top() < 0)
    selRect.setTop (0);
  const QSize screenSize = screen()->virtualSize();
  if (selRect.right() > screenSize.width())
    selRect.setRight (screenSize.width());
  if (selRect.bottom() > screenSize.height())
    selRect.setBottom (screenSize.height());

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

  setMask (QRegion (QRect (0, 0, width(), height ()))
           - QRegion (QRect (getFrameSize(), getTitleSize()+2, selRect.width(), selRect.height()))
           - QRegion (QRect (0, 0, getFrameSize()+w, getTitleSize()+2-getFrameSize()))
           - QRegion (QRect (width()-getFrameSize()-w, 0, getFrameSize()+w, getTitleSize()+2-getFrameSize()))
           - QRegion (QRect (0, getTitleSize()+2+h, getFrameSize()-2, selRect.height()-h-h))
           - QRegion (QRect (width()-getFrameSize()+2, getTitleSize()+2+h, getFrameSize()-2, selRect.height()-h-h))
           - QRegion (QRect (getFrameSize()+w, height()-getFrameSize()+2, selRect.width()-w-w, getFrameSize()-2)));

  titleBar->setGeometry (getFrameSize()+w, 0, selRect.width()-h-h, getTitleSize());
  topLeftCorner->setGeometry (0, getTitleSize()+2-getFrameSize(), getFrameSize()+w, getFrameSize()+h);
  topRightCorner->setGeometry (width()-getFrameSize()-w, getTitleSize()+2-getFrameSize(), getFrameSize()+w, getFrameSize()+h);
  bottomLeftCorner->setGeometry (0, height()-getFrameSize()-h, getFrameSize()+w, getFrameSize()+h);
  bottomRightCorner->setGeometry (width()-getFrameSize()-w, height()-getFrameSize()-h, getFrameSize()+w, getFrameSize()+h);
}

QRect KMagSelWin::getSelRect ()
{
  return QRect (
      x() + getFrameSize(),
      y() + getTitleSize()+2,
      width() - getFrameSize() - getFrameSize(),
      height() - getFrameSize() - getTitleSize()-2);
}

void KMagSelWin::startResizing ()
{
  oldSelRect = getSelRect();
}

void KMagSelWin::titleMoved (const QPoint &offset)
{
  QRect selRect = oldSelRect;
  selRect.translate (offset.x(), offset.y());
  setSelRect (selRect);
  Q_EMIT resized ();
}

void KMagSelWin::topLeftResized (const QPoint &offset)
{
  setSelRect (QRect(oldSelRect.topLeft() + offset, oldSelRect.bottomRight ()));
  Q_EMIT resized();
}

void KMagSelWin::topRightResized (const QPoint &offset)
{
  setSelRect (QRect(oldSelRect.topRight() + offset, oldSelRect.bottomLeft ()));
  Q_EMIT resized();
}

void KMagSelWin::bottomLeftResized (const QPoint &offset)
{
  setSelRect (QRect(oldSelRect.bottomLeft() + offset, oldSelRect.topRight ()));
  Q_EMIT resized();
}

void KMagSelWin::bottomRightResized (const QPoint &offset)
{
  setSelRect (QRect(oldSelRect.bottomRight() + offset, oldSelRect.topLeft()));
  Q_EMIT resized();
}


//--------------------------------------------------------------------------
//   KMagSelWinCorner
//--------------------------------------------------------------------------

KMagSelWinCorner::KMagSelWinCorner ( QWidget * parent ) :
    QLabel (parent)
{
  setFrameStyle (QFrame::WinPanel | QFrame::Raised);
  setLineWidth (1);
}

KMagSelWinCorner::~KMagSelWinCorner()
{
}

void KMagSelWinCorner::mousePressEvent ( QMouseEvent * e )
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  oldPos = e->globalPos ();
#else
    oldPos = e->globalPosition().toPoint();
#endif
  Q_EMIT startResizing ();
}

void KMagSelWinCorner::mouseReleaseEvent ( QMouseEvent * e )
{
  setFrameShadow (QFrame::Raised);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QPoint p = e->globalPos ();
#else
  QPoint p = e->globalPosition().toPoint();
#endif
  Q_EMIT resized (p - oldPos);
}

void KMagSelWinCorner::mouseMoveEvent ( QMouseEvent * e )
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QPoint p = e->globalPos ();
#else
  QPoint p = e->globalPosition().toPoint();
#endif
  Q_EMIT resized (p - oldPos);
}
