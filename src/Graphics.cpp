// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

using namespace Vaca;

std::list<Graphics::Pen *> Graphics::mPens;
std::list<Graphics::Brush *> Graphics::mBrushes;

//////////////////////////////////////////////////////////////////////
// Graphics::Pen

Graphics::Pen::Pen(int style, int width, COLORREF color)
  : style(style)
  , width(width)
  , color(color)
{
  handle = CreatePen(style, width, color);
}

Graphics::Pen::~Pen()
{
  DeleteObject(reinterpret_cast<HGDIOBJ>(handle));
}

//////////////////////////////////////////////////////////////////////
// Graphics::Brush

Graphics::Brush::Brush(int style, int hatch, COLORREF color)
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

Graphics::Brush::~Brush()
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
  mHDC         = GetDC(NULL);
  mAutoRelease = true;
  mAutoDelete  = false;
  mNoPaint     = false;
  mFont        = &Font::getDefault();
  mPenSize     = 1;
}

/**
 * Creates a Graphics context related to the specified HDC.
 */
Graphics::Graphics(HDC hdc)
{
  assert(hdc != NULL);

  mHDC         = hdc;
  mAutoRelease = false;
  mAutoDelete  = false;
  mNoPaint     = false;
  mFont        = NULL;
  mPenSize     = 1;
}

Graphics::Graphics(HDC hdc, Image &image)
{
  assert(hdc != NULL);

  mHDC         = CreateCompatibleDC(hdc);
  mAutoRelease = false;
  mAutoDelete  = true;
  mNoPaint     = false;
  mFont        = NULL;
  mPenSize     = 1;
  
  SelectObject(mHDC, image.getHBITMAP());
}

/**
 * Creates a Graphics context for the specified Widget.
 */
// Graphics::Graphics(Widget *widget)
// {
//   HWND hwnd = widget->getHWND();

//   assert(hwnd != NULL);

//   mHDC     = GetDC(hwnd);
//   mRelease = true;
//   mDelete  = false;
//   mNoPaint = false;
//   mFont    = NULL;
// }

Graphics::~Graphics()
{
  if (mAutoRelease)
    ReleaseDC(NULL, mHDC);
  else if (mAutoDelete)
    DeleteDC(mHDC);
}

/**
 * Indicates that the Graphics wasn't touched. It's called by the
 * default implementation of the Widget::onPaint.
 */
void Graphics::noPaint()
{
  mNoPaint = true;
}

bool Graphics::wasPainted()
{
  return !mNoPaint;
}

Rect Graphics::getClipBounds()
{
  assert(mHDC != NULL);

  RECT rc;
  int res = ::GetClipBox(mHDC, &rc);

  if (res == NULLREGION ||
      res == ERROR)
    return Rect();
  else
    return Rect(&rc);
}

void Graphics::excludeClip(const Rect &rc)
{
  assert(mHDC != NULL);
  ExcludeClipRect(mHDC, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

void Graphics::intersectClip(const Rect &rc)
{
  assert(mHDC != NULL);
  IntersectClipRect(mHDC, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

bool Graphics::isVisible(const Point &pt)
{
  assert(mHDC != NULL);
  return PtVisible(mHDC, pt.x, pt.y) != FALSE;
}

bool Graphics::isVisible(const Rect &rc)
{
  assert(mHDC != NULL);
  RECT rc2 = rc;
  return RectVisible(mHDC, &rc2) != FALSE;
}

// void Graphics::setPaintMode()
// {
//   assert(mHDC != NULL);
//   SetROP2(mHDC, R2_NOT);
// }

// void Graphics::setNotMode()
// {
//   assert(mHDC != NULL);
//   SetROP2(mHDC, R2_NOT);
// }

Color Graphics::getColor()
{
  return mColor;
}

void Graphics::setColor(const Color &color)
{
  mColor = color;
}

Font &Graphics::getFont()
{
  return *mFont;
}

void Graphics::setFont(Font &font)
{
  mFont = &font;
}

int Graphics::getPenSize()
{
  return mPenSize;
}

void Graphics::setPenSize(int penSize)
{
  mPenSize = penSize;
}

Color Graphics::getPixel(const Point &pt)
{
  assert(mHDC != NULL);

  return Color(GetPixel(mHDC, pt.x, pt.y));
}

Color Graphics::getPixel(int x, int y)
{
  assert(mHDC != NULL);

  return Color(GetPixel(mHDC, x, y));
}

void Graphics::setPixel(const Point &pt, Color color)
{
  assert(mHDC != NULL);

  SetPixel(mHDC, pt.x, pt.y, color.getColorRef());
}

void Graphics::setPixel(int x, int y, Color color)
{
  assert(mHDC != NULL);

  SetPixel(mHDC, x, y, color.getColorRef());
}

void Graphics::moveTo(const Point &pt)
{
  moveTo(pt.x, pt.y);
}

void Graphics::lineTo(const Point &pt)
{
  lineTo(pt.x, pt.y);
}

void Graphics::moveTo(int x, int y)
{
  assert(mHDC != NULL);
  MoveToEx(mHDC, x, y, NULL);
}

void Graphics::lineTo(int x, int y)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ oldPen = SelectObject(mHDC, thePen);

  LineTo(mHDC, x, y);

  SelectObject(mHDC, oldPen);
}

void Graphics::beginPath()
{
  assert(mHDC != NULL);
  BeginPath(mHDC);
}

void Graphics::endPath()
{
  assert(mHDC != NULL);
  EndPath(mHDC);
}

void Graphics::strokePath()
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ theBrush = findHBRUSH(BS_NULL, 0, 0);
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);
//   int oldMode = SetBkMode(mHDC, OPAQUE);

  StrokePath(mHDC);

//   SetBkMode(mHDC, oldMode);
  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::fillPath()
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_NULL, 0, 0);
  HGDIOBJ theBrush = findHBRUSH(BS_SOLID, 0, mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);
//   int oldBkMode = SetBkMode(mHDC, OPAQUE);
  int oldPolyFillMode = SetPolyFillMode(mHDC, ALTERNATE);

  FillPath(mHDC);

//   SetBkMode(mHDC, oldBkMode);
  SetPolyFillMode(mHDC, oldPolyFillMode);
  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::drawString(const String &str, const Point &pt)
{
  drawString(str, pt.x, pt.y);
}

void Graphics::drawString(const String &str, int x, int y)
{
  assert(mHDC != NULL);

  int oldMode = SetBkMode(mHDC, TRANSPARENT);
  int oldColor = SetTextColor(mHDC, mColor.getColorRef());

  if (mFont != NULL) {
    HGDIOBJ oldFont = SelectObject(mHDC, reinterpret_cast<HGDIOBJ>(mFont->getHFONT()));
    TextOut(mHDC, x, y, str.c_str(), str.size());
    SelectObject(mHDC, oldFont);
  }
  else
    TextOut(mHDC, x, y, str.c_str(), str.size());

  SetBkMode(mHDC, oldMode); 
  SetTextColor(mHDC, oldColor);
}

void Graphics::drawString(const String &str, const Rect &_rc, int flags)
{
  assert(mHDC != NULL);

  int oldMode = SetBkMode(mHDC, TRANSPARENT);
  int oldColor = SetTextColor(mHDC, mColor.getColorRef());

  RECT rc = _rc;

  if (mFont != NULL) {
    HGDIOBJ oldFont = SelectObject(mHDC, reinterpret_cast<HGDIOBJ>(mFont->getHFONT()));
    DrawText(mHDC, str.c_str(), str.size(), &rc, flags);
    SelectObject(mHDC, oldFont);
  }
  else
    DrawText(mHDC, str.c_str(), str.size(), &rc, flags);

  SetBkMode(mHDC, oldMode); 
  SetTextColor(mHDC, oldColor);
}

void Graphics::drawDisabledString(const String &str, const Rect &rc, int flags)
{
  Color oldColor = mColor;

  setColor(System::getColor(COLOR_3DHIGHLIGHT));
  drawString(str, Rect(rc.x+1, rc.y+1, rc.w, rc.h), flags);

  setColor(System::getColor(COLOR_GRAYTEXT));
  drawString(str, rc, flags);

  mColor = oldColor;
}

void Graphics::drawImage(Image &image, int x, int y)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight());
}

void Graphics::drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height)
{
  assert(mHDC != NULL);

  Graphics &source = image.getGraphics();

  assert(source.getHDC() != NULL);
  
  BitBlt(mHDC, dstX, dstY, width, height, source.getHDC(), srcX, srcY, SRCCOPY);
}

void Graphics::drawImage(Image &image, int x, int y, Color bgColor)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight(), bgColor);
}

void Graphics::drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height, Color bgColor)
{
  assert(mHDC != NULL);

  Graphics &source = image.getGraphics();

  assert(source.getHDC() != NULL);

#if 0				// WinCE
  TransparentImage(mHDC, dstX, dstY, width, height,
		   source.getHDC(), srcX, srcY, width, height,
		   bgColor.getColorRef());
#else
    
  HDC maskHDC = CreateCompatibleDC(mHDC);
  HBITMAP theMask = CreateBitmap(width, height, 1, 1, NULL);
  HGDIOBJ oldMask = SelectObject(maskHDC, theMask);
  COLORREF oldBkColor = SetBkColor(source.getHDC(), bgColor.getColorRef());
  
  BitBlt(maskHDC, 0, 0, width, height,
	 source.getHDC(), srcX, srcY, SRCCOPY);
  
  MaskBlt(mHDC, dstX, dstY, width, height,
	  source.getHDC(), srcX, srcY,
	  theMask, 0, 0, MAKEROP4(0x00AA0029, SRCCOPY)); // 0x00AA0029 is NOP

  SetBkColor(source.getHDC(), oldBkColor);
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

void Graphics::drawImage(Image &image, const Point &pt, Color bgColor)
{
  drawImage(image, pt.x, pt.y, bgColor);
}

void Graphics::drawImage(Image &image, const Point &pt, const Rect &rc, Color bgColor)
{
  drawImage(image, pt.x, pt.y, rc.x, rc.y, rc.w, rc.h, bgColor);
}

void Graphics::drawImageList(ImageList &imageList, int imageIndex, int x, int y, int style)
{
  assert(mHDC != NULL);
  assert(imageList.isValid());

  ImageList_Draw(imageList.getHIMAGELIST(),
		 imageIndex, mHDC, x, y, style);
}

void Graphics::drawImageList(ImageList &imageList, int imageIndex, const Point &pt, int style)
{
  drawImageList(imageList, imageIndex, pt.x, pt.y, style);
}

void Graphics::drawLine(const Point &pt1, const Point &pt2)
{
  drawLine(pt1.x, pt1.y, pt2.x, pt2.y);
}

void Graphics::drawLine(int x1, int y1, int x2, int y2)
{
  assert(mHDC != NULL);

  POINT oldPos;
  HGDIOBJ thePen = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ oldPen = SelectObject(mHDC, thePen);

  MoveToEx(mHDC, x1, y1, &oldPos);
  LineTo(mHDC, x2, y2);
  MoveToEx(mHDC, oldPos.x, oldPos.y, NULL);

  SelectObject(mHDC, oldPen);
}

void Graphics::drawRect(const Rect &rc)
{
  drawRect(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawRect(int x, int y, int w, int h)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ theBrush = findHBRUSH(BS_NULL, 0, 0);
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);

  Rectangle(mHDC, x, y, x+w, y+h);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::draw3dRect(const Rect &rc, Color topLeft, Color bottomRight)
{
  draw3dRect(rc.x, rc.y, rc.w, rc.h, topLeft, bottomRight);
}

void Graphics::draw3dRect(int x, int y, int w, int h, Color topLeft, Color bottomRight)
{
  assert(mHDC != NULL);

  POINT oldPos;
  HGDIOBJ oldPen = SelectObject(mHDC, findHPEN(PS_SOLID, mPenSize, topLeft.getColorRef()));
  MoveToEx(mHDC, x, y+h-2, &oldPos);
  LineTo(mHDC, x, y);
  LineTo(mHDC, x+w-1, y);

  SelectObject(mHDC, findHPEN(PS_SOLID, mPenSize, bottomRight.getColorRef()));
  LineTo(mHDC, x+w-1, y+h-1);
  LineTo(mHDC, x-1, y+h-1);

  SelectObject(mHDC, oldPen);
  MoveToEx(mHDC, oldPos.x, oldPos.y, NULL);
}

/**
 * Draws the outline of an ellipse. It uses the current selected color
 * (see setColor), and doesn't paint the background.
 */
void Graphics::drawEllipse(const Rect &rc)
{
  drawEllipse(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawEllipse(int x, int y, int w, int h)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ theBrush = findHBRUSH(BS_NULL, 0, 0);
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);

  Ellipse(mHDC, x, y, x+w, y+h);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

/**
 * Draws an arc with rc as a bounding rectangle for the ellipse that
 * encloses the arc. The arc start in the startAngle (a value between
 * -360 and 360), and as a arc length of sweepAngle (in
 * counter-clockwise).
 */
void Graphics::drawArc(const Rect &rc, int startAngle, int sweepAngle)
{
  drawArc(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawArc(int x, int y, int w, int h, int startAngle, int sweepAngle)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ oldPen = SelectObject(mHDC, thePen);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Arc(mHDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(mHDC, oldPen);
}

void Graphics::drawPie(const Rect &rc, double startAngle, double sweepAngle)
{
  drawPie(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawPie(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ theBrush = findHBRUSH(BS_NULL, 0, 0);
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(mHDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::drawChord(const Rect &rc, double startAngle, double sweepAngle)
{
  drawChord(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawChord(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ theBrush = findHBRUSH(BS_NULL, 0, 0);
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(mHDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::fillRect(const Rect &rc)
{
  fillRect(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillRect(int x, int y, int w, int h)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_NULL, 0, 0);
  HGDIOBJ theBrush = findHBRUSH(BS_SOLID, 0, mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);

  Rectangle(mHDC, x, y, x+w+1, y+h+1);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::fillEllipse(const Rect &rc)
{
  fillEllipse(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillEllipse(int x, int y, int w, int h)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_NULL, 0, 0);
  HGDIOBJ theBrush = findHBRUSH(BS_SOLID, 0, mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);

  Ellipse(mHDC, x, y, x+w+1, y+h+1);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::fillPie(const Rect &rc, double startAngle, double sweepAngle)
{
  fillPie(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillPie(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_NULL, 0, 0);
  HGDIOBJ theBrush = findHBRUSH(BS_SOLID, 0, mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(mHDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::fillChord(const Rect &rc, double startAngle, double sweepAngle)
{
  fillChord(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillChord(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  assert(mHDC != NULL);

  HGDIOBJ thePen   = findHPEN(PS_NULL, 0, 0);
  HGDIOBJ theBrush = findHBRUSH(BS_SOLID, 0, mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHDC, thePen);
  HGDIOBJ oldBrush = SelectObject(mHDC, theBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(mHDC, x, y, x+w, y+h, x1, y1, x2, y2);

  SelectObject(mHDC, oldPen);
  SelectObject(mHDC, oldBrush);
}

void Graphics::fillRegion(const Region &rgn)
{
  assert(mHDC != NULL);

  HGDIOBJ theBrush = findHBRUSH(BS_SOLID, 0, mColor.getColorRef());
      
  FillRgn(mHDC,
	  const_cast<Region *>(&rgn)->getHRGN(),
	  reinterpret_cast<HBRUSH>(theBrush));
}

void Graphics::fillGradientRect(const Rect &rc, Color startColor, Color endColor,
				Orientation orientation)
{
  fillGradientRect(rc.x, rc.y, rc.w, rc.h, startColor, endColor, orientation);
}

typedef BOOL (WINAPI * GFProc)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

void Graphics::fillGradientRect(int x, int y, int w, int h,
				Color startColor, Color endColor,
				Orientation orientation)
{
  assert(mHDC != NULL);
  
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
  GradientFill(mHDC, vert, 2, &gRect, 1,
	       orientation == Horizontal ? GRADIENT_FILL_RECT_H:
					   GRADIENT_FILL_RECT_V);
#else
  #define GRADIENT_FILL_RECT_H 0
  #define GRADIENT_FILL_RECT_V 1

  static GFProc pGF = NULL;

  if (pGF == NULL) {
    HMODULE hMsImg32 = LoadLibrary(_T("msimg32.dll"));
    if (hMsImg32 != NULL)
      pGF = (GFProc)GetProcAddress(hMsImg32, "GradientFill");
  }

  if (pGF != NULL) {
    pGF(mHDC, vert, 2, &gRect, 1,
	orientation == Horizontal ? GRADIENT_FILL_RECT_H:
				    GRADIENT_FILL_RECT_V);
  }
#endif
}

void Graphics::drawGradientRect(const Rect &rc, Color topLeft, Color topRight, Color bottomLeft, Color bottomRight)
{
  drawGradientRect(rc.x, rc.y, rc.w, rc.h, topLeft, topRight, bottomLeft, bottomRight);
}

void Graphics::drawGradientRect(int x, int y, int w, int h, Color topLeft, Color topRight, Color bottomLeft, Color bottomRight)
{
  fillGradientRect(x,     y,     w, 1, topLeft,    topRight,    Horizontal);
  fillGradientRect(x,     y,     1, h, topLeft,    bottomLeft,  Vertical);
  fillGradientRect(x,     y+h-1, w, 1, bottomLeft, bottomRight, Horizontal);
  fillGradientRect(x+w-1, y,     1, h, topRight,   bottomRight, Vertical);
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
  HANDLE oldBrush = SelectObject(mHDC, newBrush);

  PatBlt(mHDC, x+border,   y,          w-border,  border,   PATINVERT);
  PatBlt(mHDC, x+w-border, y+border,   border,    h-border, PATINVERT);
  PatBlt(mHDC, x,          y+h-border, w-border,  border,   PATINVERT);
  PatBlt(mHDC, x,          y,          border,    h-border, PATINVERT);

  DeleteObject(SelectObject(mHDC, oldBrush));
  DeleteObject(hbitmap);
}

Size Graphics::measureString(const String &str, int fitInWidth, int flags)
{
  assert(mHDC != NULL);

  RECT rc = Rect(0, 0, fitInWidth, 0);
  HGDIOBJ oldFont;

  if (mFont != NULL)
    oldFont = SelectObject(mHDC, reinterpret_cast<HGDIOBJ>(mFont->getHFONT()));

  if (!str.empty()) {
    DrawText(mHDC, str.c_str(), str.size(), &rc, flags | DT_CALCRECT);
  }
  else {
    SIZE sz;

    if (GetTextExtentPoint32(mHDC, _T(" "), 1, &sz)) {
      rc.right = sz.cx;
      rc.bottom = sz.cy;
    }
  }

  if (mFont != NULL)
    SelectObject(mHDC, oldFont);

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
  assert(mHDC != NULL);

  SetROP2(mHDC, drawMode);
}

HDC Graphics::getHDC()
{
  return mHDC;
}

HPEN Graphics::findHPEN(int style, int width, COLORREF color)
{
  for (std::list<Pen *>::iterator it = mPens.begin();
       it != mPens.end();
       ++it) {
    Pen *pen = *it;

    if (pen->style == style &&
	pen->width == width &&
	pen->color == color)
      return pen->handle;
  }

  Pen *pen = new Pen(style, width, color);
  mPens.push_back(pen);
  return pen->handle;
}

HBRUSH Graphics::findHBRUSH(int style, int hatch, COLORREF color)
{
  for (std::list<Brush *>::iterator it = mBrushes.begin();
       it != mBrushes.end();
       ++it) {
    Brush *brush = *it;

    if (brush->style == style &&
	brush->hatch == hatch &&
	brush->color == color)
      return brush->handle;
  }

  Brush *brush = new Brush(style, hatch, color);
  mBrushes.push_back(brush);
  return brush->handle;
}

void Graphics::deleteHandles()
{
  for (std::list<Pen *>::iterator it = mPens.begin();
       it != mPens.end();
       ++it)
    delete (*it);

  for (std::list<Brush *>::iterator it = mBrushes.begin();
       it != mBrushes.end();
       ++it)
    delete (*it);
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
