/***************************************************************************
                          kmagview.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sourceforge.net
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
#include "kmag.h"
#include "kmagselrect.moc"
#include "kmagzoomview.h"

// include files for Qt
#include <qbitmap.h>
#include <qcursor.h>
#include <qglobal.h>
#include <qpainter.h>
#include <qwhatsthis.h>
#include <qwidget.h>

// include files for KDE
#include <kcursor.h>
#include <kdebug.h>
#include <klocale.h>

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


KMagZoomView::KMagZoomView(QWidget *parent, const char *name)
  : QFrame(parent, name),
    m_selRect(0, 0, 128, 128),
    m_grabTimer(parent),
    m_mouseViewTimer(parent),
    m_latestCursorPos(0,0),
    m_followMouse(false),
    m_showMouse(1),
    m_zoom(1.0),
    m_fitToWindow(true)
{
  KApplication::setGlobalMouseTracking(TRUE);
  setMouseTracking(TRUE);
  setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  setLineWidth(0);
  setBackgroundColor(QColor("grey"));
  setFocusPolicy(QWidget::StrongFocus);
  
  // init the zoom matrix
  m_zoomMatrix.reset();  
  m_zoomMatrix.scale(m_zoom, m_zoom);  

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
 selected region. The contents will be magnified if zoom level is set."));

  // different ways to show the cursor.
  m_showMouseTypes << "Hidden" << "Box" << "Arrow" << "Actual";

  if(m_fitToWindow)
    fitToWindow();
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
  } else {
    m_followMouse = false;
    m_mouseMode = Normal;
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
void KMagZoomView::resizeEvent(QResizeEvent* )
{
  if(m_fitToWindow)
    fitToWindow(); 
}

/**
 * Called when the widget is to be repainted
 *
 * @param p
 */
void KMagZoomView::paintEvent(QPaintEvent* )
{
  if(m_grabbedZoomedPixmap.isNull())
    return;
  // paint on this widget
  paintViewImage(this, m_refreshSwitch);
}

/**
 * This function will actually draw on the paintdevice the
 * final output. This is separated from paintEvent for printing purposes.
 */
void KMagZoomView::paintViewImage(QPaintDevice *dev, bool updateMousePos)
{
  if(!dev)
    return;

  // get a rectangle centered inside the frame
  QRect pRect(pixmapRect());

  // get rectangular area that needs to be repainted
  //QRect uRect(e->rect());

  // get the rectangle inside the frame
  QRect wRect(contentsRect());

  // A pixmap which will be eventually displayed
  QPixmap *zoomView;

  // Get mouse position relative to the image
  QPoint mousePos = calcMousePos (updateMousePos);

  // show the pixel under mouse cursor
  if(m_showMouse) {

    // Pixmap which will have the zoomed pixmap + mouse
    zoomView = new QPixmap(m_grabbedZoomedPixmap);

    // paint the mouse cursor
    paintMouseCursor(zoomView, mousePos);
  } else { // do not show mouse
    zoomView = &m_grabbedZoomedPixmap;
  }

  if(m_grabbedZoomedPixmap.width() < wRect.width() &&
     m_grabbedZoomedPixmap.height() < wRect.height()) {
    // zoomed image is smaller than the display area
    // there is a need to clear the window first
    // a paint area that is to be painted on
    QPixmap paintArea(wRect.size());
    QPainter pa(&paintArea);

    pa.setBackgroundColor(Qt::black);
  
    // clear the paint area
    pa.eraseRect(paintArea.rect());

    // draw the pixmap on the paint area
    bitBlt(&paintArea, pRect.x(), pRect.y(), zoomView);

    // bitBlt the paintArea pixmap on the widget  
    bitBlt(dev, QPoint(0,0), &paintArea);
  } else {
    int width = m_grabbedZoomedPixmap.width();
    int height = m_grabbedZoomedPixmap.height();
    int xdiff = width - wRect.width();
    int ydiff = height - wRect.height();

    // set the (x,y) of start of the part of zoom image to display
    // and the w,h of the part to display
    if(xdiff >= 0) {
      // center the shown part of image to the mouse position
      xdiff = mousePos.x() - (int)(wRect.width()/2.0);
      // Is the mouse at the border of the part?
      if (xdiff < 0)
        xdiff = 0;
      else if (xdiff + wRect.width() > width)
        xdiff = width - wRect.width();

      width = wRect.width();
    }

    if(ydiff >= 0) {
      // center the shown part of image to the mouse position
      ydiff = mousePos.y() - (int)(wRect.height()/2.0);
      // Is the mouse at the border of the part?
      if (ydiff < 0)
        ydiff = 0;
      else if (ydiff + wRect.height() > height)
        ydiff = height - wRect.height();

      height = wRect.height();
    }

    if(xdiff < 0 || ydiff < 0) {
      // only one of the sides is larger than the display area
      if(xdiff < 0)
        xdiff = 0;
      else
        ydiff = 0;

      // there is a need to clear the window first
      // a paint area that is to be painted on
      QPixmap paintArea(wRect.size());
      QPainter pa(&paintArea);

      pa.setBackgroundColor(Qt::black);
    
      // clear the paint area
      pa.eraseRect(paintArea.rect());

      // The rect corr. to the part of zoom view whish should be shown
      QRect srcRect(xdiff, ydiff, width, height);

      // bitBlt zoom view onto the paint area
      bitBlt(&paintArea, QPoint(pRect.x(), pRect.y()), zoomView, srcRect);

      // bitBlt the paintArea pixmap on the widget  
      bitBlt(dev, QPoint(0,0), &paintArea);
    } else { // the zoom image is larger than the display    
      // The rect corr. to the part of zoom view whish should be shown
      QRect srcRect(xdiff, ydiff, width, height);

      // bitBlt this part on to the widget.
      bitBlt(dev, QPoint(0,0), zoomView, srcRect);
    } //if(xdiff < 0 || ydiff < 0)

  } //if-else(xdiff < 0 && ydiff <0)   


  if(zoomView != &m_grabbedZoomedPixmap)  
    delete zoomView;
}

/**
 * Draws the mouse cursor according to the current selection of the type of
 * mouse cursor to draw.
 */
void KMagZoomView::paintMouseCursor(QPaintDevice *dev, QPoint mousePos)
{
  if(!dev)
    return;

  // painter for the zoom view
  QPainter pz(dev);

  if(m_latestCursorPos.x() >= 0 && m_latestCursorPos.x() < m_selRect.width() &&
     m_latestCursorPos.y() >= 0 && m_latestCursorPos.y() < m_selRect.height() ) { // || updateMousePos) {
    // mouse position is indeed inside the selRect
      
    // How to show the mouse :

    switch(m_showMouse) {
    case 1:
      // 1. Square around the pixel
      pz.setPen(Qt::white);
      pz.setRasterOp(Qt::XorROP);
      pz.drawRect(mousePos.x()-1, mousePos.y()-1, (int)m_zoom+2, (int)m_zoom+2);
      break;

    case 2:
    {
      // 2. Arrow cursor
      pz.setPen(Qt::black);
      pz.setBackgroundColor(Qt::white);

      QBitmap sCursor( 16, 16, left_ptr_bits, TRUE );
      QBitmap mask( 16, 16, left_ptrmsk_bits, TRUE );
      sCursor.setMask(mask);
      sCursor = sCursor.xForm(m_zoomMatrix);
                
      // since hot spot is at 3,1
      pz.drawPixmap(mousePos.x()-(int)(3.0*m_zoom), mousePos.y()-(int)m_zoom, sCursor);
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
          sCursor = sCursor.xForm(m_zoomMatrix);
                    
          // since hot spot is at 3,1
          pz.drawPixmap(mousePos.x()-(int)(3.0*m_zoom), mousePos.y()-(int)m_zoom, sCursor);
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

  // get coordinates of the pixel w.r.t. the zoomed pixmap
  QPoint mousePos ((int)((float)m_latestCursorPos.x()*m_zoom),
                   (int)((float)m_latestCursorPos.y()*m_zoom));
  return mousePos;
}


QRect KMagZoomView::pixmapRect()
{
  int free_x = width() - m_grabbedZoomedPixmap.width();
  int free_y = height() - m_grabbedZoomedPixmap.height();

  QPoint startPoint((free_x > 0) ? (free_x / 2) : 0,
                    (free_y > 0) ? (free_y / 2) : 0);

  QRect r(m_grabbedZoomedPixmap.rect());
  r.moveTopLeft(startPoint);

  return (r);
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
    newCenter = e->globalPos();

    // make sure the mouse position is not taking the grab window outside
    // the display
    if(newCenter.x() < m_selRect.width()/2) {
      // set X to the minimum possible X
      newCenter.setX(m_selRect.width()/2);
    } else if(newCenter.x() >=  QApplication::desktop()->width()-m_selRect.width()/2) {
      // set X to the maximum possible X
      newCenter.setX(QApplication::desktop()->width()-m_selRect.width()/2-1);
    }

    if(newCenter.y() < m_selRect.height()/2) {
      // set Y to the minimum possible Y
      newCenter.setY(m_selRect.height()/2);
    } else if(newCenter.y() >=  QApplication::desktop()->height()-m_selRect.height()/2) {
      // set Y to the maximum possible Y
      newCenter.setY(QApplication::desktop()->height()-m_selRect.height()/2-1);
    }
    // move to the new center
    m_selRect.moveCenter(newCenter);
    // update the grab rectangle display
    m_selRect.update();
    grabFrame();
  } else if(m_mouseMode == GrabSelection) {
     QPoint newPos;

    // get new position
    newPos = e->globalPos();

    QPoint delta = (newPos - m_oldMousePos)/m_zoom;
    QPoint newCenter = m_oldCenter-delta;

    // make sure the mouse position is not taking the grab window outside
    // the display
    if(newCenter.x() < m_selRect.width()/2) {
      // set X to the minimum possible X
      newCenter.setX(m_selRect.width()/2);
    } else if(newCenter.x() >=  QApplication::desktop()->width()-m_selRect.width()/2) {
      // set X to the maximum possible X
      newCenter.setX(QApplication::desktop()->width()-m_selRect.width()/2-1);
    }

    if(newCenter.y() < m_selRect.height()/2) {
      // set Y to the minimum possible Y
      newCenter.setY(m_selRect.height()/2);
    } else if(newCenter.y() >=  QApplication::desktop()->height()-m_selRect.height()/2) {
      // set Y to the maximum possible Y
      newCenter.setY(QApplication::desktop()->height()-m_selRect.height()/2-1);
    }
            
    // move to the new center  
    m_selRect.moveCenter(newCenter);
    // update the grab rectangle display
    m_selRect.update();
    grabFrame();
  }
}

void KMagZoomView::keyPressEvent(QKeyEvent *e)
{
  if(e->key() == QKeyEvent::Key_Control) {
    m_ctrlKeyPressed = true;
  } else if(e->key() == QKeyEvent::Key_Shift){
    m_shiftKeyPressed = true;    
  } else {
    e->ignore();
  }
}

void KMagZoomView::keyReleaseEvent(QKeyEvent *e)
{
  if(e->key() == QKeyEvent::Key_Control) {
    m_ctrlKeyPressed = false;
  } else if(e->key() == QKeyEvent::Key_Shift){
    m_shiftKeyPressed = false;    
  } else {
    e->ignore();
  }
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
    // this is a temporary solution, cast, mabye newWidth and newHeight should be float
  unsigned int newWidth = static_cast<unsigned int>(this->width()/this->m_zoom);
  unsigned int newHeight = static_cast<unsigned int>(this->height()/this->m_zoom);

  QPoint currCenter = m_selRect.center();

  m_selRect.setWidth(newWidth);
  m_selRect.setHeight(newHeight);

   // make sure the selection window does not go outside of the display
   if(currCenter.x() < m_selRect.width()/2) {
     // set X to the minimum possible X
     currCenter.setX(m_selRect.width()/2);
   } else if(currCenter.x() >=  QApplication::desktop()->width()-m_selRect.width()/2) {
     // set X to the maximum possible X
     currCenter.setX(QApplication::desktop()->width()-m_selRect.width()/2-1);
   }

   if(currCenter.y() < m_selRect.height()/2) {
     // set Y to the minimum possible Y
     currCenter.setY(m_selRect.height()/2);
   } else if(currCenter.y() >=  QApplication::desktop()->height()-m_selRect.height()/2) {
     // set Y to the maximum possible Y
     currCenter.setY(QApplication::desktop()->height()-m_selRect.height()/2-1);
   }

  m_selRect.moveCenter(currCenter);
  // update the grab rectangle display
  m_selRect.update();
//  m_fitToWindow = true;
  repaint(false);
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
    newCenter = QCursor::pos();

    // make sure the mouse position is not taking the grab window outside
    // the display
    if(newCenter.x() < m_selRect.width()/2) {
      // set X to the minimum possible X
      newCenter.setX(m_selRect.width()/2);
    } else if(newCenter.x() >=  QApplication::desktop()->width()-m_selRect.width()/2) {
      // set X to the maximum possible X
      newCenter.setX(QApplication::desktop()->width()-m_selRect.width()/2-1);
    }

    if(newCenter.y() < m_selRect.height()/2) {
      // set Y to the minimum possible Y
      newCenter.setY(m_selRect.height()/2);
    } else if(newCenter.y() >=  QApplication::desktop()->height()-m_selRect.height()/2) {
      // set Y to the maximum possible Y
      newCenter.setY(QApplication::desktop()->height()-m_selRect.height()/2-1);
    }              
    // move to the new center
    m_selRect.moveCenter(newCenter);

  }

  //QRect r = pixmapRect();

  // define a normalized rectangle
  QRect selRect = m_selRect.normalize();

  // grab screenshot from the screen and put it in the pixmap
  m_grabbedPixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), selRect.x(), selRect.y(),
                                        selRect.width(), selRect.height());

  // zoom the image
  m_grabbedZoomedPixmap = m_grabbedPixmap.xForm(m_zoomMatrix);

  // call repaint to display the newly grabbed image
  repaint(false);

  // update the grab rectangle display
  m_selRect.update();
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
    repaint(false);
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
  // use this zoom
  m_zoom = zoom;

  // update selection window size when zooming in if necessary
  if (m_fitToWindow)
    fitToWindow();

  // recompute the zoom matrix
  m_zoomMatrix.reset();
  m_zoomMatrix.scale(m_zoom, m_zoom);

  m_grabbedZoomedPixmap = m_grabbedPixmap.xForm(m_zoomMatrix);

  repaint();
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
  if(m_showMouse && !m_grabbedZoomedPixmap.isNull()) {
    // Pixmap which will have the zoomed pixmap + mouse
    QPixmap zoomView(m_grabbedZoomedPixmap);

    // paint the mouse cursor w/o updating to a newer position
    paintMouseCursor(&zoomView, calcMousePos(false));
    
    return(zoomView);
  } else { // no mouse cursor
     return(m_grabbedZoomedPixmap);
  }
}

#include "kmagzoomview.moc"
