// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_GRAPHICS_H
#define VACA_GRAPHICS_H

#include "vaca/base.h"
#include "vaca/NonCopyable.h"
#include "vaca/Rect.h"
#include "vaca/Font.h"

#include <list>
#include <vector>

namespace vaca {

// ======================================================================

enum class FillRule
{
  EvenOdd,
  Winding
};

/**
   Class to control a graphics context.

   @warning
     This is a Win32's HDC wrapper.
*/
class VACA_DLL Graphics : private NonCopyable
{

  friend class Application;

  HDC m_handle;
  HPEN m_nullPen;
  HBRUSH m_nullBrush;
  bool m_autoRelease : 1;
  bool m_autoDelete : 1;
  Font m_font;
  FillRule m_fillRule;

protected:

  Graphics();			// accessible through ScreenGraphics

public:

  Graphics(HDC hdc);
  Graphics(HDC hdc, Image& image);
  Graphics(Widget* widget);
  virtual ~Graphics();

  Rect getClipBounds();
  void getClipRegion(Region& rgn);
  void setClipRegion(Region& rgn);
  void excludeClipRect(const Rect& rc);
  void excludeClipRegion(Region& rgn);
  void intersectClipRect(const Rect& rc);
  void intersectClipRegion(Region& rgn);
  void addClipRegion(Region& rgn);
  void xorClipRegion(Region& rgn);

  bool isVisible(const Point& pt);
  bool isVisible(const Rect& rc);

  // ======================================================================
  // Font

  Font getFont() const;
  void setFont(Font font);
  void getFontMetrics(FontMetrics& fontMetrics);

  // ======================================================================
  // Pîxel

  Color getPixel(const Point& pt);
  Color getPixel(int x, int y);
  void setPixel(const Point& pt, const Color& color);
  void setPixel(int x, int y, const Color& color);

  // ======================================================================
  // Low-level path routines

  double getMiterLimit() const;
  void setMiterLimit(double limit);

  FillRule getFillRule() const;
  void setFillRule(FillRule fillRule);

  void tracePath(const GraphicsPath& path, const Point& pt);
  void getPath(GraphicsPath& path) const;
  Region getRegionFromPath() const;

  void strokePath(const Pen& pen);
  void fillPath(const Brush& brush);
  void strokeAndFillPath(const Pen& pen, const Brush& brush);

  // ======================================================================
  // High-level path routines

  void strokePath(const GraphicsPath& path, const Pen& pen, const Point& pt);
  void fillPath(const GraphicsPath& path, const Brush& brush, const Point& pt);
  void strokeAndFillPath(const GraphicsPath& path, const Pen& pen, const Brush& brush, const Point& pt);

  // ======================================================================

  void drawString(const String& str, const Color& color, const Point& pt);
  void drawString(const String& str, const Color& color, int x, int y);
  void drawString(const String& str, const Color& color, const Rect& rc, int flags = DT_WORDBREAK);
  void drawDisabledString(const String& str, const Rect& rc, int flags = DT_WORDBREAK);

  void drawImage(Image& image, int x, int y);
  void drawImage(Image& image, int dstX, int dstY, int srcX, int srcY, int width, int height);
  void drawImage(Image& image, int x, int y, const Color& bgColor);
  void drawImage(Image& image, int dstX, int dstY, int srcX, int srcY, int width, int height, const Color& bgColor);
  void drawImage(Image& image, const Point& pt);
  void drawImage(Image& image, const Point& pt, const Rect& rc);
  void drawImage(Image& image, const Point& pt, const Color& bgColor);
  void drawImage(Image& image, const Point& pt, const Rect& rc, const Color& bgColor);

  void drawImageList(ImageList& imageList, int imageIndex, int x, int y, int style);
  void drawImageList(ImageList& imageList, int imageIndex, const Point& pt, int style);

  void drawLine(const Pen& pen, const Point& pt1, const Point& pt2);
  void drawLine(const Pen& pen, int x1, int y1, int x2, int y2);
  void drawBezier(const Pen& pen, const Point points[4]);
  void drawBezier(const Pen& pen, const std::vector<Point>& points);
  void drawBezier(const Pen& pen, const Point& pt1, const Point& pt2, const Point& pt3, const Point& pt4);
  void drawBezier(const Pen& pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
  void drawRect(const Pen& pen, const Rect& rc);
  void drawRect(const Pen& pen, int x, int y, int w, int h);
  void drawRoundRect(const Pen& pen, const Rect& rc, const Size& ellipse);
  void drawRoundRect(const Pen& pen, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight);
  void draw3dRect(const Rect& rc, const Color& topLeft, const Color& bottomRight);
  void draw3dRect(int x, int y, int w, int h, const Color& topLeft, const Color& bottomRight);
  void drawEllipse(const Pen& pen, const Rect& rc);
  void drawEllipse(const Pen& pen, int x, int y, int w, int h);
  void drawArc(const Pen& pen, const Rect& rc, double startAngle, double sweepAngle);
  void drawArc(const Pen& pen, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void drawPie(const Pen& pen, const Rect& rc, double startAngle, double sweepAngle);
  void drawPie(const Pen& pen, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void drawChord(const Pen& pen, const Rect& rc, double startAngle, double sweepAngle);
  void drawChord(const Pen& pen, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void drawPolyline(const Pen& pen, const std::vector<Point>& points);

  void fillRect(const Brush& brush, const Rect& rc);
  void fillRect(const Brush& brush, int x, int y, int w, int h);
  void fillRoundRect(const Brush& brush, const Rect& rc, const Size& ellipse);
  void fillRoundRect(const Brush& brush, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight);
  void fillEllipse(const Brush& brush, const Rect& rc);
  void fillEllipse(const Brush& brush, int x, int y, int w, int h);
  void fillPie(const Brush& brush, const Rect& rc, double startAngle, double sweepAngle);
  void fillPie(const Brush& brush, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void fillChord(const Brush& brush, const Rect& rc, double startAngle, double sweepAngle);
  void fillChord(const Brush& brush, int x, int y, int w, int h, double startAngle, double sweepAngle);
  void fillRegion(const Brush& brush, const Region& rgn);

  void fillGradientRect(const Rect& rc, const Color& startColor, const Color& endColor, Orientation orientation);
  void fillGradientRect(int x, int y, int w, int h, const Color& startColor, const Color& endColor, Orientation orientation);
  void drawGradientRect(const Rect& rc, const Color& topLeft, const Color& topRight, const Color& bottomLeft, const Color& bottomRight);
  void drawGradientRect(int x, int y, int w, int h, const Color& topLeft, const Color& topRight, const Color& bottomLeft, const Color& bottomRight);

  void drawXorFrame(const Rect& rc, int border = 3);
  void drawXorFrame(int x, int y, int w, int h, int border = 3);
  void fillXorFrame(const Rect& rc);
  void fillXorFrame(int x, int y, int w, int h);

  void drawFocus(const Rect& rc);

  // 32767 is the limit for Win98
  Size measureString(const String& str, int fitInWidth = 32767, int flags = DT_WORDBREAK);

  // SetROP2 wrapper
  void setRop2(int drawMode);

  HDC getHandle() const;

private:

  void initialize();

  void drawBezier(const Pen& pen, CONST POINT* lppt, int numPoints);
  void drawBezierTo(const Pen& pen, CONST POINT* lppt, int numPoints);
  void drawPolyline(const Pen& pen, CONST POINT* lppt, int numPoints);

};

/**
   Class to draw directly in the screen.
*/
class VACA_DLL ScreenGraphics : public Graphics
{
public:
  ScreenGraphics();
  virtual ~ScreenGraphics();
};

} // namespace vaca

#endif // VACA_GRAPHICS_H
