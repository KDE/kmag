/***************************************************************************
                          colorsim.cpp  -  description
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

/***************************************************************************

Citations:

[1] H. Brettel, F. Viénot and J. D. Mollon (1997)
      "Computerized simulation of color appearance for dichromats."
      J. Opt. Soc. Am. A 14, 2647-2655.
[2] F. Viénot, H. Brettel and J. D. Mollon (1999)
      "Digital video colourmaps for checking the legibility of displays by
        dichromats."
      Color Research and Application 24, 243-252.

 ***************************************************************************/

// application specific includes
#include "colorsim.h"

// include files for Qt
#include <QtGui/QColor>

#include <math.h>

typedef qreal matrix[3][3];

#define SIMPLE_ALGORITHM

#ifndef SIMPLE_ALGORITHM
typedef qreal vector[3];

struct fcoef {
  vector k[2];
//   vector e;
  matrix s[2];
};
#endif

class xyza {
  private:
    qreal x, y, z, a;

  public:
    xyza() {}
    xyza(const QColor &c);
    QRgb rgba() const;
    xyza gamma(qreal q) const;
    xyza operator*(const matrix m) const;
#ifndef SIMPLE_ALGORITHM
    xyza(const vector v);
    qreal operator*(const vector m) const;
    xyza flatten(fcoef c) const;
#endif
};

xyza::xyza(const QColor &c) :
  x(c.redF()), y(c.greenF()), z(c.blueF()), a(c.alphaF())
{
}

inline qreal clamp(qreal n)
{
  return qMin(qreal(1.0), qMax(qreal(0.0), n));
}

QRgb xyza::rgba() const
{
  return QColor::fromRgbF(clamp(x), clamp(y), clamp(z), a).rgba();
}

xyza xyza::operator*(const matrix m) const
{
  xyza r;
  r.x = (x * m[0][0]) + (y * m[0][1]) + (z * m[0][2]);
  r.y = (x * m[1][0]) + (y * m[1][1]) + (z * m[1][2]);
  r.z = (x * m[2][0]) + (y * m[2][1]) + (z * m[2][2]);
  r.a = a;
  return r;
}

xyza xyza::gamma(qreal q) const
{
  xyza r;
  r.x = pow(x, q);
  r.y = pow(y, q);
  r.z = pow(z, q);
  r.a = a;
  return r;
}

#if !defined(SIMPLE_ALGORITHM) || !defined(STANFORD_ALGORITHM)

/***************************************************************************

 These RGB<->LMS transformation matrices are from [1].

 ***************************************************************************/

static const matrix rgb2lms = {
  {0.1992, 0.4112, 0.0742},
  {0.0353, 0.2226, 0.0574},
  {0.0185, 0.1231, 1.3550}
};

static const matrix lms2rgb = {
  { 7.4645, -13.8882,  0.1796},
  {-1.1852,   6.8053, -0.2234},
  { 0.0058,  -0.4286,  0.7558}
};

#endif

#ifdef SIMPLE_ALGORITHM

# ifndef STANFORD_ALGORITHM // from "Computerized simulation of color appearance for dichromats"

#  ifdef CRA_ALGORITHM

/***************************************************************************

 These matrices are derived from Table II in [2], for the code based on the
 Onur/Poliang algorithm below, using the LMS transformation from [1].
 No tritanopia data were provided, so that simulation does not work correctly.

 ***************************************************************************/

static const matrix coef[3] = {
  { {0.0, 2.39238646, -0.04658523}, {0.0,        1.0, 0.0       }, {0.0, 0.0, 1.0} },
  { {1.0, 0.0,         0.0       }, {0.41799267, 0.0, 0.01947228}, {0.0, 0.0, 1.0} },
  { {1.0, 0.0,         0.0       }, {0.0,        1.0, 0.0       }, {0.0, 0.0, 0.0} }
};

#  else

/***************************************************************************

 These matrices are derived from the "Color Blindness Simulation" sample
 palettes from Visolve, Ryobi System Solutions, using the LMS transformations
 from [1].
 http://www.ryobi-sol.co.jp/visolve/en/simulation.html

 ***************************************************************************/

static const matrix coef[3] = {
  { {0.0, 2.60493696, -0.08742194}, {0.0,        1.0, 0.0       }, {0.0,          0.0,        1.0} },
  { {1.0, 0.0,         0.0       }, {0.38395980, 0.0, 0.03370622}, {0.0,          0.0,        1.0} },
  { {1.0, 0.0,         0.0       }, {0.0,        1.0, 0.0       }, {-3.11932916, 12.18076308, 0.0} }
};

#  endif

# else // from the "Analysis of Color Blindness" project

/***************************************************************************

 This code is based on the matrices from [2], as presented by Onur and Poliang.
 The tritanopia simulation uses different representative wavelengths (yellow
 and blue) than those reccommended by [1] and found in most other simulations
 (red and cyan).
 http://www.stanford.edu/~ofidaner/psych221_proj/colorblindness_project.htm

 ***************************************************************************/

static const matrix coef[3] = {
  { { 0.0, 2.02344, -2.52581}, {0.0,      1.0, 0.0    }, { 0.0,      0.0,      1.0} },
  { { 1.0, 0.0,      0.0    }, {0.494207, 0.0, 1.24827}, { 0.0,      0.0,      1.0} },
  { { 1.0, 0.0,      0.0    }, {0.0,      1.0, 0.0    }, {-0.395913, 0.801109, 0.0} }
};

static const matrix rgb2lms = {
  {17.8824,   43.5161,   4.11935},
  { 3.45565,  27.1554,   3.86714},
  { 0.0299566, 0.184309, 1.46709}
};

static const matrix lms2rgb = {
  { 0.080944447905, -0.130504409160,  0.116721066440},
  {-0.010248533515,  0.054019326636, -0.113614708214},
  {-0.000365296938, -0.004121614686,  0.693511404861}
};

# endif

inline QRgb recolor(QRgb c, int mode, qreal g)
{
  if (mode > 0 && mode < 4) {
    xyza n = QColor(c);
    if (g != 1.0) {
      xyza r = n.gamma(g) * rgb2lms * coef[mode-1] * lms2rgb;
      return r.gamma(qreal(1.0) / g).rgba();
    }
    else {
      xyza r = n * rgb2lms * coef[mode-1] * lms2rgb;
      return r.rgba();
    }
  }
  else {
    return qRgb(qGray(c), qGray(c), qGray(c));
  }
}

#else // from "Computerized simulation of color appearance for dichromats"

/***************************************************************************

 This code is based on [1]. The RGB<->LMS transformation matrices are declared
 above.

 ***************************************************************************/

static const fcoef coef[3] = {
  {
    { {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0} }, // k
//     { }, // e
    // a { }
    { // s
      { {0.0, 2.39238646, -0.04658523}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0} },
      { {0.0, 0.37421464, -0.02034378}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0} }
    }
  },
  {
    { {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0} }, // k
//     { }, // e
    // a { }
    { // s
      { {1.0, 0.0, 0.0}, {0.41799267, 0.0, 0.01947228}, {0.0, 0.0, 1.0} },
      { {1.0, 0.0, 0.0}, {0.41799267, 0.0, 0.01947228}, {0.0, 0.0, 1.0} }
    }
  },
  {
    { {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0} }, // k
//     { }, // e
    // a { }
    { // s
      { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0} },
      { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0} }
    }
  }
  /* The 's' matrices are calculated thusly:
  u = E.y*A.z - E.z*A.y;
  v = E.z*A.x - E.x*A.z;
  w = E.x*A.y - E.y*A.x;
  r.x = (mode != 1) ? x : (-v/u) * y + (-w/u) * z;
  r.y = (mode != 2) ? y : (-u/v) * x + (-w/v) * z;
  r.z = (mode != 3) ? z : (-u/w) * x + (-v/w) * y;
  */
};

xyza::xyza(const vector v) :
    x(v[0]), y(v[1]), z(v[2]), a(0.0)
{
}

qreal xyza::operator*(const vector v) const
{
  return (x * v[0]) + (y * v[1]) + (z * v[2]);
}

xyza xyza::flatten(fcoef c) const
{
//   xyza e(c.e);
//   qreal u = (*this * c.k[0]) / (*this * c.k[1]);
//   qreal v = (e * c.k[0]) / (e * c.k[1]);
//   int i = (u < v ? 0 : 1);
  int i = 0;
  return *this * c.s[i];
}

inline QRgb recolor(QRgb c, int mode, qreal g)
{
  if (mode > 0 && mode < 4) {
    xyza n = QColor(c);
    xyza r = n.gamma(g) * rgb2lms;
    r = r.flatten(coef[mode-1]) * lms2rgb;
    return r.gamma(qreal(1.0) / g).rgba();
  }
  else {
    const int g = qGray(c);
    return qRgb(g,g,g);
  }
}

#endif

QImage ColorSim::recolor(const QImage &pm, int mode, qreal gamma)
{
  // get raw data in a format we can manipulate
  QImage i = pm;
  if (i.format() != QImage::Format_RGB32 && i.format() != QImage::Format_ARGB32)
    i = i.convertToFormat(QImage::Format_ARGB32);

  int n = i.width() * i.height();
  QRgb *d = (QRgb*)i.bits();
  for (int k = 0; k < n; ++k)
    d[k] = ::recolor(d[k], mode, gamma);

  return i;
}
// kate: indent-width 2;
