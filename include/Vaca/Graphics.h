// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Vaca nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VACA_GRAPHICS_H
#define VACA_GRAPHICS_H

#include <list>
#include <vector>
#include <boost/noncopyable.hpp>

#include "Vaca/base.h"
#include "Vaca/Rect.h"
#include "Vaca/Color.h"
#include "Vaca/String.h"

namespace Vaca {

class Application;
class Brush;
class Font;
class Image;
class ImageList;
class Pen;
class Point;
class Region;
class Widget;

/**
 * Class to handle a graphics context.  It's used to handle the Win32
 * HDCs.
 */
class VACA_DLL Graphics : private boost::noncopyable
{

  friend class Application;

  struct _Pen
  {
    HPEN handle;
    int style;
    int width;
    COLORREF color;

    _Pen(int style, int width, COLORREF color);
    ~_Pen();
  };

  struct _Brush
  {
    HBRUSH handle;
    int style;
    int hatch;
    COLORREF color;

    _Brush(int style, int hatch, COLORREF color);
    ~_Brush();
  };

  static std::list<_Pen *> mPens;
  static std::list<_Brush *> mBrushes;

  HDC   m_HDC;
  bool  m_autoRelease : 1;
  bool  m_autoDelete : 1;
  bool  m_noPaint : 1;
  Color m_color;
  Font *m_font;
  int   m_penStyle;
  int   m_penWidth;

protected:
  
  Graphics();			// accessible through ScreenGraphics

public:

  Graphics(HDC hdc);
  Graphics(HDC hdc, Image &image);
  Graphics(Widget *widget);
  virtual ~Graphics();

  void noPaint();
  bool wasPainted();

  Rect getClipBounds();
  void getClipRegion(Region &rgn);
  void setClipRegion(Region &rgn);
  void excludeClipRect(const Rect &rc);
  void excludeClipRegion(Region &rgn);
  void intersectClipRect(const Rect &rc);
  void intersectClipRegion(Region &rgn);
  void addClipRegion(Region &rgn);
  void xorClipRegion(Region &rgn);

  bool isVisible(const Point &pt);
  bool isVisible(const Rect &rc);

//   void setPaintMode();
//   void setXORMode();

  Color getColor();
  void setColor(const Color &color);

  Font *getFont();
  void setFont(Font *font);

  void setPenStyle(int penStyle);
  void setPenWidth(int penWidth);

  double getMiterLimit();
  void setMiterLimit(double limit);
  
  Color getPixel(const Point &pt);
  Color getPixel(int x, int y);
  void setPixel(const Point &pt, const Color &color);
  void setPixel(int x, int y, const Color &color);

  void beginPath();
  void endPath();
  void strokePath(Pen &pen);
  void fillPath(Brush &brush);

  void moveTo(const Point &pt);
  void moveTo(int x, int y);
  void lineTo(Pen &pen, const Point &pt);
  void lineTo(Pen &pen, int x, int y);
  void curveTo(int x1, int y1, int x2, int y2, int x3, int y3);
  void curveTo(const Point &pt1, const Point &pt2, const Point &pt3);
  void curveTo(const std::vector<Point> &points);
  void closeFigure();

  void getRegionFromPath(Region &region);

  void drawString(const String &str, const Point &pt);
  void drawString(const String &str, int x, int y);
  void drawString(const String &str, const Rect &rc, int flags = DT_WORDBREAK);
  void drawDisabledString(const String &str, const Rect &rc, int flags = DT_WORDBREAK);

  void drawImage(Image &image, int x, int y);
  void drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height);
  void drawImage(Image &image, int x, int y, const Color &bgColor);
  void drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height, const Color &bgColor);
  void drawImage(Image &image, const Point &pt);
  void drawImage(Image &image, const Point &pt, const Rect &rc);
  void drawImage(Image &image, const Point &pt, const Color &bgColor);
  void drawImage(Image &image, const Point &pt, const Rect &rc, const Color &bgColor);

  void drawImageList(ImageList &imageList, int imageIndex, int x, int y, int style);
  void drawImageList(ImageList &imageList, int imageIndex, const Point &pt, int style);

  void drawLine(Pen &pen, const Point &pt1, const Point &pt2);
  void drawLine(Pen &pen, int x1, int y1, int x2, int y2);
  void drawBezier(Pen &pen, const Point points[4]);
  void drawBezier(Pen &pen, const std::vector<Point> &points);
  void drawBezier(Pen &pen, const Point &pt1, const Point &pt2, const Point &pt3, const Point &pt4);
  void drawBezier(Pen &pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
  void drawRect(Pen &pen, const Rect &rc);
  void drawRect(Pen &pen, int x, int y, int w, int h);
  void drawRoundRect(Pen &pen, const Rect &rc, const Size &ellipse);
  void drawRoundRect(Pen &pen, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight);
  void draw3dRect(const Rect &rc, const Color &topLeft, const Color &bottomRight);
  void draw3dRect(int x, int y, int w, int h, const Color &topLeft, const Color &bottomRight);
  void drawEllipse(Pen &pen, const Rect &rc);
  void drawEllipse(Pen &pen, int x, int y, int w, int h);
  void drawArc(Pen &pen, const Rect &rc, double startAngle, double sweepAngle);
  void drawArc(Pen &pen, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void drawPie(Pen &pen, const Rect &rc, double startAngle, double sweepAngle);
  void drawPie(Pen &pen, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void drawChord(Pen &pen, const Rect &rc, double startAngle, double sweepAngle);
  void drawChord(Pen &pen, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void drawPolyline(Pen &pen, const std::vector<Point> &points);
  
  void fillRect(Brush &brush, const Rect &rc);
  void fillRect(Brush &brush, int x, int y, int w, int h);
  void fillRoundRect(Brush &brush, const Rect &rc, const Size &ellipse);
  void fillRoundRect(Brush &brush, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight);
  void fillEllipse(Brush &brush, const Rect &rc);
  void fillEllipse(Brush &brush, int x, int y, int w, int h);
  void fillPie(Brush &brush, const Rect &rc, double startAngle, double sweepAngle);
  void fillPie(Brush &brush, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void fillChord(Brush &brush, const Rect &rc, double startAngle, double sweepAngle);
  void fillChord(Brush &brush, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void fillRegion(Brush &brush, const Region &rgn);

  void fillGradientRect(const Rect &rc, const Color &startColor, const Color &endColor, Orientation orientation);
  void fillGradientRect(int x, int y, int w, int h, const Color &startColor, const Color &endColor, Orientation orientation);
  void drawGradientRect(const Rect &rc, const Color &topLeft, const Color &topRight, const Color &bottomLeft, const Color &bottomRight);
  void drawGradientRect(int x, int y, int w, int h, const Color &topLeft, const Color &topRight, const Color &bottomLeft, const Color &bottomRight);

  void drawXorFrame(const Rect &rc, int border = 3);
  void drawXorFrame(int x, int y, int w, int h, int border = 3);

  void drawFocus(const Rect &rc);

  // 32767 is the limit for Win98
  Size measureString(const String &str, int fitInWidth = 32767, int flags = DT_WORDBREAK);

  // SetROP2 wrapper
  void setRop2(int drawMode);

  HDC getHDC();

  static HPEN findHPEN(int style, int width, COLORREF color);
  static HBRUSH findHBRUSH(int style, int hatch, COLORREF color);

private:
  
  static void deleteHandles();
  void drawBezier(Pen &pen, CONST POINT *lppt, int numPoints);
  void drawBezierTo(CONST POINT *lppt, int numPoints);
  void drawPolyline(Pen &pen, CONST POINT *lppt, int numPoints);
  
};

/**
 * Class to draw directly in the screen.
 */
class VACA_DLL ScreenGraphics : public Graphics
{
public:
  ScreenGraphics();
  virtual ~ScreenGraphics();
};

} // namespace Vaca

#endif
