/***************************************************************************
                          kmagview.h  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sf.net
    copyright            : (C) 2003-2005 by Olaf Schmidt
    email                : ojschmidt@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License        *
 *                                                                         *
 ***************************************************************************/


#ifndef KMagZoomView_h_
#define KMagZoomView_h_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for TQt
#include <tqwidget.h>
#include <tqpainter.h>
#include <tqpixmap.h>
#include <tqtimer.h>
#include <tqscrollview.h>
#include <tqstringlist.h>
#include <tqrect.h>
#include <tqcursor.h>

//class KMagSelRect;
#include "kmagselrect.h"

/**
 * The KMagZoomView class provides the view widget for the KmagApp instance.  
 *   
 * @author Olaf Schmikt <ojschmidt@kde.org>
 * @author Sarang Lakare <sarang#users.sourceforge.net>
 */
class KMagZoomView : public TQScrollView
{
  Q_OBJECT
  TQ_OBJECT
  public:
    /// Constructor for the main view
    KMagZoomView(TQWidget *parent = 0, const char *name=0);

    /// Destructor for the main view
    ~KMagZoomView();

    /// Toggles the refreshing of the window
    void toggleRefresh();

    /// Returns the currently displayed zoomed view
    TQPixmap getPixmap();

    /// Returns the state of the refresh switch
    bool getRefreshtqStatus() const { return m_refreshSwitch; };

    /// Returns teh status of followMouse
    bool getFollowMouse() const { return m_followMouse; };

    /// Get the status of "show rect. always"
    bool getShowSelRect() const { return (m_selRect.getAlwaysVisible()); };

    /// Get the coordinates of the selection rectangle
    TQRect getSelRectPos() const { return (TQRect&)(m_selRect); };

    /// Returns the current state of show mouse
    unsigned int getShowMouseType() const;

    /// Returns the different ways of showing mouse cursor
    TQStringList getShowMouseStringList() const;

    /// Returns the status of "fit to window" option
    bool getFitToWindow() const { return (m_fitToWindow); };

  public slots:

    /// Sets zoom to the given value
    void setZoom(float zoom = 0.0);

    /// Sets the rotation to the given value
    void setRotation(int rotation = 0);

    /// Sets whether the magnified image is shown inverted
    void setInvertation(bool invert);
    
    /// Grabs a frame from the given portion of the display
    void grabFrame();

    /// Update the mouse cursor in the zoom view
    void updateMouseView();

    /// Set grab-window-follows-mouse mode
    void followMouse(bool follow = true);

    /// Shows/Hides the selection marker
    void showSelRect(bool show=true);

    /// Set the position of the selection region to the given pos
    void setSelRectPos(const TQRect & rect);

    /// Set the refresh rate in fps (frames per second)
    void setRefreshRate(float fps);

    /// Shows/Hides mouse cursor in the zoomed view
    bool showMouse(unsigned int type);

    /// Set the status of "fit to window" option
    void setFitToWindow (bool fit=true);

    /// Fits the zoom view to the zoom view window
    void fitToWindow();

  signals:
    void contextMenu(TQPoint pos);

  protected:
    /// Called when the widget is hidden
    void hideEvent( TQHideEvent * e);

    /// Called when the widget is shown
    void showEvent( TQShowEvent * e);

    /// Called when the widget has been resized
    void resizeEvent(TQResizeEvent *e);
    
    /// Called when the widget is to be repainted
    void drawContents ( TQPainter * p, int clipx, int clipy, int clipw, int cliph );

    /// This function calculates the mouse position relative to the image
    TQPoint calcMousePos(bool updateMousePos=true);

    /// This function draws the mouse cursor
    void paintMouseCursor(TQPaintDevice *dev, TQPoint mousePos);

    /// Called when mouse click is detected
    void mousePressEvent (TQMouseEvent *e);

    /// Called when mouse is moved
    void mouseMoveEvent(TQMouseEvent *e);

    /// Mouse button release event handler
    void mouseReleaseEvent(TQMouseEvent *e);

    /// Mouse button release event handler
    void keyPressEvent(TQKeyEvent *e);

    /// Mouse button release event handler
    void keyReleaseEvent(TQKeyEvent *e);

    /// Catch context menu events
    void contextMenuEvent (TQContextMenuEvent *e);

    /// Mouse button release event handler
    void focusOutEvent(TQFocusEvent *e);

    /// Returns the rectangle where the pixmap will be drawn
    TQRect pixmapRect();

  private:
    /// Stores the pixmap grabbed from the screen - to be zoomed
    TQPixmap m_grabbedPixmap;

    /// The selected rectangle which is to be grabbed
    KMagSelRect m_selRect;

    /// Grabs a window when the timer goes off
    TQTimer m_grabTimer;

    /// Updates the mouse view
    TQTimer m_mouseViewTimer;

    /// Zoom matrix
    TQWMatrix m_zoomMatrix;

    /// Inverted zoom matrix
    TQWMatrix m_invertedMatrix;

    /// Saves the mouse position when a button is clicked and b4 the cursor is moved to new position
    TQPoint m_oldMousePos;

    /// Saves the center of the grab window
    TQPoint m_oldCenter;

    /// Possible modes for the mouse to be in
    enum KMagMouseMode {
      Normal,
      StartSelect,
      ResizeSelection,
      MoveSelection,
      GrabSelection
    };

    /// The current mode which the mouse is
    KMagMouseMode m_mouseMode;

    /// stores the state of the Ctrl key
    bool m_ctrlKeyPressed;    

    /// stores the state of the Shift key
    bool m_shiftKeyPressed;    

    /// Store the more recent updated cursor position
    TQPoint m_latestCursorPos;

    /// Various ways of showing mouse cursor
    TQStringList m_showMouseTypes;

    // configuration options:

    /// To follow mouse motion or not when no key is pressed
    bool m_followMouse;

    /// State of refreshing - on or off
    bool m_refreshSwitch;

    /// Stores the state of the refresh switch on hide event
    bool m_refreshSwitchStateOnHide;

    /// Show mouse cursor type - 0 : do not show, non zero: show
    unsigned int m_showMouse;

    /// Frames per second for refresh
    unsigned int m_fps;

    /// Stores the amount to zoom the pixmap
    float m_zoom;

    /// Stores the degrees to rotate the pixmap
    int m_rotation;

    /// Whether the magnified image is to be shown inverted
    int m_invert;

    /// Fit the zoom view to the zoom window
    bool m_fitToWindow;

    /// Update the magnification matrix
    void updateMatrix();
};

#endif // KMagZoomView_h_
