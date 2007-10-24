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

#include "stdvaca.h"
#include "Vaca/Graphics.hpp"
#include "Vaca/Image.hpp"
#include "Vaca/ImageList.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/Font.hpp"
#include "Vaca/Rect.hpp"
#include "Vaca/Point.hpp"
#include "Vaca/Size.hpp"
#include "Vaca/Widget.hpp"
#include "Vaca/System.hpp"
#include "Vaca/Region.hpp"
#include "Vaca/Pen.hpp"
#include "Vaca/Brush.hpp"

#ifndef GRADIENT_FILL_RECT_H
  #define GRADIENT_FILL_RECT_H 0
  #define GRADIENT_FILL_RECT_V 1
#endif

using namespace Vaca;

std::list<Graphics::_Pen *> Graphics::mPens;
std::list<Graphics::_Brush *> Graphics::mBrushes;

//////////////////////////////////////////////////////////////////////
// Graphics::_Pen

Graphics::_Pen::_Pen(int style, int width, COLORREF color)
  : style(style)
  , width(width)
  , color(color)
{
  handle = CreatePen(style, width, color);
}

Graphics::_Pen::~_Pen()
{
  DeleteObject(reinterpret_cast<HGDIOBJ>(handle));
}

//////////////////////////////////////////////////////////////////////
// Graphics::_Brush

Graphics::_Brush::_Brush(int style, int hatch, COLORREF color)
  : style(style)
  , hatch(hatch)
  , color(color)
{
  LOGBRUSH lb;

  lb.lbStyle = style;
  lb.lbColor = color;
  lb.lbHatch = hatch;

  handle = CreateBrushIndirect(&lb);
}

Graphics::_Brush::~_Brush()
{
  DeleteObject(reinterpret_cast<HGDIOBJ>(handle));
}

//////////////////////////////////////////////////////////////////////
// Graphics

/**
 * Creates a Graphics context for the screen.
 */
Graphics::Graphics()
{
  m_HDC         = GetDC(NULL);
  m_autoRelease = true;
  m_autoDelete  = false;
  m_noPaint     = false;
  m_font        = Font::getDefault();
  m_penStyle    = PS_SOLID;
  m_penWidth    = 1;
}

/**
 * Creates a Graphics context related to the specified HDC.
 */
Graphics::Graphics(HDC hdc)
{
  assert(hdc != NULL);

  m_HDC         = hdc;
  m_autoRelease = false;
  m_autoDelete  = false;
  m_noPaint     = false;
  m_font        = NULL;
  m_penStyle    = PS_SOLID;
  m_penWidth    = 1;
}

Graphics::Graphics(HDC hdc, Image &image)
{
  assert(hdc != NULL);

  m_HDC         = CreateCompatibleDC(hdc);
  m_autoRelease = false;
  m_autoDelete  = true;
  m_noPaint     = false;
  m_font        = NULL;
  m_penStyle    = PS_SOLID;
  m_penWidth    = 1;
  
  SelectObject(m_HDC, image.getHBITMAP());
}

Graphics::Graphics(Widget *widget)
{
  HWND hwnd = widget->getHWND();

  assert(::IsWindow(hwnd));

  m_HDC         = GetDC(hwnd);
  m_autoRelease = true;
  m_autoDelete  = false;
  m_noPaint     = false;
  m_color       = widget->getFgColor();
  m_font        = widget->getFont();
  m_penStyle    = PS_SOLID;
  m_penWidth    = 1;
}

Graphics::~Graphics()
{
  if (m_autoRelease)
    ReleaseDC(NULL, m_HDC);
  else if (m_autoDelete)
    DeleteDC(m_HDC);
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
  assert(m_HDC != NULL);

  RECT rc;
  int res = ::GetClipBox(m_HDC, &rc);

  if (res == NULLREGION ||
      res == ERROR)
    return Rect();
  else
    return Rect(&rc);
}

void Graphics::getClipRegion(Region &rgn)
{
  assert(m_HDC != NULL);
  // TODO Region::assign(const Rect &rc)
  rgn = Region::fromRect(getClipBounds());
  GetClipRgn(m_HDC, rgn.getHRGN());
}

void Graphics::setClipRegion(Region &rgn)
{
  assert(m_HDC != NULL);
  ExtSelectClipRgn(m_HDC, rgn.getHRGN(), RGN_COPY);
}

void Graphics::excludeClipRect(const Rect &rc)
{
  assert(m_HDC != NULL);
  ExcludeClipRect(m_HDC, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

void Graphics::excludeClipRegion(Region &rgn)
{
  assert(m_HDC != NULL);
  ExtSelectClipRgn(m_HDC, rgn.getHRGN(), RGN_DIFF);
}

void Graphics::intersectClipRect(const Rect &rc)
{
  assert(m_HDC != NULL);
  IntersectClipRect(m_HDC, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

void Graphics::intersectClipRegion(Region &rgn)
{
  assert(m_HDC != NULL);
  ExtSelectClipRgn(m_HDC, rgn.getHRGN(), RGN_AND);
}

void Graphics::addClipRegion(Region &rgn)
{
  assert(m_HDC != NULL);
  ExtSelectClipRgn(m_HDC, rgn.getHRGN(), RGN_OR);
}

void Graphics::xorClipRegion(Region &rgn)
{
  assert(m_HDC != NULL);
  ExtSelectClipRgn(m_HDC, rgn.getHRGN(), RGN_XOR);
}

bool Graphics::isVisible(const Point &pt)
{
  assert(m_HDC != NULL);
  return PtVisible(m_HDC, pt.x, pt.y) != FALSE;
}

bool Graphics::isVisible(const Rect &rc)
{
  assert(m_HDC != NULL);
  RECT rc2 = rc;
  return RectVisible(m_HDC, &rc2) != FALSE;
}

// void Graphics::setPaintMode()
// {
//   assert(m_HDC != NULL);
//   SetROP2(m_HDC, R2_NOT);
// }

// void Graphics::setNotMode()
// {
//   assert(m_HDC != NULL);
//   SetROP2(m_HDC, R2_NOT);
// }

Color Graphics::getColor()
{
  return m_color;
}

void Graphics::setColor(const Color &color)
{
  m_color = color;
}

Font *Graphics::getFont()
{
  assert(m_font != NULL);
  return m_font;
}

void Graphics::setFont(Font *font)
{
  assert(font != NULL);
  m_font = font;
}

void Graphics::setPenStyle(int penStyle)
{
  m_penStyle = penStyle;
}

void Graphics::setPenWidth(int penWidth)
{
  m_penWidth = penWidth;
}

double Graphics::getMiterLimit()
{
  assert(m_HDC != NULL);

  float limit;

  if (GetMiterLimit(m_HDC, &limit) != FALSE)
    return limit;
  else
    return 10.0;
}

void Graphics::setMiterLimit(double limit)
{
  assert(m_HDC != NULL);

  SetMiterLimit(m_HDC, static_cast<FLOAT>(limit), NULL);
}

Color Graphics::getPixel(const Point &pt)
{
  assert(m_HDC != NULL);

  return Color(GetPixel(m_HDC, pt.x, pt.y));
}

Color Graphics::getPixel(int x, int y)
{
  assert(m_HDC != NULL);

  return Color(GetPixel(m_HDC, x, y));
}

void Graphics::setPixel(const Point &pt, const Color &color)
{
  assert(m_HDC != NULL);

  SetPixel(m_HDC, pt.x, pt.y, color.getColorRef());
}

void Graphics::setPixel(int x, int y, const Color &color)
{
  assert(m_HDC != NULL);

  SetPixel(m_HDC, x, y, color.getColorRef());
}

void Graphics::beginPath()
{
  assert(m_HDC != NULL);
  BeginPath(m_HDC);
}

void Graphics::endPath()
{
  assert(m_HDC != NULL);
  EndPath(m_HDC);
}

void Graphics::strokePath(Pen &pen)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen = SelectObject(m_HDC, pen.getHPEN());

  StrokePath(m_HDC);

  SelectObject(m_HDC, oldPen);
}

void Graphics::fillPath(Brush &brush)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, findHPEN(PS_NULL, 0, 0));
  HGDIOBJ oldBrush = SelectObject(m_HDC, brush.getHBRUSH());
//   int oldBkMode = SetBkMode(m_HDC, OPAQUE);
  int oldPolyFillMode = SetPolyFillMode(m_HDC, ALTERNATE); // TODO ALTERNATE or WINDING

  FillPath(m_HDC);

//   SetBkMode(m_HDC, oldBkMode);
  SetPolyFillMode(m_HDC, oldPolyFillMode);
  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::moveTo(const Point &pt)
{
  moveTo(pt.x, pt.y);
}

void Graphics::moveTo(int x, int y)
{
  assert(m_HDC != NULL);
  MoveToEx(m_HDC, x, y, NULL);
}

void Graphics::lineTo(Pen &pen, const Point &pt)
{
  lineTo(pen, pt.x, pt.y);
}

void Graphics::lineTo(Pen &pen, int x, int y)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen = SelectObject(m_HDC, pen.getHPEN());

  LineTo(m_HDC, x, y);

  SelectObject(m_HDC, oldPen);
}

void Graphics::curveTo(int x1, int y1, int x2, int y2, int x3, int y3)
{
  POINT pt[3];
  pt[0].x = x1;
  pt[0].y = y1;
  pt[1].x = x2;
  pt[1].y = y2;
  pt[2].x = x3;
  pt[2].y = y3;
  drawBezierTo(pt, 3);
}

void Graphics::curveTo(const Point &pt1, const Point &pt2, const Point &pt3)
{
  POINT pt[3];
  pt[0] = pt1;
  pt[1] = pt2;
  pt[2] = pt3;
  drawBezierTo(pt, 3);
}

void Graphics::curveTo(const std::vector<Point> &points)
{
  int numPoints = static_cast<int>(points.size());

  assert(numPoints >= 3);

  POINT *pt = new POINT[numPoints];
  std::copy(points.begin(), points.end(), pt);
  drawBezierTo(pt, numPoints);
  delete[] pt;
}

void Graphics::closeFigure()
{
  assert(m_HDC != NULL);
  CloseFigure(m_HDC);
}

void Graphics::getRegionFromPath(Region &region)
{
  assert(m_HDC != NULL);

  HRGN hrgn = PathToRegion(m_HDC);

  if (hrgn != NULL)
    region.assign(hrgn,
		  SelfDestruction(true));
}

void Graphics::drawString(const String &str, const Point &pt)
{
  drawString(str, pt.x, pt.y);
}

void Graphics::drawString(const String &str, int x, int y)
{
  assert(m_HDC != NULL);

  int oldMode = SetBkMode(m_HDC, TRANSPARENT);
  int oldColor = SetTextColor(m_HDC, m_color.getColorRef());

  if (m_font != NULL) {
    HGDIOBJ oldFont = SelectObject(m_HDC, reinterpret_cast<HGDIOBJ>(m_font->getHFONT()));
    TextOut(m_HDC, x, y, str.c_str(), static_cast<int>(str.size()));
    SelectObject(m_HDC, oldFont);
  }
  else
    TextOut(m_HDC, x, y, str.c_str(), static_cast<int>(str.size()));

  SetBkMode(m_HDC, oldMode); 
  SetTextColor(m_HDC, oldColor);
}

void Graphics::drawString(const String &str, const Rect &_rc, int flags)
{
  assert(m_HDC != NULL);

  int oldMode = SetBkMode(m_HDC, TRANSPARENT);
  int oldColor = SetTextColor(m_HDC, m_color.getColorRef());

  RECT rc = _rc;

  if (m_font != NULL) {
    HGDIOBJ oldFont = SelectObject(m_HDC, reinterpret_cast<HGDIOBJ>(m_font->getHFONT()));
    DrawText(m_HDC, str.c_str(), static_cast<int>(str.size()), &rc, flags);
    SelectObject(m_HDC, oldFont);
  }
  else
    DrawText(m_HDC, str.c_str(), static_cast<int>(str.size()), &rc, flags);

  SetBkMode(m_HDC, oldMode); 
  SetTextColor(m_HDC, oldColor);
}

void Graphics::drawDisabledString(const String &str, const Rect &rc, int flags)
{
  Color oldColor = m_color;

  setColor(System::getColor(COLOR_3DHIGHLIGHT));
  drawString(str, Rect(rc.x+1, rc.y+1, rc.w, rc.h), flags);

  setColor(System::getColor(COLOR_GRAYTEXT));
  drawString(str, rc, flags);

  m_color = oldColor;
}

void Graphics::drawImage(Image &image, int x, int y)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight());
}

void Graphics::drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height)
{
  assert(m_HDC != NULL);

  Graphics *source = image.getGraphics();

  assert(source->getHDC() != NULL);
  
  BitBlt(m_HDC, dstX, dstY, width, height, source->getHDC(), srcX, srcY, SRCCOPY);
}

void Graphics::drawImage(Image &image, int x, int y, const Color &bgColor)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight(), bgColor);
}

void Graphics::drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height, const Color &bgColor)
{
  assert(m_HDC != NULL);

  Graphics *source = image.getGraphics();

  assert(source->getHDC() != NULL);

#if 0				// WinCE
  TransparentImage(m_HDC, dstX, dstY, width, height,
		   source->getHDC(), srcX, srcY, width, height,
		   bgColor.getColorRef());
#else
    
  HDC maskHDC = CreateCompatibleDC(m_HDC);
  HBITMAP theMask = CreateBitmap(width, height, 1, 1, NULL);
  HGDIOBJ oldMask = SelectObject(maskHDC, theMask);
  COLORREF oldBkColor = SetBkColor(source->getHDC(), bgColor.getColorRef());
  
  BitBlt(maskHDC, 0, 0, width, height,
	 source->getHDC(), srcX, srcY, SRCCOPY);
  
  MaskBlt(m_HDC, dstX, dstY, width, height,
	  source->getHDC(), srcX, srcY,
	  theMask, 0, 0, MAKEROP4(0x00AA0029, SRCCOPY)); // 0x00AA0029 is NOP

  SetBkColor(source->getHDC(), oldBkColor);
  DeleteObject(SelectObject(maskHDC, oldMask));
  DeleteObject(maskHDC);

#endif
}

void Graphics::drawImage(Image &image, const Point &pt)
{
  drawImage(image, pt.x, pt.y);
}

void Graphics::drawImage(Image &image, const Point &pt, const Rect &rc)
{
  drawImage(image, pt.x, pt.y, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawImage(Image &image, const Point &pt, const Color &bgColor)
{
  drawImage(image, pt.x, pt.y, bgColor);
}

void Graphics::drawImage(Image &image, const Point &pt, const Rect &rc, const Color &bgColor)
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
void Graphics::drawImageList(ImageList &imageList, int imageIndex, int x, int y, int style)
{
  assert(m_HDC != NULL);
  assert(imageList.isValid());

  ImageList_Draw(imageList.getHIMAGELIST(),
		 imageIndex, m_HDC, x, y, style);
}

void Graphics::drawImageList(ImageList &imageList, int imageIndex, const Point &pt, int style)
{
  drawImageList(imageList, imageIndex, pt.x, pt.y, style);
}

void Graphics::drawLine(Pen &pen, const Point &pt1, const Point &pt2)
{
  drawLine(pen, pt1.x, pt1.y, pt2.x, pt2.y);
}

void Graphics::drawLine(Pen &pen, int x1, int y1, int x2, int y2)
{
  assert(m_HDC != NULL);

  POINT oldPos;
  HGDIOBJ oldPen = SelectObject(m_HDC, pen.getHPEN());

  MoveToEx(m_HDC, x1, y1, &oldPos);
  LineTo(m_HDC, x2, y2);
  MoveToEx(m_HDC, oldPos.x, oldPos.y, NULL);

  SelectObject(m_HDC, oldPen);
}

void Graphics::drawBezier(Pen &pen, const Point points[4])
{
  POINT pt[4];
  std::copy(points, points+4, pt);
  drawBezier(pen, pt, 4);
}

void Graphics::drawBezier(Pen &pen, const std::vector<Point> &points)
{
  int numPoints = static_cast<int>(points.size());

  assert(numPoints >= 4);

  POINT *pt = new POINT[numPoints];
  std::copy(points.begin(), points.end(), pt);
  drawBezier(pen, pt, numPoints);
  delete[] pt;
}

void Graphics::drawBezier(Pen &pen, const Point &pt1, const Point &pt2, const Point &pt3, const Point &pt4)
{
  POINT pt[4];
  pt[0] = pt1;
  pt[1] = pt2;
  pt[2] = pt3;
  pt[3] = pt4;
  drawBezier(pen, pt, 4);
}

void Graphics::drawBezier(Pen &pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
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

void Graphics::drawRect(Pen &pen, const Rect &rc)
{
  drawRect(pen, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawRect(Pen &pen, int x, int y, int w, int h)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, pen.getHPEN());
  HGDIOBJ oldBrush = SelectObject(m_HDC, findHBRUSH(BS_NULL, 0, 0));

  Rectangle(m_HDC, x, y, x+w, y+h);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::drawRoundRect(Pen &pen, const Rect &rc, const Size &ellipse)
{
  drawRoundRect(pen, rc.x, rc.y, rc.w, rc.h, ellipse.w, ellipse.h);
}

void Graphics::drawRoundRect(Pen &pen, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, pen.getHPEN());
  HGDIOBJ oldBrush = SelectObject(m_HDC, findHBRUSH(BS_NULL, 0, 0));

  RoundRect(m_HDC, x, y, x+w, y+h, ellipseWidth, ellipseHeight);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::draw3dRect(const Rect &rc, const Color &topLeft, const Color &bottomRight)
{
  draw3dRect(rc.x, rc.y, rc.w, rc.h, topLeft, bottomRight);
}

void Graphics::draw3dRect(int x, int y, int w, int h, const Color &topLeft, const Color &bottomRight)
{
  assert(m_HDC != NULL);

  POINT oldPos;
  HGDIOBJ oldPen = SelectObject(m_HDC, findHPEN(m_penStyle, m_penWidth, topLeft.getColorRef()));
  MoveToEx(m_HDC, x, y+h-2, &oldPos);
  LineTo(m_HDC, x, y);
  LineTo(m_HDC, x+w-1, y);

  SelectObject(m_HDC, findHPEN(m_penStyle, m_penWidth, bottomRight.getColorRef()));
  LineTo(m_HDC, x+w-1, y+h-1);
  LineTo(m_HDC, x-1, y+h-1);

  SelectObject(m_HDC, oldPen);
  MoveToEx(m_HDC, oldPos.x, oldPos.y, NULL);
}

/**
 * Draws the outline of an ellipse. It uses the current selected color
 * (see setColor), and doesn't paint the background.
 */
void Graphics::drawEllipse(Pen &pen, const Rect &rc)
{
  drawEllipse(pen, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawEllipse(Pen &pen, int x, int y, int w, int h)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, pen.getHPEN());
  HGDIOBJ oldBrush = SelectObject(m_HDC, findHBRUSH(BS_NULL, 0, 0));

  Ellipse(m_HDC, x, y, x+w, y+h);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

/**
 * Draws an arc with rc as a bounding rectangle for the ellipse that
 * encloses the arc. The arc start in the startAngle (a value between
 * -360 and 360), and as a arc length of sweepAngle (in
 * counter-clockwise).
 */
void Graphics::drawArc(Pen &pen, const Rect &rc, double startAngle, double sweepAngle)
{
  drawArc(pen, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawArc(Pen &pen, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen = SelectObject(m_HDC, pen.getHPEN());
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Arc(m_HDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_HDC, oldPen);
}

void Graphics::drawPie(Pen &pen, const Rect &rc, double startAngle, double sweepAngle)
{
  drawPie(pen, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawPie(Pen &pen, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, pen.getHPEN());
  HGDIOBJ oldBrush = SelectObject(m_HDC, findHBRUSH(BS_NULL, 0, 0));
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(m_HDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::drawChord(Pen &pen, const Rect &rc, double startAngle, double sweepAngle)
{
  drawChord(pen, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawChord(Pen &pen, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, pen.getHPEN());
  HGDIOBJ oldBrush = SelectObject(m_HDC, findHBRUSH(BS_NULL, 0, 0));
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(m_HDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::drawPolyline(Pen &pen, const std::vector<Point> &points)
{
  int numPoints = static_cast<int>(points.size());

  assert(numPoints >= 2);

  POINT *pt = new POINT[numPoints];
  std::copy(points.begin(), points.end(), pt);
  drawPolyline(pen, pt, numPoints);
  delete[] pt;
}

void Graphics::fillRect(Brush &brush, const Rect &rc)
{
  fillRect(brush, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillRect(Brush &brush, int x, int y, int w, int h)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, findHPEN(PS_NULL, 0, 0));
  HGDIOBJ oldBrush = SelectObject(m_HDC, brush.getHBRUSH());

  Rectangle(m_HDC, x, y, x+w+1, y+h+1);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::fillRoundRect(Brush &brush, const Rect &rc, const Size &ellipse)
{
  fillRoundRect(brush, rc.x, rc.y, rc.w, rc.h, ellipse.w, ellipse.h);
}

void Graphics::fillRoundRect(Brush &brush, int x, int y, int w, int h, int ellipseWidth, int ellipseHeight)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, findHPEN(PS_NULL, 0, 0));
  HGDIOBJ oldBrush = SelectObject(m_HDC, brush.getHBRUSH());

  RoundRect(m_HDC, x, y, x+w, y+h, ellipseWidth, ellipseHeight);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::fillEllipse(Brush &brush, const Rect &rc)
{
  fillEllipse(brush, rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillEllipse(Brush &brush, int x, int y, int w, int h)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, findHPEN(PS_NULL, 0, 0));
  HGDIOBJ oldBrush = SelectObject(m_HDC, brush.getHBRUSH());

  Ellipse(m_HDC, x, y, x+w+1, y+h+1);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::fillPie(Brush &brush, const Rect &rc, double startAngle, double sweepAngle)
{
  fillPie(brush, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillPie(Brush &brush, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, findHPEN(PS_NULL, 0, 0));
  HGDIOBJ oldBrush = SelectObject(m_HDC, brush.getHBRUSH());
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(m_HDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::fillChord(Brush &brush, const Rect &rc, double startAngle, double sweepAngle)
{
  fillChord(brush, rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillChord(Brush &brush, int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen   = SelectObject(m_HDC, findHPEN(PS_NULL, 0, 0));
  HGDIOBJ oldBrush = SelectObject(m_HDC, brush.getHBRUSH());
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(m_HDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(m_HDC, oldPen);
  SelectObject(m_HDC, oldBrush);
}

void Graphics::fillRegion(Brush &brush, const Region &rgn)
{
  assert(m_HDC != NULL);

  FillRgn(m_HDC,
	  const_cast<Region *>(&rgn)->getHRGN(),
	  brush.getHBRUSH());
}

void Graphics::fillGradientRect(const Rect &rc, const Color &startColor, const Color &endColor,
				Orientation orientation)
{
  fillGradientRect(rc.x, rc.y, rc.w, rc.h, startColor, endColor, orientation);
}

typedef BOOL (WINAPI * GFProc)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

void Graphics::fillGradientRect(int x, int y, int w, int h,
				const Color &startColor,
				const Color &endColor,
				Orientation orientation)
{
  assert(m_HDC != NULL);
  
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
  GradientFill(m_HDC, vert, 2, &gRect, 1,
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
    pGF(m_HDC, vert, 2, &gRect, 1,
	orientation == Horizontal ? GRADIENT_FILL_RECT_H:
				    GRADIENT_FILL_RECT_V);
  }
#endif
}

void Graphics::drawGradientRect(const Rect &rc,
				const Color &topLeft, const Color &topRight,
				const Color &bottomLeft, const Color &bottomRight)
{
  drawGradientRect(rc.x, rc.y, rc.w, rc.h, topLeft, topRight, bottomLeft, bottomRight);
}

void Graphics::drawGradientRect(int x, int y, int w, int h,
				const Color &topLeft, const Color &topRight,
				const Color &bottomLeft, const Color &bottomRight)
{
  fillGradientRect(x,     y,     w, 1, topLeft,    topRight,    Orientation::Horizontal);
  fillGradientRect(x,     y,     1, h, topLeft,    bottomLeft,  Orientation::Vertical);
  fillGradientRect(x,     y+h-1, w, 1, bottomLeft, bottomRight, Orientation::Horizontal);
  fillGradientRect(x+w-1, y,     1, h, topRight,   bottomRight, Orientation::Vertical);
}

void Graphics::drawXorFrame(const Rect &rc, int border)
{
  drawXorFrame(rc.x, rc.y, rc.w, rc.h, border);
}

void Graphics::drawXorFrame(int x, int y, int w, int h, int border)
{
  static WORD pattern[] = { 0x00aa, 0x0055, 0x00aa, 0x0055,
			    0x00aa, 0x0055, 0x00aa, 0x0055 };

  HBITMAP hbitmap = CreateBitmap(8, 8, 1, 1, pattern);
  HBRUSH newBrush = CreatePatternBrush(hbitmap);
  HANDLE oldBrush = SelectObject(m_HDC, newBrush);

  PatBlt(m_HDC, x+border,   y,          w-border,  border,   PATINVERT);
  PatBlt(m_HDC, x+w-border, y+border,   border,    h-border, PATINVERT);
  PatBlt(m_HDC, x,          y+h-border, w-border,  border,   PATINVERT);
  PatBlt(m_HDC, x,          y,          border,    h-border, PATINVERT);

  DeleteObject(SelectObject(m_HDC, oldBrush));
  DeleteObject(hbitmap);
}

void Graphics::drawFocus(const Rect &rc)
{
  assert(m_HDC != NULL);

  RECT _rc = rc;
  ::DrawFocusRect(m_HDC, &_rc);
}

Size Graphics::measureString(const String &str, int fitInWidth, int flags)
{
  assert(m_HDC != NULL);

  RECT rc = Rect(0, 0, fitInWidth, 0);
  HGDIOBJ oldFont;

  if (m_font != NULL)
    oldFont = SelectObject(m_HDC, reinterpret_cast<HGDIOBJ>(m_font->getHFONT()));

  if (!str.empty()) {
    DrawText(m_HDC, str.c_str(), static_cast<int>(str.size()), &rc, flags | DT_CALCRECT);
  }
  else {
    SIZE sz;

    if (GetTextExtentPoint32(m_HDC, _T(" "), 1, &sz)) {
      rc.right = sz.cx;
      rc.bottom = sz.cy;
    }
  }

  if (m_font != NULL)
    SelectObject(m_HDC, oldFont);

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
  assert(m_HDC != NULL);

  SetROP2(m_HDC, drawMode);
}

HDC Graphics::getHDC()
{
  return m_HDC;
}

/**
 * @param style PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT, PS_NULL, PS_INSIDEFRAME
 * 
 */
HPEN Graphics::findHPEN(int style, int width, COLORREF color)
{
  for (std::list<_Pen *>::iterator it = mPens.begin();
       it != mPens.end();
       ++it) {
    _Pen *pen = *it;

    if (pen->style == style &&
	pen->width == width &&
	pen->color == color)
      return pen->handle;
  }

  _Pen *pen = new _Pen(style, width, color);
  mPens.push_back(pen);
  return pen->handle;
}

HBRUSH Graphics::findHBRUSH(int style, int hatch, COLORREF color)
{
  for (std::list<_Brush *>::iterator it = mBrushes.begin();
       it != mBrushes.end();
       ++it) {
    _Brush *brush = *it;

    if (brush->style == style &&
	brush->hatch == hatch &&
	brush->color == color)
      return brush->handle;
  }

  _Brush *brush = new _Brush(style, hatch, color);
  mBrushes.push_back(brush);
  return brush->handle;
}

void Graphics::deleteHandles()
{
  for (std::list<_Pen *>::iterator it = mPens.begin();
       it != mPens.end();
       ++it)
    delete (*it);

  for (std::list<_Brush *>::iterator it = mBrushes.begin();
       it != mBrushes.end();
       ++it)
    delete (*it);

  mPens.clear();
  mBrushes.clear();
}

void Graphics::drawBezier(Pen &pen, CONST POINT *lppt, int numPoints)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen = SelectObject(m_HDC, pen.getHPEN());

  PolyBezier(m_HDC, lppt, numPoints);

  SelectObject(m_HDC, oldPen);
}

void Graphics::drawBezierTo(CONST POINT *lppt, int numPoints)
{
  assert(m_HDC != NULL);

  HGDIOBJ thePen = findHPEN(m_penStyle, m_penWidth, m_color.getColorRef());
  HGDIOBJ oldPen = SelectObject(m_HDC, thePen);

  PolyBezierTo(m_HDC, lppt, numPoints);

  SelectObject(m_HDC, oldPen);
}

void Graphics::drawPolyline(Pen &pen, CONST POINT *lppt, int numPoints)
{
  assert(m_HDC != NULL);

  HGDIOBJ oldPen = SelectObject(m_HDC, pen.getHPEN());

  Polyline(m_HDC, lppt, numPoints);

  SelectObject(m_HDC, oldPen);
}

//////////////////////////////////////////////////////////////////////
// ScreenGraphics

ScreenGraphics::ScreenGraphics()
  : Graphics()
{
}

ScreenGraphics::~ScreenGraphics()
{
}
