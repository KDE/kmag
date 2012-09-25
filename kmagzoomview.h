/***************************************************************************
                          kmagview.h  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sf.net
    copyright            : (C) 2003-2004 by Olaf Schmidt
    email                : ojschmidt@kde.org
    copyright            : (C) 2008 by Matthew Woehlke
    email                : mw_triad@users.sourceforge.net
    copyright              (C) 2010 Sebastian Sauer
    email                  sebsauer@kdab.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KMagZoomView_h
#define KMagZoomView_h

// include files for Qt
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtCore/QTimer>
#include <QtGui/QAbstractScrollArea>
#include <QtCore/QRect>
#include <QtGui/QCursor>
#include <QtGui/QFocusEvent>
#include <QtGui/QHideEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QMouseEvent>

//class KMagSelRect;
#include "kmagselrect.h"

#include "focustrackconfig.h"
#ifdef QAccessibilityClient_FOUND
#include <qaccessibilityclient/registry.h>
#endif

/**
 * The KMagZoomView class provides the view widget for the KmagApp instance.
 *
 * @author Sarang Lakare <sarang#users.sourceforge.net>
 */
class KMagZoomView : public QAbstractScrollArea
{
    Q_OBJECT
  public:
    /// Constructor for the main view
    explicit KMagZoomView(QWidget *parent = 0, const char *name=0);

    /// Destructor for the main view
    ~KMagZoomView();

    /// Toggles the refreshing of the window
    void toggleRefresh();

    /// Returns the currently displayed zoomed view
    QImage getImage();

    /// Returns the state of the refresh switch
    bool getRefreshStatus() const { return m_refreshSwitch; }

    /// Returns the status of followMouse
    bool getFollowMouse() const { return m_followMouse; }

#ifdef QAccessibilityClient_FOUND
    /// Returns the status of followFocus
    bool getFollowFocus() const { return m_followFocus; }
#endif

    /// Get the status of "show rect. always"
    bool getShowSelRect() const { return (m_selRect.getAlwaysVisible()); }

    /// Get the coordinates of the selection rectangle
    QRect getSelRectPos() const { return static_cast<QRect>(m_selRect); }

    /// Returns the current state of show mouse
    unsigned int getShowMouseType() const;

    /// Returns the different ways of showing mouse cursor
    QStringList getShowMouseStringList() const;

    /// Returns the status of "fit to window" option
    bool getFitToWindow() const { return (m_fitToWindow); }

  public slots:

    /// Sets zoom to the given value
    void setZoom(float zoom = 0.0);

    /// Sets the rotation to the given value
    void setRotation(int rotation = 0);

    /// Sets the color mode to the given value
    void setColorMode(int mode = 0);

    /// Grabs a frame from the given portion of the display
    void grabFrame();

    /// Update the mouse cursor in the zoom view
    void updateMouseView();

    /// Set grab-window-follows-mouse mode
    void followMouse(bool follow = true);

#ifdef QAccessibilityClient_FOUND
    /// Set grab-window-follows-mouse-and-keyboard-focus mode
    void followBoth(bool follow = true);
    
    /// Set grab-window-follows-keyboard-focus mode
    void followFocus(bool follow = true);
#endif

    /// Shows/Hides the selection marker
    void showSelRect(bool show=true);

    /// Set the position of the selection region to the given pos
    void setSelRectPos(const QRect & rect);

    /// Set the refresh rate in fps (frames per second)
    void setRefreshRate(float fps);

    /// Shows/Hides mouse cursor in the zoomed view
    bool showMouse(unsigned int type);

    /// Set the status of "fit to window" option
    void setFitToWindow (bool fit=true);

    /// Fits the zoom view to the zoom view window
    void fitToWindow();

#ifdef QAccessibilityClient_FOUND
  private slots:
    /// Called from a dbus service when followFocus is true
    void focusChanged(const QAccessibleClient::AccessibleObject &object);
#endif
  protected:
    /// Called when the widget is hidden
    void hideEvent( QHideEvent * e);

    /// Called when the widget is shown
    void showEvent( QShowEvent * e);

    /// Called when the widget has been resized
    void resizeEvent(QResizeEvent *e);

    /// Called when the widget is to be repainted
    void paintEvent(QPaintEvent *e);

    /// This function calculates the mouse position relative to the image
    QPoint calcMousePos(bool updateMousePos=true);

    /// This function draws the mouse cursor
    void paintMouseCursor(QPaintDevice *dev, const QPoint & mousePos);

    /// Called when mouse click is detected
    void mousePressEvent (QMouseEvent *e);

    /// Called when mouse is moved
    void mouseMoveEvent(QMouseEvent *e);

    /// Mouse button release event handler
    void mouseReleaseEvent(QMouseEvent *e);

    /// Mouse button release event handler
    void keyPressEvent(QKeyEvent *e);

    /// Mouse button release event handler
    void keyReleaseEvent(QKeyEvent *e);

    /// Mouse button release event handler
    void focusOutEvent(QFocusEvent *e);

    /// Returns the rectangle where the pixmap will be drawn
    QRect pixmapRect();

    /// Q3ScrollView porting helpers, maybe inline them
    int contentsX() const;
    int contentsY() const;
    int contentsWidth() const;
    int contentsHeight() const;
    int visibleWidth() const;
    int visibleHeight() const;
    void setContentsPos(int x, int y);

    /// Setup transformation matrix for zooming, rotating, and mirroring
    void setupMatrix();

  private:

#ifdef QAccessibilityClient_FOUND
    /// Global Accessibility Registry
    QAccessibleClient::Registry m_registry;
#endif

    /// Stores the pixmap which is recolored from the grabbed one
    QPixmap m_coloredPixmap;

    /// The selected rectangle which is to be grabbed
    KMagSelRect m_selRect;

    /// Grabs a window when the timer goes off
    QTimer m_grabTimer;

    /// Updates the mouse view
    QTimer m_mouseViewTimer;

    /// Zoom matrix
    QMatrix m_zoomMatrix;

    /// Saves the mouse position when a button is clicked and b4 the cursor is moved to new position
    QPoint m_oldMousePos;

    /// Saves the center of the grab window
    QPoint m_oldCenter;
    
#ifdef QAccessibilityClient_FOUND
    /// Saves the keyboard focus position
    QPoint m_oldFocus;
#endif

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
    QPoint m_latestCursorPos;

    /// Various ways of showing mouse cursor
    QStringList m_showMouseTypes;

    // configuration options:

    /// To follow mouse motion or not when no key is pressed
    bool m_followMouse;

    /// To follow keyboard focus or not
    bool m_followFocus;
    bool m_followBoth;

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

    /// Stores color simulation mode to apply
    int m_colormode;

    /// Fit the zoom view to the zoom window
    bool m_fitToWindow;
};

#endif // KMagZoomView_h
