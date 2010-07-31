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
#include <tqbitmap.h>
#include <tqpixmap.h>
#include <tqimage.h>
#include <tqcursor.h>
#include <tqglobal.h>
#include <tqpainter.h>
#include <tqwhatsthis.h>
#include <tqwidget.h>

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



static bool obscuredRegion (TQRegion &region, Window winId, Window ignoreId, Window start = 0, int level = -1) {
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
            region -= TQRegion (atts.x, atts.y, atts.width, atts.height, TQRegion::Rectangle);
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




KMagZoomView::KMagZoomView(TQWidget *parent, const char *name)
  : TQScrollView(parent, name),
    m_selRect(0, 0, 128, 128, this),
    m_grabTimer(0),
    m_mouseViewTimer(0),
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
  viewport()->setFocusPolicy(TQWidget::StrongFocus);
  
  m_ctrlKeyPressed = false;
  m_shiftKeyPressed = false;
  m_refreshSwitch = true;
  m_refreshSwitchStateOnHide = m_refreshSwitch;
  
  // set the refresh rate
  setRefreshRate(10);

  // connect it to grabFrame()
  connect(&m_grabTimer, TQT_SIGNAL(timeout()), TQT_SLOT(grabFrame()));
  // start the grabTimer
  m_grabTimer.start(static_cast<int>(1000.0/m_fps));

  // connect it to updateMouseView()
  connect(&m_mouseViewTimer, TQT_SIGNAL(timeout()), TQT_SLOT(updateMouseView()));
  // start the grabTimer @ 25 frames per second!
  m_mouseViewTimer.start(25);

  TQWhatsThis::add(this, i18n("This is the main window which shows the contents of the\
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
    setVScrollBarMode (TQScrollView::AlwaysOff);
    setHScrollBarMode (TQScrollView::AlwaysOff);
  } else {
    m_followMouse = false;
    m_mouseMode = Normal;
    setVScrollBarMode (TQScrollView::AlwaysOn);
    setHScrollBarMode (TQScrollView::AlwaysOn);
  }
}

/**
 * Called when the widget is hidden. Stop refresh when this happens.
 */
void KMagZoomView::hideEvent( TQHideEvent* )
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
void KMagZoomView::showEvent( TQShowEvent* )
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
void KMagZoomView::resizeEvent( TQResizeEvent * e )
{
  TQScrollView::resizeEvent (e);
  if(m_fitToWindow)
    fitToWindow();
}

/**
 * Called when the widget is to be repainted
 *
 * @param p
 */
void KMagZoomView::drawContents ( TQPainter * p, int clipx, int clipy, int clipw, int cliph )
{
  if(m_grabbedPixmap.isNull())
    return;

  // A pixmap which will be eventually displayed
  TQRect areaToPaint = m_invertedMatrix.mapRect (TQRect(clipx, clipy, clipw, cliph));
  TQPixmap clippedPixmap (areaToPaint.size());
  clippedPixmap.fill (TQColor (128, 128, 128));
  areaToPaint &= TQRect (TQPoint (0,0), m_selRect.size());
  bitBlt(&clippedPixmap, TQPoint (0,0), &m_grabbedPixmap, areaToPaint);

  // show the pixel under mouse cursor
  if(m_showMouse) {
    // paint the mouse cursor
    paintMouseCursor(&clippedPixmap, calcMousePos (m_refreshSwitch)-TQPoint (areaToPaint.x(), areaToPaint.y()));
  }

  TQPixmap zoomedPixmap;
  zoomedPixmap = clippedPixmap.xForm (m_zoomMatrix);

  if (m_invert) {
    TQImage zoomedImage;
    zoomedImage = zoomedPixmap.convertToImage();
    zoomedImage.invertPixels (false);
    p->drawImage (TQPoint (clipx-contentsX(), clipy-contentsY()), zoomedImage, zoomedImage.rect(),
                  Qt::ThresholdDither | Qt::ThresholdAlphaDither | Qt::AvoidDither);
  } else {
    p->drawPixmap (TQPoint (clipx, clipy), zoomedPixmap, zoomedPixmap.rect());
  }
}

/**
 * Draws the mouse cursor according to the current selection of the type of
 * mouse cursor to draw.
 */
void KMagZoomView::paintMouseCursor(TQPaintDevice *dev, TQPoint mousePos)
{
  if(!dev)
    return;

  TQPainter pz(dev);

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

      TQBitmap sCursor( 16, 16, left_ptr_bits, TRUE );
      TQBitmap mask( 16, 16, left_ptrmsk_bits, TRUE );
      sCursor.setMask(mask);

      // since hot spot is at 3,1
      pz.drawPixmap(mousePos.x()-3, mousePos.y()-1, sCursor);
    }
    break;

    case 3:
    {    
      // 3. Actual cursor
      // Get the current cursor type
      TQWidget *dummy  = KApplication::widgetAt(TQCursor::pos(), FALSE);
      if(!dummy)
        break;
      kdDebug() << ">" << dummy->name() << ":" << dummy->cursor().shape() << "-" << endl;
      switch(this->cursor().shape())  {
        case ArrowCursor :
         {
          // 2. Arrow cursor
          pz.setPen(Qt::black);
          pz.setBackgroundColor(Qt::white);

          TQBitmap sCursor( 16, 16, left_ptr_bits, TRUE );
          TQBitmap mask( 16, 16, left_ptrmsk_bits, TRUE );
          sCursor.setMask(mask);

          // since hot spot is at 3,1
          pz.drawPixmap(mousePos.x()-3, mousePos.y()-1, sCursor);
        }
        break;
        default:
          TQBitmap sCursor( 32, 32, phand_bits, TRUE );
          TQBitmap mask( 32, 32, phandm_bits, TRUE );
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


TQPoint KMagZoomView::calcMousePos(bool updateMousePos)
{
  // get position of mouse wrt selRect
  if(updateMousePos) { // get a new position only if asked
    m_latestCursorPos = TQCursor::pos();
    m_latestCursorPos -= TQPoint(m_selRect.x(), m_selRect.y());
  }

  // get coordinates of the pixel w.r.t. the pixmap
  return TQPoint (m_latestCursorPos.x(), m_latestCursorPos.y());
}


// MOUSE ACTIONS

/**
 * Called when mouse is clicked inside the window
 *
 * @param e
 */
void KMagZoomView::mousePressEvent(TQMouseEvent *e)
{
  switch(e->button()) {
  case TQMouseEvent::LeftButton :
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
        TQCursor::setPos(m_selRect.bottomRight());

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
        TQCursor::setPos(m_selRect.center());

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

  case TQMouseEvent::MidButton :
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
      TQCursor::setPos(m_selRect.center());

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
void KMagZoomView::mouseReleaseEvent(TQMouseEvent *e)
{
  switch(e->button()) {
  case TQMouseEvent::LeftButton :
  case TQMouseEvent::MidButton :
    // check if currently in move mode
    if(m_mouseMode == MoveSelection) {
      // hide the selection window
      m_selRect.hide();
      // set the mouse mode to normal
      m_mouseMode = Normal;

      // restore the cursor shape
      setCursor(arrowCursor);

      // restore the cursor position
      TQCursor::setPos(m_oldMousePos);
    } else if(m_mouseMode == ResizeSelection) {
      // hide the selection window
      m_selRect.hide();
      // set the mouse mode to normal
      m_mouseMode = Normal;

      // restore the cursor shape
      setCursor(arrowCursor);

      // restore the cursor position
      TQCursor::setPos(m_oldMousePos);
    } else if(m_mouseMode == GrabSelection) {
      // hide the selection window
      m_selRect.hide();

      // set the mouse mode to normal
      m_mouseMode = Normal;

      // restore the cursor shape
      setCursor(arrowCursor);
    }    
    break;

  case TQMouseEvent::RightButton :
    break;
  case TQMouseEvent::NoButton :
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
void KMagZoomView::mouseMoveEvent(TQMouseEvent *e)
{
  if(m_mouseMode == ResizeSelection) {
    // In resize selection mode
    // set the current mouse position as the bottom, right corner
    m_selRect.setRight(e->globalX());
    m_selRect.setBottom(e->globalY());
    m_selRect.update();

    grabFrame();
  } else if(m_mouseMode == MoveSelection) {
    TQPoint newCenter;

    // set new center to be the current mouse position
    m_selRect.moveCenter(e->globalPos());
    m_selRect.update();

    grabFrame();
  } else if(m_mouseMode == GrabSelection) {
    TQPoint newPos;

    // get new position
    newPos = e->globalPos();
    TQPoint delta = (newPos - m_oldMousePos)/m_zoom;
    m_selRect.moveCenter(m_oldCenter-delta);
    m_selRect.update();

    grabFrame();
  }
}

void KMagZoomView::keyPressEvent(TQKeyEvent *e)
{
  int offset = 16;
  if (e->state() & TQKeyEvent::ShiftButton)
    offset = 1;

  if (e->key() == TQKeyEvent::Key_Control)
    m_ctrlKeyPressed = true;
  else if (e->key() == TQKeyEvent::Key_Shift)
    m_shiftKeyPressed = true;    
  else if (e->key() == TQKeyEvent::Key_Left)
  {
    if (e->state() & TQKeyEvent::ControlButton)
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
  else if (e->key() == TQKeyEvent::Key_Right)
  {
    if (e->state() & TQKeyEvent::ControlButton)
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
  else if (e->key() == TQKeyEvent::Key_Up)
  {
    if (e->state() & TQKeyEvent::ControlButton)
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
  else if (e->key() == TQKeyEvent::Key_Down)
  {
    if (e->state() & TQKeyEvent::ControlButton)
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

void KMagZoomView::keyReleaseEvent(TQKeyEvent *e)
{
  if (e->key() == TQKeyEvent::Key_Control)
    m_ctrlKeyPressed = false;
  else if (e->key() == TQKeyEvent::Key_Shift)
    m_shiftKeyPressed = false;
  else
    e->ignore();
}

void KMagZoomView::contextMenuEvent (TQContextMenuEvent *e)
{
 emit contextMenu(e->globalPos());
 e->accept();
}

void KMagZoomView::focusOutEvent(TQFocusEvent *e)
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
  TQPoint currCenter = m_selRect.center();
  TQRect newRect = m_invertedMatrix.mapRect (TQRect(0, 0, visibleWidth(), visibleHeight()));
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
     TQPoint newCenter;

    // set new center to be the current mouse position
    m_selRect.moveCenter(TQCursor::pos());
    m_selRect.update();
  }

  //TQRect r = pixmapRect();

  // define a normalized rectangle
  TQRect selRect = m_selRect.normalize();

  // grab screenshot from the screen and put it in the pixmap
  m_grabbedPixmap = TQPixmap::grabWindow(TQApplication::desktop()->winId(), selRect.x(), selRect.y(),
                                        selRect.width(), selRect.height());

  // If the KMag window itself is in the screenshot, then it need to be filled with gray to avoid recursion
  TQPoint globalPos = viewport()->mapToGlobal (viewport()->rect().topLeft());
  TQRegion intersection (globalPos.x(), globalPos.y(), viewport()->width(), viewport()->height(), TQRegion::Rectangle);
  intersection &= TQRegion (selRect, TQRegion::Rectangle);

  // We don't want to overpaint other windows that happen to be on top
  obscuredRegion (intersection, topLevelWidget()->winId(), m_selRect.winId());
  intersection.translate (-selRect.x(), -selRect.y());

  TQPainter painter (&m_grabbedPixmap, true);
  TQMemArray<TQRect> rects (intersection.rects());
  for (uint i = 0; i < rects.size(); i++)
    painter.fillRect (rects[i], TQBrush (TQColor (128, 128, 128)));

  // call repaint to display the newly grabbed image
  TQRect newSize = m_zoomMatrix.mapRect (m_grabbedPixmap.rect());
  resizeContents (newSize.width(), newSize.height());
  viewport()->repaint(false);
}


/**
 * Updates the mouse cursor in the zoom view
 */
void KMagZoomView::updateMouseView()
{
  TQPoint pos(TQCursor::pos());
  if(m_selRect.left() <= pos.x() && pos.x() <= m_selRect.right() &&
     m_selRect.top() <= pos.y() && pos.y() <= m_selRect.bottom() &&
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
void KMagZoomView::setSelRectPos(const TQRect & rect)
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

TQStringList KMagZoomView::getShowMouseStringList() const
{
  return (m_showMouseTypes);
}


/**
 * Returns the image which is being displayed. Its again drawn by adding
 * the mouse cursor if needed.
 */
TQPixmap KMagZoomView::getPixmap()
{
  // show the pixel under mouse cursor
  if(m_showMouse && !m_grabbedPixmap.isNull()) {
    // Pixmap which will have the pixmap + mouse
    TQPixmap mousePixmap(m_grabbedPixmap);

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
