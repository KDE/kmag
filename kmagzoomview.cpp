/***************************************************************************
                          kmagview.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sourceforge.net
    copyright            : (C) 2003-2005 by Olaf Schmidt
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


// application specific includes
#include "kmagzoomview.h"
#include "kmagzoomview.moc"

// include files for Qt
#include <qbitmap.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qcursor.h>
#include <qglobal.h>
#include <qpainter.h>
#include <qwhatsthis.h>
#include <qwidget.h>

// include files for KDE
#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <klocale.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

// include bitmaps for cursors
static uchar left_ptr_bits[] = {
   0x00, 0x00, 0x08, 0x00, 0x18, 0x00, 0x38, 0x00, 0x78, 0x00, 0xf8, 0x00,
   0xf8, 0x01, 0xf8, 0x03, 0xf8, 0x07, 0xf8, 0x00, 0xd8, 0x00, 0x88, 0x01,
   0x80, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00};

static uchar left_ptrmsk_bits[] = {
   0x0c, 0x00, 0x1c, 0x00, 0x3c, 0x00, 0x7c, 0x00, 0xfc, 0x00, 0xfc, 0x01,
   0xfc, 0x03, 0xfc, 0x07, 0xfc, 0x0f, 0xfc, 0x0f, 0xfc, 0x01, 0xdc, 0x03,
   0xcc, 0x03, 0x80, 0x07, 0x80, 0x07, 0x00, 0x03};

static uchar phand_bits[] = {
  0x00, 0x00, 0x00, 0x00,  0xfe, 0x01, 0x00, 0x00,  0x01, 0x02, 0x00, 0x00,
  0x7e, 0x04, 0x00, 0x00,  0x08, 0x08, 0x00, 0x00,  0x70, 0x08, 0x00, 0x00,
  0x08, 0x08, 0x00, 0x00,  0x70, 0x14, 0x00, 0x00,  0x08, 0x22, 0x00, 0x00,
  0x30, 0x41, 0x00, 0x00,  0xc0, 0x20, 0x00, 0x00,  0x40, 0x12, 0x00, 0x00,
  0x80, 0x08, 0x00, 0x00,  0x00, 0x05, 0x00, 0x00,  0x00, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00 };
    static uchar phandm_bits[] = {
  0xfe, 0x01, 0x00, 0x00,  0xff, 0x03, 0x00, 0x00,  0xff, 0x07, 0x00, 0x00,
  0xff, 0x0f, 0x00, 0x00,  0xfe, 0x1f, 0x00, 0x00,  0xf8, 0x1f, 0x00, 0x00,
  0xfc, 0x1f, 0x00, 0x00,  0xf8, 0x3f, 0x00, 0x00,  0xfc, 0x7f, 0x00, 0x00,
  0xf8, 0xff, 0x00, 0x00,  0xf0, 0x7f, 0x00, 0x00,  0xe0, 0x3f, 0x00, 0x00,
  0xc0, 0x1f, 0x00, 0x00,  0x80, 0x0f, 0x00, 0x00,  0x00, 0x07, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00 };



static bool obscuredRegion (QRegion &region, Window winId, Window ignoreId, Window start = 0, int level = -1) {
  Window root, parent, *children; uint nchildren;
  if (0 == start)
    start = qt_xrootwin();

  bool winIdFound = false;
  if (0 != XQueryTree (qt_xdisplay(), start, &root, &parent, &children, &nchildren)) {
    for (uint i=0; i < nchildren; ++i) {
      if (winIdFound) {
        if (ignoreId != children [i]) {
          XWindowAttributes atts;
          XGetWindowAttributes (qt_xdisplay(), children [i], &atts);
          if (atts.map_state == IsViewable)
            region -= QRegion (atts.x, atts.y, atts.width, atts.height, QRegion::Rectangle);
        }
      }
      else if (winId == children [i])
        winIdFound = true;

      // According to tests, my own window ID is either on toplevel or two levels below.
      // To avoid unneccessary recursion, we limit the search to two recursion levels,
      // then to five recursion levels, and make a full recursive search only if that
      // was unsuccessful.
      else if (level > 1)
        winIdFound = obscuredRegion (region, winId, ignoreId, children [i], level-1);
      else if (level == -1)
        if (! (winIdFound = obscuredRegion (region, winId, ignoreId, children [i], 0)))
          if (! (winIdFound = obscuredRegion (region, winId, ignoreId, children [i], 1)))
            winIdFound = obscuredRegion (region, winId, ignoreId, children [i], -1);
    }

    if (children != NULL)
      XFree (children);
  }

  return winIdFound;
}




KMagZoomView::KMagZoomView(QWidget *parent, const char *name)
  : QScrollView(parent, name),
    m_selRect(0, 0, 128, 128, this),
    m_grabTimer(parent),
    m_mouseViewTimer(parent),
    m_latestCursorPos(0,0),
    m_followMouse(false),
    m_showMouse(1),
    m_zoom(1.0),
    m_rotation(0),
    m_invert(false),
    m_fitToWindow(true)
{
  KApplication::setGlobalMouseTracking(TRUE);
  viewport()->setMouseTracking(TRUE);
  viewport()->setBackgroundMode (NoBackground);
  viewport()->setFocusPolicy(QWidget::StrongFocus);
  
  m_ctrlKeyPressed = false;
  m_shiftKeyPressed = false;
  m_refreshSwitch = true;
  m_refreshSwitchStateOnHide = m_refreshSwitch;
  
  // set the refresh rate
  setRefreshRate(10);

  // connect it to grabFrame()
  connect(&m_grabTimer, SIGNAL(timeout()), SLOT(grabFrame()));
  // start the grabTimer
  m_grabTimer.start(static_cast<int>(1000.0/m_fps));

  // connect it to updateMouseView()
  connect(&m_mouseViewTimer, SIGNAL(timeout()), SLOT(updateMouseView()));
  // start the grabTimer @ 25 frames per second!
  m_mouseViewTimer.start(25);

  QWhatsThis::add(this, i18n("This is the main window which shows the contents of the\
 selected region. The contents will be magnified according to the zoom level that is set."));

  // different ways to show the cursor.
  m_showMouseTypes << "Hidden" << "Box" << "Arrow" << "Actual";

  updateMatrix();
}

KMagZoomView::~KMagZoomView()
{
  KApplication::setGlobalMouseTracking(FALSE);
}

/**
 * This function will set/reset mouse following of grab window.
 */
void KMagZoomView::followMouse(bool follow)
{
  if(follow) {
    m_followMouse = true;
    m_mouseMode = Normal;
    setVScrollBarMode (QScrollView::AlwaysOff);
    setHScrollBarMode (QScrollView::AlwaysOff);
  } else {
    m_followMouse = false;
    m_mouseMode = Normal;
    setVScrollBarMode (QScrollView::AlwaysOn);
    setHScrollBarMode (QScrollView::AlwaysOn);
  }
}

/**
 * Called when the widget is hidden. Stop refresh when this happens.
 */
void KMagZoomView::hideEvent( QHideEvent* )
{
  // Save the state of the refresh switch.. the state will be restored
  // when showEvent is called
  m_refreshSwitchStateOnHide = m_refreshSwitch;

  // Check if refresh is ON
  if(m_refreshSwitch) {
    toggleRefresh();
  }
}


/**
 * Called when the widget is shown. Start refresh when this happens.
 */
void KMagZoomView::showEvent( QShowEvent* )
{
  // Check if refresh switch was ON when hide was called and if currently it is OFF
  if(m_refreshSwitchStateOnHide && !m_refreshSwitch) {
    // start the refresh in that case
    toggleRefresh();
  }
}

/**
 * Called when the widget is resized. Check if fitToWindow is active when this happens.
 */
void KMagZoomView::resizeEvent( QResizeEvent * e )
{
  QScrollView::resizeEvent (e);
  if(m_fitToWindow)
    fitToWindow();
}

/**
 * Called when the widget is to be repainted
 *
 * @param p
 */
void KMagZoomView::drawContents ( QPainter * p, int clipx, int clipy, int clipw, int cliph )
{
  if(m_grabbedPixmap.isNull())
    return;

  // A pixmap which will be eventually displayed
  QRect areaToPaint = m_invertedMatrix.mapRect (QRect(clipx, clipy, clipw, cliph));
  QPixmap clippedPixmap (areaToPaint.size());
  clippedPixmap.fill (QColor (128, 128, 128));
  areaToPaint &= QRect (QPoint (0,0), m_selRect.size());
  bitBlt(&clippedPixmap, QPoint (0,0), &m_grabbedPixmap, areaToPaint);

  // show the pixel under mouse cursor
  if(m_showMouse) {
    // paint the mouse cursor
    paintMouseCursor(&clippedPixmap, calcMousePos (m_refreshSwitch)-QPoint (areaToPaint.x(), areaToPaint.y()));
  }

  QPixmap zoomedPixmap;
  zoomedPixmap = clippedPixmap.xForm (m_zoomMatrix);

  if (m_invert) {
    QImage zoomedImage;
    zoomedImage = zoomedPixmap.convertToImage();
    zoomedImage.invertPixels (false);
    p->drawImage (QPoint (clipx-contentsX(), clipy-contentsY()), zoomedImage, zoomedImage.rect(),
                  Qt::ThresholdDither | Qt::ThresholdAlphaDither | Qt::AvoidDither);
  } else {
    p->drawPixmap (QPoint (clipx, clipy), zoomedPixmap, zoomedPixmap.rect());
  }
}

/**
 * Draws the mouse cursor according to the current selection of the type of
 * mouse cursor to draw.
 */
void KMagZoomView::paintMouseCursor(QPaintDevice *dev, QPoint mousePos)
{
  if(!dev)
    return;

  QPainter pz(dev);

  if(m_latestCursorPos.x() >= 0 && m_latestCursorPos.x() < m_selRect.width() &&
     m_latestCursorPos.y() >= 0 && m_latestCursorPos.y() < m_selRect.height() ) {
    // mouse position is indeed inside the selRect
      
    // How to show the mouse :

    switch(m_showMouse) {
    case 1:
      // 1. Square around the pixel
      pz.setPen(Qt::white);
      pz.setRasterOp(Qt::XorROP);
      pz.drawRect(mousePos.x()-1, mousePos.y()-1, 3, 3);
      break;

    case 2:
    {
      // 2. Arrow cursor
      pz.setPen(Qt::black);
      pz.setBackgroundColor(Qt::white);

      QBitmap sCursor( 16, 16, left_ptr_bits, TRUE );
      QBitmap mask( 16, 16, left_ptrmsk_bits, TRUE );
      sCursor.setMask(mask);

      // since hot spot is at 3,1
      pz.drawPixmap(mousePos.x()-3, mousePos.y()-1, sCursor);
    }
    break;

    case 3:
    {    
      // 3. Actual cursor
      // Get the current cursor type
      QWidget *dummy  = KApplication::widgetAt(QCursor::pos(), FALSE);
      if(!dummy)
        break;
      kdDebug() << ">" << dummy->name() << ":" << dummy->cursor().shape() << "-" << endl;
      switch(this->cursor().shape())  {
        case ArrowCursor :
         {
          // 2. Arrow cursor
          pz.setPen(Qt::black);
          pz.setBackgroundColor(Qt::white);

          QBitmap sCursor( 16, 16, left_ptr_bits, TRUE );
          QBitmap mask( 16, 16, left_ptrmsk_bits, TRUE );
          sCursor.setMask(mask);

          // since hot spot is at 3,1
          pz.drawPixmap(mousePos.x()-3, mousePos.y()-1, sCursor);
        }
        break;
        default:
          QBitmap sCursor( 32, 32, phand_bits, TRUE );
          QBitmap mask( 32, 32, phandm_bits, TRUE );
          sCursor.setMask(mask);

          pz.drawPixmap(mousePos.x(), mousePos.y(), sCursor);
        break;
      } // switch(cursor)


    }
    break;

    default:
      // do not show anything
      break;
    } // switch(m_showMouse)
  }
}


QPoint KMagZoomView::calcMousePos(bool updateMousePos)
{
  // get position of mouse wrt selRect
  if(updateMousePos) { // get a new position only if asked
    m_latestCursorPos = QCursor::pos();
    m_latestCursorPos -= QPoint(m_selRect.x(), m_selRect.y());
  }

  // get coordinates of the pixel w.r.t. the pixmap
  return QPoint (m_latestCursorPos.x(), m_latestCursorPos.y());
}


// MOUSE ACTIONS

/**
 * Called when mouse is clicked inside the window
 *
 * @param e
 */
void KMagZoomView::mousePressEvent(QMouseEvent *e)
{
  switch(e->button()) {
  case QMouseEvent::LeftButton :
    if(m_ctrlKeyPressed) {
      // check if currently in resize mode
      // don't do anything if fitToWindow is enabled
      if ((m_mouseMode != ResizeSelection) && !m_fitToWindow) {
        // set the mode to ResizeSelection
        m_mouseMode = ResizeSelection;

        // set mouse cursor to "resize all direction"
        setCursor(sizeAllCursor);

        // backup the old position
        m_oldMousePos.setX(e->globalX());
        m_oldMousePos.setY(e->globalY());

        // set the cursor position to the bottom-right of the selected region
        QCursor::setPos(m_selRect.bottomRight());

        // show the selection rectangle
        m_selRect.show();
      }
      else {
        #if QT_VERSION >= 300
        // ignore this button press.. so it goes to the parent
        e->ignore();
        #endif
      }
    } else if(m_shiftKeyPressed) {
      // check if currently in move mode
      // don't do anything if follow mouse is enabled
      if ((m_mouseMode != MoveSelection) && !m_followMouse) {
        m_mouseMode = MoveSelection;

        // set mouse cursor to cross hair
        setCursor(crossCursor);

        // backup the old position
        m_oldMousePos.setX(e->globalX());
        m_oldMousePos.setY(e->globalY());

        // set the cursor position to the center of the selected region
        QCursor::setPos(m_selRect.center());

        // show the selected rectangle
        m_selRect.show();
      }
      else {
        #if QT_VERSION >= 300
        // ignore this button press.. so it goes to the parent
        e->ignore();
        #endif
      }
    } else {
      // check if currently in move mode
      // don't do anything if follow mouse is enabled
      if ((m_mouseMode != GrabSelection) && !m_followMouse) {
        m_mouseMode = GrabSelection;

        // set mouse cursor to hand
        setCursor(KCursor::handCursor());

        // store the old position
        m_oldMousePos.setX(e->globalX());
        m_oldMousePos.setY(e->globalY());

        m_oldCenter = m_selRect.center();

        // show the selected rectangle
        m_selRect.show();
      }
      else {
        #if QT_VERSION >= 300
        // ignore this button press.. so it goes to the parent
        e->ignore();
        #endif
      }
    }
    break;

  case QMouseEvent::MidButton :
    // check if currently in move mode
    // don't do anything if follow mouse is enabled
    if ((m_mouseMode != MoveSelection) && !m_followMouse) {
      m_mouseMode = MoveSelection;

      // set mouse cursor to cross hair
      setCursor(crossCursor);

      // backup the old position
      m_oldMousePos.setX(e->globalX());
      m_oldMousePos.setY(e->globalY());

      // set the cursor position to the center of the selected region
      QCursor::setPos(m_selRect.center());

      // show the selected rectangle
      m_selRect.show();
    }
    else {
      #if QT_VERSION >= 300
      // ignore this button press.. so it goes to the parent
      e->ignore();
      #endif
    }
    break;
  // do nothing
  default:
#if QT_VERSION >= 300
    // ignore this button press.. so it goes to the parent
    e->ignore();
#endif
    break;
  }
}


/**
 * Called when a mouse button is released
 *
 * @param e
 */
void KMagZoomView::mouseReleaseEvent(QMouseEvent *e)
{
  switch(e->button()) {
  case QMouseEvent::LeftButton :
  case QMouseEvent::MidButton :
    // check if currently in move mode
    if(m_mouseMode == MoveSelection) {
      // hide the selection window
      m_selRect.hide();
      // set the mouse mode to normal
      m_mouseMode = Normal;

      // restore the cursor shape
      setCursor(arrowCursor);

      // restore the cursor position
      QCursor::setPos(m_oldMousePos);
    } else if(m_mouseMode == ResizeSelection) {
      // hide the selection window
      m_selRect.hide();
      // set the mouse mode to normal
      m_mouseMode = Normal;

      // restore the cursor shape
      setCursor(arrowCursor);

      // restore the cursor position
      QCursor::setPos(m_oldMousePos);
    } else if(m_mouseMode == GrabSelection) {
      // hide the selection window
      m_selRect.hide();

      // set the mouse mode to normal
      m_mouseMode = Normal;

      // restore the cursor shape
      setCursor(arrowCursor);
    }    
    break;

  case QMouseEvent::RightButton :
    break;
  case QMouseEvent::NoButton :
    break;

  // do nothing
  default:
    ;
  }
}


/**
 * Called when mouse is moved inside the window
 *
 * @param e
 */
void KMagZoomView::mouseMoveEvent(QMouseEvent *e)
{
  if(m_mouseMode == ResizeSelection) {
    // In resize selection mode
    // set the current mouse position as the bottom, right corner
    m_selRect.setRight(e->globalX());
    m_selRect.setBottom(e->globalY());
    m_selRect.update();

    grabFrame();
  } else if(m_mouseMode == MoveSelection) {
    QPoint newCenter;

    // set new center to be the current mouse position
    m_selRect.moveCenter(e->globalPos());
    m_selRect.update();

    grabFrame();
  } else if(m_mouseMode == GrabSelection) {
    QPoint newPos;

    // get new position
    newPos = e->globalPos();
    QPoint delta = (newPos - m_oldMousePos)/m_zoom;
    m_selRect.moveCenter(m_oldCenter-delta);
    m_selRect.update();

    grabFrame();
  }
}

void KMagZoomView::keyPressEvent(QKeyEvent *e)
{
  int offset = 16;
  if (e->state() & QKeyEvent::ShiftButton)
    offset = 1;

  if (e->key() == QKeyEvent::Key_Control)
    m_ctrlKeyPressed = true;
  else if (e->key() == QKeyEvent::Key_Shift)
    m_shiftKeyPressed = true;    
  else if (e->key() == QKeyEvent::Key_Left)
  {
    if (e->state() & QKeyEvent::ControlButton)
    {
      if (offset >= m_selRect.width())
        m_selRect.setWidth (1);
      else
        m_selRect.setWidth (m_selRect.width()-offset);
  }
    else if (contentsX() > 0)
    {
      offset = (int)(offset*m_zoom);
      if (contentsX() > offset)
        setContentsPos (contentsX()-offset, contentsY());
      else
        setContentsPos (0, contentsY());
    }
    else if (m_followMouse == false)
    {
      if (offset > m_selRect.x())
        m_selRect.setX (0);
      else
        m_selRect.moveBy (-offset,0);
    }
    m_selRect.update();
  }
  else if (e->key() == QKeyEvent::Key_Right)
  {
    if (e->state() & QKeyEvent::ControlButton)
      m_selRect.setRight (m_selRect.right()+offset);
    else if (contentsX() < contentsWidth()-visibleWidth())
    {
      offset = (int)(offset*m_zoom);
      if (contentsX()+offset < contentsWidth()-visibleWidth())
        setContentsPos (contentsX()+offset, contentsY());
      else
        setContentsPos (contentsWidth()-visibleWidth(), contentsY());
    }
    else if (m_followMouse == false)
      m_selRect.moveBy (offset,0);

    m_selRect.update();
  }
  else if (e->key() == QKeyEvent::Key_Up)
  {
    if (e->state() & QKeyEvent::ControlButton)
    {
      if (offset >= m_selRect.height())
        m_selRect.setHeight (1);
      else
        m_selRect.setHeight (m_selRect.height()-offset);
    }
    else if (contentsY() > 0)
    {
      offset = (int)(offset*m_zoom);
      if (contentsY() > offset)
        setContentsPos (contentsX(), contentsY()-offset);
      else
        setContentsPos (contentsX(), 0);
    }
    else if (m_followMouse == false)
    {
      if (offset > m_selRect.y())
        m_selRect.setY (0);
      else
        m_selRect.moveBy (0, -offset);
    }
    m_selRect.update();
  }
  else if (e->key() == QKeyEvent::Key_Down)
  {
    if (e->state() & QKeyEvent::ControlButton)
      m_selRect.setBottom (m_selRect.bottom()+offset);
    else if (contentsY() < contentsHeight()-visibleHeight())
    {
      offset = (int)(offset*m_zoom);
      if (contentsY()+offset < contentsHeight()-visibleHeight())
        setContentsPos (contentsX(), contentsY()+offset);
      else
        setContentsPos (contentsX(), contentsHeight()-visibleHeight());
    }
    else if (m_followMouse == false)
      m_selRect.moveBy (0, offset);
    m_selRect.update();
  }
  else
    e->ignore();
}

void KMagZoomView::keyReleaseEvent(QKeyEvent *e)
{
  if (e->key() == QKeyEvent::Key_Control)
    m_ctrlKeyPressed = false;
  else if (e->key() == QKeyEvent::Key_Shift)
    m_shiftKeyPressed = false;
  else
    e->ignore();
}

void KMagZoomView::contextMenuEvent (QContextMenuEvent *e)
{
 emit contextMenu(e->globalPos());
 e->accept();
}

void KMagZoomView::focusOutEvent(QFocusEvent *e)
{
  if(e->lostFocus() == TRUE) {
    m_ctrlKeyPressed = false;
    m_shiftKeyPressed = false;
  }
}

// SLOTS

/**
 * This will fit the zoom view to the view window, thus using the maximum
 * possible space in the window.
 */
void KMagZoomView::fitToWindow()
{
  QPoint currCenter = m_selRect.center();
  QRect newRect = m_invertedMatrix.mapRect (QRect(0, 0, visibleWidth(), visibleHeight()));
  m_selRect.setSize (newRect.size());
  m_selRect.moveCenter(currCenter);
  m_selRect.update();

  viewport()->repaint(false);
}

void KMagZoomView::setFitToWindow(bool fit)
{
  m_fitToWindow = fit;
  if (fit)
    fitToWindow();
}


/**
 * Grabs frame from X
 */
void KMagZoomView::grabFrame()
{
  // check refresh status
  if (!m_refreshSwitch)
     return;

  // check if follow-mouse is enabled
  if(m_followMouse && (m_mouseMode != ResizeSelection)) {
    // in this case grab w.r.t the current mouse position
     QPoint newCenter;

    // set new center to be the current mouse position
    m_selRect.moveCenter(QCursor::pos());
    m_selRect.update();
  }

  //QRect r = pixmapRect();

  // define a normalized rectangle
  QRect selRect = m_selRect.normalize();

  // grab screenshot from the screen and put it in the pixmap
  m_grabbedPixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), selRect.x(), selRect.y(),
                                        selRect.width(), selRect.height());

  // If the KMag window itself is in the screenshot, then it need to be filled with gray to avoid recursion
  QPoint globalPos = viewport()->mapToGlobal (viewport()->rect().topLeft());
  QRegion intersection (globalPos.x(), globalPos.y(), viewport()->width(), viewport()->height(), QRegion::Rectangle);
  intersection &= QRegion (selRect, QRegion::Rectangle);

  // We don't want to overpaint other windows that happen to be on top
  obscuredRegion (intersection, topLevelWidget()->winId(), m_selRect.winId());
  intersection.translate (-selRect.x(), -selRect.y());

  QPainter painter (&m_grabbedPixmap, true);
  QMemArray<QRect> rects (intersection.rects());
  for (uint i = 0; i < rects.size(); i++)
    painter.fillRect (rects[i], QBrush (QColor (128, 128, 128)));

  // call repaint to display the newly grabbed image
  QRect newSize = m_zoomMatrix.mapRect (m_grabbedPixmap.rect());
  resizeContents (newSize.width(), newSize.height());
  viewport()->repaint(false);
}


/**
 * Updates the mouse cursor in the zoom view
 */
void KMagZoomView::updateMouseView()
{
  QPoint pos(QCursor::pos());
  if(m_selRect.left() <= pos.x() <= m_selRect.right() &&
     m_selRect.top() <= pos.y() <= m_selRect.bottom() &&
     m_refreshSwitch)
    viewport()->repaint(false);
}

/**
 * Toggles the state of refreshing.
 */
void KMagZoomView::toggleRefresh()
{
  if(m_refreshSwitch) {
    m_refreshSwitch = false;
    m_grabTimer.stop();
    m_mouseViewTimer.stop();
  } else {
    m_refreshSwitch = true;
    m_grabTimer.start(1000/m_fps);
    m_mouseViewTimer.start(40);
  }
}

/**
 * This function sets the zoom value to be used.
 */
void KMagZoomView::setZoom(float zoom)
{
  m_zoom = zoom;
  updateMatrix();
  viewport()->repaint();
}

/**
 * This function sets the rotation value to be used.
 */
void KMagZoomView::setRotation(int rotation)
{
  m_rotation = rotation;
  updateMatrix();
  viewport()->repaint();
}

/**
 * This function sets whether the magnified image is shown inverted
 */
void KMagZoomView::setInvertation(bool invert)
{
  m_invert = invert;
  viewport()->repaint();
}

/**
 * Set a new refresh rate.
 */
void KMagZoomView::setRefreshRate(float fps)
{
  if(fps < 0.1)
    return;
  m_fps = static_cast<unsigned int>(fps);  
  
  if(m_grabTimer.isActive())
    m_grabTimer.changeInterval(static_cast<int>(1000.0/m_fps));
}

void KMagZoomView::showSelRect(bool show)
{
  m_selRect.alwaysVisible(show);
  if(show) {
    m_selRect.show();
  } else if(m_mouseMode == Normal) {
    m_selRect.hide();
  }
}

/**
 * Sets the selection rectangle to the given position.
 */
void KMagZoomView::setSelRectPos(const QRect & rect)
{
  m_selRect.setRect(rect.x(), rect.y(), rect.width(), rect.height());
  m_selRect.update();
  grabFrame();
}

bool KMagZoomView::showMouse(unsigned int type)
{
  if(type > m_showMouseTypes.count()-1)
    return (false);
  else
    m_showMouse = type;

  return(true);
}

unsigned int KMagZoomView::getShowMouseType() const
{
  return (m_showMouse);
}

QStringList KMagZoomView::getShowMouseStringList() const
{
  return (m_showMouseTypes);
}


/**
 * Returns the image which is being displayed. Its again drawn by adding
 * the mouse cursor if needed.
 */
QPixmap KMagZoomView::getPixmap()
{
  // show the pixel under mouse cursor
  if(m_showMouse && !m_grabbedPixmap.isNull()) {
    // Pixmap which will have the pixmap + mouse
    QPixmap mousePixmap(m_grabbedPixmap);

    // paint the mouse cursor w/o updating to a newer position
    paintMouseCursor(&mousePixmap, calcMousePos(false));
    
    return(mousePixmap);
  } else { // no mouse cursor
     return(m_grabbedPixmap);
  }
}

/**
 * Update the magnification matrix
 */
void KMagZoomView::updateMatrix()
{
  // update selection window size if necessary
  if (m_fitToWindow)
    fitToWindow();

  // recompute the zoom matrix
  m_zoomMatrix.reset();
  m_zoomMatrix.scale(m_zoom, m_zoom);
  m_zoomMatrix.rotate(m_rotation);

  bool inverted;
  m_invertedMatrix = m_zoomMatrix.invert (&inverted);

}
