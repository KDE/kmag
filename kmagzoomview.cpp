//$Id$
/***************************************************************************
                          kmagview.cpp  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001 by Sarang Lakare
    email                : sarang@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwhatsthis.h>
#include <klocale.h>
#include <qwidget.h>

// application specific includes
//#include "kmagdoc.h"
#include "kmag.h"
#include "kmagselrect.moc"
#include "kmagzoomview.h"

// include files for Qt
//#include <qprinter.h>
#include <qpainter.h>

#include <iostream>

KMagZoomView::KMagZoomView(QWidget *parent, const char *name)
	: QFrame(parent, name),
    m_selRect(0, 0, 128, 128),
		m_grabTimer(parent),
		m_zoom(1.0),
		m_followMouse(false)
{
  setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  setLineWidth(0);
  setBackgroundColor(QColor("white"));
	setFocusPolicy(QWidget::StrongFocus);
	
	// init the zoom matrix
  m_zoomMatrix.reset();	
  m_zoomMatrix.scale(m_zoom, m_zoom);	

	// update freq
	m_fps = 10;
	m_ctrlKeyPressed = false;
	m_shiftKeyPressed = false;
	m_refreshSwitch = true;
	
	// start the grabTimer and connect it to grabFrame()
  m_grabTimer.start(1000/m_fps);
  connect(&m_grabTimer, SIGNAL(timeout()), SLOT(grabFrame()));

  QWhatsThis::add(this, i18n("This is the main window which shows the contents of the\
 selected region. The contents will be magnified if zoom level is set."));
}

KMagZoomView::~KMagZoomView()
{
}

/**
 * This function will set/reset mouse following of grab window.
 */
void KMagZoomView::followMouse(bool follow)
{
	if(follow) {
  	m_followMouse = true;
		setMouseTracking(true);
		m_mouseMode = Normal;
	} else {
  	m_followMouse = false;
		setMouseTracking(false);
		m_mouseMode = Normal;
	}
}

/**
 * Called when the widget is to be repainted
 *
 * @param p
 */
void KMagZoomView::paintEvent(QPaintEvent *e)
{
	// get a rectangle centered inside the frame
  QRect pRect(pixmapRect());

  QRect uRect(e->rect());

	// get the rectangle in the frame
  QRect wRect(contentsRect());

  QPainter p(this);
  p.setClipRect(wRect);

  QRect r;

  // top

  r = wRect;
  r.setBottom(pRect.top()-1);
  r = r.intersect(uRect);
  if ( !r.isEmpty() )
    p.eraseRect(r);

  // bottom

  r = wRect;
  r.setTop(pRect.bottom()+1);
  r = r.intersect(uRect);
  if ( !r.isEmpty() )
    p.eraseRect(r);

  // left

  r = wRect;
  r.setTop(pRect.top());
  r.setBottom(pRect.bottom());
  r.setRight(pRect.left()-1);
  r = r.intersect(uRect);
  if ( !r.isEmpty() )
    p.eraseRect(r);

  // right

  r = wRect;
  r.setTop(pRect.top());
  r.setBottom(pRect.bottom());
  r.setLeft(pRect.right()+1);
  r = r.intersect(uRect);
  if ( !r.isEmpty() )
    p.eraseRect(r);

	// draw the pixmap
  p.drawPixmap(pRect.x(), pRect.y(), m_grabbedZoomedPixmap);
	
//	bitBlt(this, 0, 0, &m_grabbedZoomedPixmap);

}

QRect KMagZoomView::pixmapRect()
{
  int free_x = width()  - m_grabbedZoomedPixmap.width();
  int free_y = height()  - m_grabbedZoomedPixmap.height();

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
	// don't do anything if follow mouse is enabled
	if(m_followMouse)
		return;

  switch(e->button()) {
  case QMouseEvent::LeftButton :
		if(m_ctrlKeyPressed) {
			// check if currently in resize mode
      if(m_mouseMode != ResizeSelection) {
        // set the mode to ResizeSelection
        m_mouseMode = ResizeSelection;

        // set mouse cursor to "resize all direction"
        setCursor(sizeAllCursor);

        // backup the old position
        m_oldMousePos.setX(e->globalX());
        m_oldMousePos.setY(e->globalY());

        // set the cursor position to the center of the selected region
        QCursor::setPos(m_selRect.bottomRight());

        // show the selection rectangle
        m_selRect.show();
      }
		} else if(m_shiftKeyPressed) {
      // check if currently in move mode
      if(m_mouseMode != MoveSelection) {
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
		} else {
      // check if currently in move mode
      if(m_mouseMode != GrabSelection) {
        m_mouseMode = GrabSelection;

        // set mouse cursor to hand
        setCursor(pointingHandCursor);

        // store the old position
        m_oldMousePos.setX(e->globalX());
        m_oldMousePos.setY(e->globalY());

				m_oldCenter = m_selRect.center();

        // show the selected rectangle
        m_selRect.show();
      }
		}
    break;

  case QMouseEvent::MidButton :
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
 * Called when a mouse button is released
 *
 * @param e
 */
void KMagZoomView::mouseReleaseEvent(QMouseEvent *e)
{
	// don't do anything if follow mouse is enabled
	if(m_followMouse)
		return;

  switch(e->button()) {
  case QMouseEvent::LeftButton :
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

  case QMouseEvent::MidButton :
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
	// don't do anything if follow mouse is enabled
	if(m_followMouse)
		return;

	if(m_mouseMode == ResizeSelection) {
  	// In resize selection mode
    // set the current mouse position as the bottom, right corner
    m_selRect.setRight(e->globalX());
    m_selRect.setBottom(e->globalY());
    m_selRect.update();
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
		} else if(newCenter.y() >=  QApplication::desktop()->height()-m_selRect.width()/2) {
			// set Y to the maximum possible Y
			newCenter.setY(QApplication::desktop()->height()-m_selRect.width()/2-1);
		}

								
		// move to the new center	
		m_selRect.moveCenter(newCenter);

    // update the grab rectangle display
    m_selRect.update();

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
		} else if(newCenter.y() >=  QApplication::desktop()->height()-m_selRect.width()/2) {
			// set Y to the maximum possible Y
			newCenter.setY(QApplication::desktop()->height()-m_selRect.width()/2-1);
		}
						
		// move to the new center	
		m_selRect.moveCenter(newCenter);

    // update the grab rectangle display
    m_selRect.update();
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

/** Grabs frame from X
 *
 */
void KMagZoomView::grabFrame()
{

	// check if follow-mouse is enabled
	if(m_followMouse) {
		// in this case grab w.r.t the current mouse position
		 QPoint newCenter;

		// set new center to be the current mouse position
		newCenter = m_cursor.pos();

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
		} else if(newCenter.y() >=  QApplication::desktop()->height()-m_selRect.width()/2) {
			// set Y to the maximum possible Y
			newCenter.setY(QApplication::desktop()->height()-m_selRect.width()/2-1);
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
  repaint(pixmapRect(), false);

  // update the grab rectangle display
  m_selRect.update();
}

/** Toggles the state of refreshing.
  *
  */
void KMagZoomView::toggleRefresh()
{
  if(m_refreshSwitch) {
    m_refreshSwitch = false;
    m_grabTimer.stop();
  } else {
    m_refreshSwitch = true;
    m_grabTimer.start(1000/m_fps);
  }
}

/**
 * This function sets the zoom value to be used.
 */
void KMagZoomView::setZoom(float zoom)
{
  // use this zoom
  m_zoom = zoom;

  // recompute the zoom matrix
	m_zoomMatrix.reset();
  m_zoomMatrix.scale(m_zoom, m_zoom);	
 	
  m_grabbedZoomedPixmap = m_grabbedPixmap.xForm(m_zoomMatrix);

  repaint();
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
