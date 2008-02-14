/***************************************************************************
                          colorsim.h  -  description
                             -------------------
    begin                : Mon Jan 21 14:54:37 CST 2008
    copyright            : (C) 2008 by Matthew Woehlke
    email                : mw_triad@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <QtGui/QImage>
#include <QtGui/QPixmap>

namespace ColorSim {
    /**
     * Recolor a pixmap according to the specified simulation mode:
     * 1 - Protanopia
     * 2 - Deuteranopia
     * 3 - Tritanopia
     * 4 - Achromatopsia
     */
    QPixmap recolor(const QPixmap &pm, int mode, qreal gamma = 1.0);
}
