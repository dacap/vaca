// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, 2009, David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#include "Vaca/Graphics.h"
#include "Vaca/Image.h"
#include "Vaca/ImageList.h"
#include "Vaca/Debug.h"
#include "Vaca/Font.h"
#include "Vaca/Rect.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"
#include "Vaca/Widget.h"
#include "Vaca/System.h"
#include "Vaca/Region.h"
#include "Vaca/Pen.h"
#include "Vaca/Brush.h"
#include "Vaca/GraphicsPath.h"

#include <cmath>
#ifndef M_PI
#  define M_PI 3.14159265358979323846 
#endif

#ifndef GRADIENT_FILL_RECT_H
  #define GRADIENT_FILL_RECT_H 0
  #define GRADIENT_FILL_RECT_V 1
#endif

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Graphics

void Graphics::initialize()
{
  LOGBRUSH lb;
  lb.lbStyle = BS_NULL;
  lb.lbColor = 0;
  lb.lbHatch = 0;

  m_nullPen = ::CreatePen(PS_NULL, 0, 0);
  m_nullBrush = ::CreateBrushIndirect(&lb);

  m_noPaint = false;
  m_fillRule = FillRule::EvenOdd;
}
    
/**
 * Creates a Graphics context for the screen.
 */
Graphics::Graphics()
{
  m_handle      = ::GetDC(NULL);
  m_autoRelease = true;
  m_autoDelete  = false;

  initialize();
}

/**
 * Creates a Graphics context related to the specified HDC.
 */
Graphics::Graphics(HDC hdc)
{
  assert(hdc != NULL);

  m_handle         = hdc;
  m_autoRelease = false;
  m_autoDelete  = false;
  m_font        = NULL;

  initialize();
}

Graphics::Graphics(HDC hdc, Image& image)
{
  assert(hdc != NULL);

  m_handle         = CreateCompatibleDC(hdc);
  m_autoRelease = false;
  m_autoDelete  = true;
  m_font        = NULL;
  
  SelectObject(m_handle, image.getHandle());
  initialize();
}

Graphics::Graphics(Widget* widget)
{
  HWND hwnd = widget->getHandle();

  assert(::IsWindow(hwnd));

  m_handle      = ::GetDC(hwnd);
  m_autoRelease = true;
  m_autoDelete  = false;
  m_font        = widget->getFont();

  initialize();
}

Graphics::~Graphics()
{
  ::DeleteObject(reinterpret_cast<HGDIOBJ>(m_nullBrush));
  ::DeleteObject(reinterpret_cast<HGDIOBJ>(m_nullPen));

  if (m_autoRelease)
    ::ReleaseDC(NULL, m_handle);
  else if (m_autoDelete)
    ::DeleteDC(m_handle);
}

/**
 * Indicates that the Graphics wasn't touched. It's called by the
 * default implementation of the Widget::onPaint.
 */
void Graphics::noPaint()
{
  m_noPaint = true;
}

bool Graphics::wasPainted()
{
  return !m_noPaint;
}

Rect Graphics::getClipBounds()
{
  assert(m_handle);

  RECT rc;
  int res = ::GetClipBox(m_handle, &rc);

  if (res == NULLREGION ||
      res == ERROR)
    return Rect();
  else
    return Rect(&rc);
}

void Graphics::getClipRegion(Region& rgn)
{
  assert(m_handle);
  // TODO Region::assign(const Rect& rc)
  rgn = Region::fromRect(getClipBounds());
  GetClipRgn(m_handle, rgn.getHandle());
}

void Graphics::setClipRegion(Region& rgn)
{
  assert(m_handle);
  ExtSelectClipRgn(m_handle, rgn.getHandle(), RGN_COPY);
}

void Graphics::excludeClipRect(const Rect& rc)
{
  assert(m_handle);
  ExcludeClipRect(m_handle, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

void Graphics::excludeClipRegion(Region& rgn)
{
  assert(m_handle);
  ExtSelectClipRgn(m_handle, rgn.getHandle(), RGN_DIFF);
}

void Graphics::intersectClipRect(const Rect& rc)
{
  assert(m_handle);
  IntersectClipRect(m_handle, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

void Graphics::intersectClipRegion(Region& rgn)
{
  assert(m_handle);
  ExtSelectClipRgn(m_handle, rgn.getHandle(), RGN_AND);
}

void Graphics::addClipRegion(Region& rgn)
{
  assert(m_handle);
  ExtSelectClipRgn(m_handle, rgn.getHandle(), RGN_OR);
}

void Graphics::xorClipRegion(Region& rgn)
{
  assert(m_handle);
  ExtSelectClipRgn(m_handle, rgn.getHandle(), RGN_XOR);
}

bool Graphics::isVisible(const Point& pt)
{
  assert(m_handle);
  return PtVisible(m_handle, pt.x, pt.y) != FALSE;
}

bool Graphics::isVisible(const Rect& rc)
{
  assert(m_handle);
  RECT rc2 = rc;
  return RectVisible(m_handle, &rc2) != FALSE;
}

Font Graphics::getFont() const
{
  return m_font;
}

void Graphics::setFont(Font font)
{
  m_font = font;
}

void Graphics::getFontMetrics(FontMetrics& fontMetrics)
{
  HGDIOBJ oldFont = SelectObject(m_handle, reinterpret_cast<HGDIOBJ>(m_font.getHandle()));
  GetTextMetrics(m_handle, &fontMetrics.m_textMetric);
  SelectObject(m_handle, oldFont);
}

Color Graphics::getPixel(const Point& pt)
{
  assert(m_handle);

  return Color(GetPixel(m_handle, pt.x, pt.y));
}

Color Graphics::getPixel(int x, int y)
{
  assert(m_handle);

  return Color(GetPixel(m_handle, x, y));
}

void Graphics::setPixel(const Point& pt, const Color& color)
{
  assert(m_handle);

  SetPixel(m_handle, pt.x, pt.y, color.getColorRef());
}

void Graphics::setPixel(int x, int y, const Color& color)
{
  assert(m_handle);

  SetPixel(m_handle, x, y, color.getColorRef());
}

double Graphics::getMiterLimit() const
{
  assert(m_handle);

  float limit;

  if (GetMiterLimit(m_handle, &limit) != FALSE)
    return limit;
  else
    return 10.0;
}

void Graphics::setMiterLimit(double limit)
{
  assert(m_handle);

  SetMiterLimit(m_handle, static_cast<FLOAT>(limit), NULL);
}

FillRule Graphics::getFillRule() const
{
  return m_fillRule;
}

void Graphics::setFillRule(FillRule fillRule)
{
  m_fillRule = fillRule;
}

void Graphics::tracePath(const GraphicsPath& path, const Point& pt)
{
  GraphicsPath::const_iterator it = path.begin();
  GraphicsPath::const_iterator end = path.end();

  assert(m_handle);

  BeginPath(m_handle);
  MoveToEx(m_handle, pt.x, pt.y, NULL);
  
  for (; it != end; ++it) {
    switch (it->getType()) {
      case GraphicsPath::MoveTo:
  	MoveToEx(m_handle,
		 pt.x + it->getPoint().x,
		 pt.y + it->getPoint().y, NULL);
  	break;
      case GraphicsPath::LineTo:
  	LineTo(m_handle,
	       pt.x + it->getPoint().x,
	       pt.y + it->getPoint().y);
  	break;
      case GraphicsPath::BezierControl1: {
  	POINT pts[3];
  	pts[0].x = pt.x + it->getPoint().x;
  	pts[0].y = pt.y + it->getPoint().y;

	++it;
	assert(it->getType() == GraphicsPath::BezierControl2);

	pts[1].x = pt.x + it->getPoint().x;
  	pts[1].y = pt.y + it->getPoint().y;

	++it;
	assert(it->getType() == GraphicsPath::BezierTo);

	pts[2].x = pt.x + it->getPoint().x;
  	pts[2].y = pt.y + it->getPoint().y;

	PolyBezierTo(m_handle, pts, 3);
  	break;
      }
    }
    if (it->isCloseFigure())
      CloseFigure(m_handle);
  }

  EndPath(m_handle);
}

void Graphics::getPath(GraphicsPath& path) const
{
  assert(m_handle);

  path.clear();

  int npoints = GetPath(m_handle, NULL, NULL, 0);
  if (npoints > 0) {
    LPPOINT points = new POINT[npoints];
    LPBYTE types = new BYTE[npoints];

    npoints = GetPath(m_handle, points, types, npoints);

    for (int c=0; c<npoints; ++c) {
      switch (types[c] & 6) {
	case PT_MOVETO:
	  path.moveTo(points[c].x, points[c].y);
	  break;
	case PT_LINETO:
	  path.lineTo(points[c].x, points[c].y);
	  break;
	case PT_BEZIERTO:
	  path.curveTo(points[c  ].x, points[c  ].y,
		       points[c+1].x, points[c+1].y,
		       points[c+2].x, points[c+2].y);
	  c += 2;
	  break;
      }
      if (types[c] & PT_CLOSEFIGURE)
	path.closeFigure();
    }

    delete types;
    delete points;
  }
}

Region Graphics::getRegionFromPath() const
{
  assert(m_handle);
  if (HRGN hrgn = PathToRegion(m_handle))
    return Region(hrgn);
  else
    return Region();
}
  
void Graphics::strokePath(const Pen& pen)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, pen.getHandle());
  HGDIOBJ oldBrush = SelectObject(m_handle, m_nullBrush);

  StrokePath(m_handle);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::fillPath(const Brush& brush)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, m_nullPen);
  HGDIOBJ oldBrush = SelectObject(m_handle, brush.getHandle());

  int oldPolyFillMode = SetPolyFillMode(m_handle,
					m_fillRule == FillRule::EvenOdd ? ALTERNATE: WINDING);
  FillPath(m_handle);
  SetPolyFillMode(m_handle, oldPolyFillMode);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::strokeAndFillPath(const Pen& pen, const Brush& brush)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, pen.getHandle());
  HGDIOBJ oldBrush = SelectObject(m_handle, brush.getHandle());

  int oldPolyFillMode = SetPolyFillMode(m_handle, ALTERNATE); // TODO ALTERNATE or WINDING
  StrokeAndFillPath(m_handle);
  SetPolyFillMode(m_handle, oldPolyFillMode);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::strokePath(const GraphicsPath& path, const Pen& pen, const Point& pt)
{
  tracePath(path, pt);
  strokePath(pen);
}

void Graphics::fillPath(const GraphicsPath& path, const Brush& brush, const Point& pt)
{
  tracePath(path, pt);
  fillPath(brush);
}

void Graphics::strokeAndFillPath(const GraphicsPath& path, const Pen& pen, const Brush& brush, const Point& pt)
{
  tracePath(path, pt);
  strokeAndFillPath(pen, brush);
}

void Graphics::drawString(const String& str, const Color& color, const Point& pt)
{
  drawString(str, color, pt.x, pt.y);
}

void Graphics::drawString(const String& str, const Color& color, int x, int y)
{
  assert(m_handle);

  int oldMode = SetBkMode(m_handle, TRANSPARENT);
  int oldColor = SetTextColor(m_handle, color.getColorRef());

  HGDIOBJ oldFont = SelectObject(m_handle, reinterpret_cast<HGDIOBJ>(m_font.getHandle()));
  TextOut(m_handle, x, y, str.c_str(), static_cast<int>(str.size()));
  SelectObject(m_handle, oldFont);

  SetBkMode(m_handle, oldMode); 
  SetTextColor(m_handle, oldColor);
}

void Graphics::drawString(const String& str, const Color& color, const Rect& _rc, int flags)
{
  assert(m_handle);

  int oldMode = SetBkMode(m_handle, TRANSPARENT);
  int oldColor = SetTextColor(m_handle, color.getColorRef());

  RECT rc = _rc;

  HGDIOBJ oldFont = SelectObject(m_handle, reinterpret_cast<HGDIOBJ>(m_font.getHandle()));
  DrawText(m_handle, str.c_str(), static_cast<int>(str.size()), &rc, flags);
  SelectObject(m_handle, oldFont);

  SetBkMode(m_handle, oldMode); 
  SetTextColor(m_handle, oldColor);
}

void Graphics::drawDisabledString(const String& str, const Rect& rc, int flags)
{
  drawString(str, System::getColor(COLOR_3DHIGHLIGHT), Rect(rc.x+1, rc.y+1, rc.w, rc.h), flags);
  drawString(str, System::getColor(COLOR_GRAYTEXT), rc, flags);
}

void Graphics::drawImage(Image& image, int x, int y)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight());
}

void Graphics::drawImage(Image& image, int dstX, int dstY, int srcX, int srcY, int width, int height)
{
  assert(m_handle);

  Graphics& source = image.getGraphics();

  assert(source.getHandle());
  
  BitBlt(m_handle, dstX, dstY, width, height, source.getHandle(), srcX, srcY, SRCCOPY);
}

void Graphics::drawImage(Image& image, int x, int y, const Color& bgColor)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight(), bgColor);
}

void Graphics::drawImage(Image& image, int dstX, int dstY, int srcX, int srcY, int width, int height, const Color& bgColor)
{
  assert(m_handle);

  Graphics& source = image.getGraphics();

  assert(source.getHandle());

#if 0				// WinCE
  TransparentImage(m_handle, dstX, dstY, width, height,
		   source.getHandle(), srcX, srcY, width, height,
		   bgColor.getColorRef());
#else
    
  HDC maskHDC = CreateCompatibleDC(m_handle);
  HBITMAP theMask = CreateBitmap(width, height, 1, 1, NULL);
  HGDIOBJ oldMask = SelectObject(maskHDC, theMask);
  COLORREF oldBkColor = SetBkColor(source.getHandle(), bgColor.getColorRef());
  
  BitBlt(maskHDC, 0, 0, width, height,
	 source.getHandle(), srcX, srcY, SRCCOPY);
  
  MaskBlt(m_handle, dstX, dstY, width, height,
	  source.getHandle(), srcX, srcY,
	  theMask, 0, 0, MAKEROP4(0x00AA0029, SRCCOPY)); // 0x00AA0029 is NOP

  SetBkColor(source.getHandle(), oldBkColor);
  DeleteObject(SelectObject(maskHDC, oldMask));
  DeleteObject(maskHDC);

#endif
}

void Graphics::drawImage(Image& image, const Point& pt)
{
  drawImage(image, pt.x, pt.y);
}

void Graphics::drawImage(Image& image, const Point& pt, const Rect& rc)
{
  drawImage(image, pt.x, pt.y, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawImage(Image& image, const Point& pt, const Color& bgColor)
{
  drawImage(image, pt.x, pt.y, bgColor);
}

void Graphics::drawImage(Image& image, const Point& pt, const Rect& rc, const Color& bgColor)
{
  drawImage(image, pt.x, pt.y, rc.x, rc.y, rc.w, rc.h, bgColor);
}

/**
 * Draws the specified image of the ImageList.
 * 
 * @param imageList
 *     List of image to get the specified image.
 *
 * @param imageIndex
 *     Specific image to draw. This must be a valid index of the image
 *     list. You can check the size of the ImageList using ImageList#getImageCount(),
 *     so the index must be between @c 0 and @c getImageCount-1.
 * 
 * @param x
 *     TODO
 * 
 * @param y
 *     TODO
 * 
 * @param style
 *     One of the following values:
 *     @li ILD_BLEND25
 *     @li ILD_FOCUS
 *     @li ILD_BLEND50
 *     @li ILD_SELECTED
 *     @li ILD_BLEND
 *     @li ILD_MASK
 *     @li ILD_NORMAL
 *     @li ILD_TRANSPARENT
 */
void Graphics::drawImageList(ImageList& imageList, int imageIndex, int x, int y, int style)
{
  assert(m_handle);
  assert(imageList.getHandle());

  ImageList_Draw(imageList.getHandle(),
		 imageIndex, m_handle, x, y, style);
}

void Graphics::drawImageList(ImageList& imageList, int imageIndex, const Point& pt, int style)
{
  drawImageList(imageList, imageIndex, pt.x, pt.y, style);
}

void Graphics::drawLine(const Pen& pen, const Point& pt1, const Point& pt2)
{
  drawLine(pen, pt1.x, pt1.y, pt2.x, pt2.y);
}

void Graphics::drawLine(const Pen& pen, int x1, int y1, int x2, int y2)
{
  assert(m_handle);

  POINT oldPos;
  HGDIOBJ oldPen = SelectObject(m_handle, pen.getHandle());

  MoveToEx(m_handle, x1, y1, &oldPos);
  LineTo(m_handle, x2, y2);
  MoveToEx(m_handle, oldPos.x, oldPos.y, NULL);

  SelectObject(m_handle, oldPen);
}

void Graphics::drawBezier(const Pen& pen, const Point points[4])
{
  POINT pt[4];
  std::copy(points, points+4, pt);
  drawBezier(pen, pt, 4);
}

void Graphics::drawBezier(const Pen& pen, const std::vector<Point>& points)
{
  int numPoints = static_cast<int>(points.size());

  assert(numPoints >= 4);

  POINT *pt = new POINT[numPoints];
  std::copy(points.begin(), points.end(), pt);
  drawBezier(pen, pt, numPoints);
  delete[] pt;
}

void Graphics::drawBezier(const Pen& pen, const Point& pt1, const Point& pt2, const Point& pt3, const Point& pt4)
{
  POINT pt[4];
  pt[0] = pt1;
  pt[1] = pt2;
  pt[2] = pt3;
  pt[3] = pt4;
  drawBezier(pen, pt, 4);
}

void Graphics::drawBezier(const Pen& pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
  POINT pt[4];
  pt[0].x = x1;
  pt[0].y = y1;
  pt[1].x = x2;
  pt[1].y = y2;
  pt[2].x = x3;
  pt[2].y = y3;
  pt[3].x = x4;
  pt[3].y = y4;
  drawBezier(pen, pt, 4);
}

void Graphics::drawRect(const Pen& pen, const Rect& rc)
{
  drawRect(pen, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawRect(const Pen& pen, int x, int y, int w, int h)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, pen.getHandle());
  HGDIOBJ oldBrush = SelectObject(m_handle, m_nullBrush);

  Rectangle(m_handle, x, y, x+w, y+h);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::drawRoundRect(const Pen& pen, const Rect& rc, const Size& ellipse)
{
  drawRoundRect(pen, rc.x, rc.y, rc.w, rc.h, ellipse.w, ellipse.h);
}

void Graphics::drawRoundRect(const Pen& pen, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, pen.getHandle());
  HGDIOBJ oldBrush = SelectObject(m_handle, m_nullBrush);

  RoundRect(m_handle, x, y, x+w, y+h, ellipseWidth, ellipseHeight);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::draw3dRect(const Rect& rc, const Color& topLeft, const Color& bottomRight)
{
  draw3dRect(rc.x, rc.y, rc.w, rc.h, topLeft, bottomRight);
}

void Graphics::draw3dRect(int x, int y, int w, int h, const Color& topLeft, const Color& bottomRight)
{
  assert(m_handle);

  HPEN pen1 = CreatePen(PS_SOLID, 1, topLeft.getColorRef());
  HPEN pen2 = CreatePen(PS_SOLID, 1, bottomRight.getColorRef());

  POINT oldPos;
  HGDIOBJ oldPen = SelectObject(m_handle, pen1);
  MoveToEx(m_handle, x, y+h-2, &oldPos);
  LineTo(m_handle, x, y);
  LineTo(m_handle, x+w-1, y);

  SelectObject(m_handle, pen2);
  LineTo(m_handle, x+w-1, y+h-1);
  LineTo(m_handle, x-1, y+h-1);

  SelectObject(m_handle, oldPen);
  MoveToEx(m_handle, oldPos.x, oldPos.y, NULL);

  DeleteObject(pen1);
  DeleteObject(pen2);
}

/**
 * Draws the outline of an ellipse. It uses the current selected color
 * (see setColor), and doesn't paint the background.
 */
void Graphics::drawEllipse(const Pen& pen, const Rect& rc)
{
  drawEllipse(pen, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawEllipse(const Pen& pen, int x, int y, int w, int h)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, pen.getHandle());
  HGDIOBJ oldBrush = SelectObject(m_handle, m_nullBrush);

  Ellipse(m_handle, x, y, x+w, y+h);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

/**
 * Draws an arc with rc as a bounding rectangle for the ellipse that
 * encloses the arc. The arc start in the startAngle (a value between
 * -360 and 360), and as a arc length of sweepAngle (in
 * counter-clockwise).
 */
void Graphics::drawArc(const Pen& pen, const Rect& rc, double startAngle, double sweepAngle)
{
  drawArc(pen, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawArc(const Pen& pen, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_handle);

  HGDIOBJ oldPen = SelectObject(m_handle, pen.getHandle());
  int x1, y1, x2, y2;

  x1 = x+w/2 + static_cast<int>(std::cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - static_cast<int>(std::sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + static_cast<int>(std::cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - static_cast<int>(std::sin((startAngle+sweepAngle)*M_PI/180)*h);

  Arc(m_handle, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_handle, oldPen);
}

void Graphics::drawPie(const Pen& pen, const Rect& rc, double startAngle, double sweepAngle)
{
  drawPie(pen, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawPie(const Pen& pen, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, pen.getHandle());
  HGDIOBJ oldBrush = SelectObject(m_handle, m_nullBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + static_cast<int>(std::cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - static_cast<int>(std::sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + static_cast<int>(std::cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - static_cast<int>(std::sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(m_handle, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::drawChord(const Pen& pen, const Rect& rc, double startAngle, double sweepAngle)
{
  drawChord(pen, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawChord(const Pen& pen, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, pen.getHandle());
  HGDIOBJ oldBrush = SelectObject(m_handle, m_nullBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + static_cast<int>(std::cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - static_cast<int>(std::sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + static_cast<int>(std::cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - static_cast<int>(std::sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(m_handle, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::drawPolyline(const Pen& pen, const std::vector<Point>& points)
{
  int numPoints = static_cast<int>(points.size());

  assert(numPoints >= 2);

  POINT *pt = new POINT[numPoints];
  std::copy(points.begin(), points.end(), pt);
  drawPolyline(pen, pt, numPoints);
  delete[] pt;
}

void Graphics::fillRect(const Brush& brush, const Rect& rc)
{
  fillRect(brush, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillRect(const Brush& brush, int x, int y, int w, int h)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, m_nullPen);
  HGDIOBJ oldBrush = SelectObject(m_handle, brush.getHandle());

  Rectangle(m_handle, x, y, x+w+1, y+h+1);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::fillRoundRect(const Brush& brush, const Rect& rc, const Size& ellipse)
{
  fillRoundRect(brush, rc.x, rc.y, rc.w, rc.h, ellipse.w, ellipse.h);
}

void Graphics::fillRoundRect(const Brush& brush, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, m_nullPen);
  HGDIOBJ oldBrush = SelectObject(m_handle, brush.getHandle());

  RoundRect(m_handle, x, y, x+w, y+h, ellipseWidth, ellipseHeight);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::fillEllipse(const Brush& brush, const Rect& rc)
{
  fillEllipse(brush, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillEllipse(const Brush& brush, int x, int y, int w, int h)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, m_nullPen);
  HGDIOBJ oldBrush = SelectObject(m_handle, brush.getHandle());

  Ellipse(m_handle, x, y, x+w+1, y+h+1);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::fillPie(const Brush& brush, const Rect& rc, double startAngle, double sweepAngle)
{
  fillPie(brush, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillPie(const Brush& brush, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, m_nullPen);
  HGDIOBJ oldBrush = SelectObject(m_handle, brush.getHandle());
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(m_handle, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::fillChord(const Brush& brush, const Rect& rc, double startAngle, double sweepAngle)
{
  fillChord(brush, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillChord(const Brush& brush, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_handle);

  HGDIOBJ oldPen   = SelectObject(m_handle, m_nullPen);
  HGDIOBJ oldBrush = SelectObject(m_handle, brush.getHandle());
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(m_handle, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_handle, oldPen);
  SelectObject(m_handle, oldBrush);
}

void Graphics::fillRegion(const Brush& brush, const Region& rgn)
{
  assert(m_handle);

  FillRgn(m_handle,
	  const_cast<Region*>(&rgn)->getHandle(),
	  brush.getHandle());
}

void Graphics::fillGradientRect(const Rect& rc, const Color& startColor, const Color& endColor,
				Orientation orientation)
{
  fillGradientRect(rc.x, rc.y, rc.w, rc.h, startColor, endColor, orientation);
}

typedef BOOL (WINAPI * GFProc)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

void Graphics::fillGradientRect(int x, int y, int w, int h,
				const Color& startColor,
				const Color& endColor,
				Orientation orientation)
{
  assert(m_handle);
  
  TRIVERTEX vert[2] ;
  GRADIENT_RECT gRect;

  vert[0].x        = x;
  vert[0].y        = y;
  vert[0].Red      = startColor.getR() | (startColor.getR() << 8);
  vert[0].Green    = startColor.getG() | (startColor.getG() << 8);
  vert[0].Blue     = startColor.getB() | (startColor.getB() << 8);
  vert[0].Alpha    = 0xff00;

  vert[1].x        = x+w;
  vert[1].y        = y+h;
  vert[1].Red      = endColor.getR() | (endColor.getR() << 8);
  vert[1].Green    = endColor.getG() | (endColor.getG() << 8);
  vert[1].Blue     = endColor.getB() | (endColor.getB() << 8);
  vert[1].Alpha    = 0xff00;

  gRect.UpperLeft  = 0;
  gRect.LowerRight = 1;

#if (WINVER >= 0x0500)
  GradientFill(m_handle, vert, 2, &gRect, 1,
	       orientation == Orientation::Horizontal
	       ? GRADIENT_FILL_RECT_H:
		 GRADIENT_FILL_RECT_V);
#else
  static GFProc pGF = NULL;

  if (pGF == NULL) {
    HMODULE hMsImg32 = LoadLibrary(_T("msimg32.dll"));
    if (hMsImg32 != NULL)
      pGF = (GFProc)GetProcAddress(hMsImg32, "GradientFill");
  }

  if (pGF != NULL) {
    pGF(m_handle, vert, 2, &gRect, 1,
	orientation == Horizontal ? GRADIENT_FILL_RECT_H:
				    GRADIENT_FILL_RECT_V);
  }
#endif
}

void Graphics::drawGradientRect(const Rect& rc,
				const Color& topLeft, const Color& topRight,
				const Color& bottomLeft, const Color& bottomRight)
{
  drawGradientRect(rc.x, rc.y, rc.w, rc.h, topLeft, topRight, bottomLeft, bottomRight);
}

void Graphics::drawGradientRect(int x, int y, int w, int h,
				const Color& topLeft, const Color& topRight,
				const Color& bottomLeft, const Color& bottomRight)
{
  fillGradientRect(x,     y,     w, 1, topLeft,    topRight,    Orientation::Horizontal);
  fillGradientRect(x,     y,     1, h, topLeft,    bottomLeft,  Orientation::Vertical);
  fillGradientRect(x,     y+h-1, w, 1, bottomLeft, bottomRight, Orientation::Horizontal);
  fillGradientRect(x+w-1, y,     1, h, topRight,   bottomRight, Orientation::Vertical);
}

void Graphics::drawXorFrame(const Rect& rc, int border)
{
  drawXorFrame(rc.x, rc.y, rc.w, rc.h, border);
}

static WORD s_pattern[] = { 0x00aa, 0x0055, 0x00aa, 0x0055,
			    0x00aa, 0x0055, 0x00aa, 0x0055 };

void Graphics::drawXorFrame(int x, int y, int w, int h, int border)
{
  HBITMAP hbitmap = CreateBitmap(8, 8, 1, 1, s_pattern);
  HBRUSH newBrush = CreatePatternBrush(hbitmap);
  HANDLE oldBrush = SelectObject(m_handle, newBrush);

  PatBlt(m_handle, x+border,   y,          w-border,  border,   PATINVERT);
  PatBlt(m_handle, x+w-border, y+border,   border,    h-border, PATINVERT);
  PatBlt(m_handle, x,          y+h-border, w-border,  border,   PATINVERT);
  PatBlt(m_handle, x,          y,          border,    h-border, PATINVERT);

  DeleteObject(SelectObject(m_handle, oldBrush));
  DeleteObject(hbitmap);
}

void Graphics::fillXorFrame(const Rect& rc)
{
  fillXorFrame(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillXorFrame(int x, int y, int w, int h)
{
  HBITMAP hbitmap = CreateBitmap(8, 8, 1, 1, s_pattern);
  HBRUSH newBrush = CreatePatternBrush(hbitmap);
  HANDLE oldBrush = SelectObject(m_handle, newBrush);

  PatBlt(m_handle, x, y, w, h, PATINVERT);

  DeleteObject(SelectObject(m_handle, oldBrush));
  DeleteObject(hbitmap);
}

void Graphics::drawFocus(const Rect& rc)
{
  assert(m_handle);

  RECT _rc = rc;
  ::DrawFocusRect(m_handle, &_rc);
}

/**
 * @warning
 *   In Win98, 32767 is the limit for @a fitInWidth.
 */
Size Graphics::measureString(const String& str, int fitInWidth, int flags)
{
  assert(m_handle);

  RECT rc = Rect(0, 0, fitInWidth, 0);
  HGDIOBJ oldFont = SelectObject(m_handle, reinterpret_cast<HGDIOBJ>(m_font.getHandle()));

  if (!str.empty()) {
    DrawText(m_handle, str.c_str(), static_cast<int>(str.size()), &rc, flags | DT_CALCRECT);
  }
  else {
    SIZE sz;

    if (GetTextExtentPoint32(m_handle, L" ", 1, &sz)) {
      rc.right = sz.cx;
      rc.bottom = sz.cy;
    }
  }

  SelectObject(m_handle, oldFont);

  return Rect(&rc).getSize();
}

/**
 * Changes the current raster operation mode (SetROP2).
 *
 * @param drawMode
 * @li R2_BLACK
 * @li R2_COPYPEN
 * @li R2_MASKNOTPEN
 * @li R2_MASKPEN
 * @li R2_MASKPENNOT
 * @li R2_MERGENOTPEN
 * @li R2_MERGEPEN
 * @li R2_MERGEPENNOT
 * @li R2_NOP
 * @li R2_NOT
 * @li R2_NOTCOPYPEN
 * @li R2_NOTMASKPEN
 * @li R2_NOTMERGEPEN
 * @li R2_NOTXORPEN
 * @li R2_WHITE
 * @li R2_XORPEN
 */
void Graphics::setRop2(int drawMode)
{
  assert(m_handle);

  SetROP2(m_handle, drawMode);
}

HDC Graphics::getHandle() const
{
  return m_handle;
}

void Graphics::drawBezier(const Pen& pen, CONST POINT* lppt, int numPoints)
{
  assert(m_handle);

  HGDIOBJ oldPen = SelectObject(m_handle, pen.getHandle());
  PolyBezier(m_handle, lppt, numPoints);
  SelectObject(m_handle, oldPen);
}

void Graphics::drawBezierTo(const Pen& pen, CONST POINT* lppt, int numPoints)
{
  assert(m_handle);

  HGDIOBJ oldPen = SelectObject(m_handle, pen.getHandle());
  PolyBezierTo(m_handle, lppt, numPoints);
  SelectObject(m_handle, oldPen);
}

void Graphics::drawPolyline(const Pen& pen, CONST POINT* lppt, int numPoints)
{
  assert(m_handle);

  HGDIOBJ oldPen = SelectObject(m_handle, pen.getHandle());
  Polyline(m_handle, lppt, numPoints);
  SelectObject(m_handle, oldPen);
}

//////////////////////////////////////////////////////////////////////
// ScreenGraphics

/**
 * Creates a Graphics instance to draw in the screen (anywhere).
 */
ScreenGraphics::ScreenGraphics()
  : Graphics()
{
}

ScreenGraphics::~ScreenGraphics()
{
}
