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
#include "Vaca/Debug.h"
#include "Vaca/Font.h"
#include "Vaca/Rect.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"
#include "Vaca/Widget.h"

using namespace Vaca;

ScreenGraphics::ScreenGraphics()
  : Graphics()
{
}

/**
 * Creates a Graphics context for the screen.
 */
Graphics::Graphics()
{
  mHdc     = GetDC(NULL);
  mRelease = true;
  mDelete  = false;
  mNoPaint = false;
  mFont    = &Font::getDefault();
  mPenSize = 1;
}

/**
 * Creates a Graphics context related to the specified HDC.
 */
Graphics::Graphics(HDC hdc)
{
  VACA_ASSERT(hdc != NULL);

  mHdc     = hdc;
  mRelease = false;
  mDelete  = false;
  mNoPaint = false;
  mFont    = NULL;
  mPenSize = 1;
}

Graphics::Graphics(HDC hdc, Image &image)
{
  VACA_ASSERT(hdc != NULL);

  mHdc     = CreateCompatibleDC(hdc);
  mRelease = false;
  mDelete  = true;
  mNoPaint = false;
  mFont    = NULL;
  mPenSize = 1;
  
  SelectObject(mHdc, image.getHbitmap());
}

/**
 * Creates a Graphics context for the specified Widget.
 */
// Graphics::Graphics(Widget *widget)
// {
//   HWND hwnd = widget->getHwnd();

//   VACA_ASSERT(hwnd != NULL);

//   mHdc     = GetDC(hwnd);
//   mRelease = true;
//   mDelete  = false;
//   mNoPaint = false;
//   mFont    = NULL;
// }

Graphics::~Graphics()
{
  VACA_ASSERT(!(mRelease && mDelete));
  
  if (mRelease)
    ReleaseDC(NULL, mHdc);
  else if (mDelete)
    DeleteDC(mHdc);
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
  VACA_ASSERT(mHdc != NULL);

  RECT rc;
  int res = ::GetClipBox(mHdc, &rc);

  if (res == NULLREGION ||
      res == ERROR)
    return Rect();
  else
    return Rect(&rc);
}

void Graphics::excludeClip(const Rect &rc)
{
  VACA_ASSERT(mHdc != NULL);
  ExcludeClipRect(mHdc, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

void Graphics::intersectClip(const Rect &rc)
{
  VACA_ASSERT(mHdc != NULL);
  IntersectClipRect(mHdc, rc.x, rc.y, rc.x+rc.w, rc.y+rc.h);
}

bool Graphics::isVisible(const Point &pt)
{
  VACA_ASSERT(mHdc != NULL);
  return PtVisible(mHdc, pt.x, pt.y) != FALSE;
}

bool Graphics::isVisible(const Rect &rc)
{
  VACA_ASSERT(mHdc != NULL);
  RECT rc2 = rc;
  return RectVisible(mHdc, &rc2) != FALSE;
}

// void Graphics::setPaintMode()
// {
//   VACA_ASSERT(mHdc != NULL);
//   SetROP2(mHdc, R2_NOT);
// }

// void Graphics::setNotMode()
// {
//   VACA_ASSERT(mHdc != NULL);
//   SetROP2(mHdc, R2_NOT);
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
  VACA_ASSERT(mHdc != NULL);

  return Color(GetPixel(mHdc, pt.x, pt.y));
}

Color Graphics::getPixel(int x, int y)
{
  VACA_ASSERT(mHdc != NULL);

  return Color(GetPixel(mHdc, x, y));
}

void Graphics::setPixel(const Point &pt, Color color)
{
  VACA_ASSERT(mHdc != NULL);

  SetPixel(mHdc, pt.x, pt.y, color.getColorRef());
}

void Graphics::setPixel(int x, int y, Color color)
{
  VACA_ASSERT(mHdc != NULL);

  SetPixel(mHdc, x, y, color.getColorRef());
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
  VACA_ASSERT(mHdc != NULL);
  MoveToEx(mHdc, x, y, NULL);
}

void Graphics::lineTo(int x, int y)
{
  VACA_ASSERT(mHdc != NULL);
  LineTo(mHdc, x, y);
}

void Graphics::beginPath()
{
  VACA_ASSERT(mHdc != NULL);
  BeginPath(mHdc);
}

void Graphics::closePath()
{
  VACA_ASSERT(mHdc != NULL);
  EndPath(mHdc);
}

void Graphics::stroke()
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ newBrush = GetStockObject(NULL_BRUSH);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);
//   int oldMode = SetBkMode(mHdc, OPAQUE);

  StrokePath(mHdc);

//   SetBkMode(mHdc, oldMode);
  DeleteObject(SelectObject(mHdc, oldPen));
  DeleteObject(SelectObject(mHdc, oldBrush));
}

void Graphics::fill()
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_NULL, 0, 0);
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ newBrush = CreateSolidBrush(mColor.getColorRef());
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);
//   int oldBkMode = SetBkMode(mHdc, OPAQUE);
  int oldPolyFillMode = SetPolyFillMode(mHdc, ALTERNATE);

  FillPath(mHdc);

//   SetBkMode(mHdc, oldBkMode);
  SetPolyFillMode(mHdc, oldPolyFillMode);
  DeleteObject(SelectObject(mHdc, oldPen));
  DeleteObject(SelectObject(mHdc, oldBrush));
}

void Graphics::drawString(const String &str, const Point &pt)
{
  drawString(str, pt.x, pt.y);
}

void Graphics::drawString(const String &str, int x, int y)
{
  VACA_ASSERT(mHdc != NULL);

  int oldMode = SetBkMode(mHdc, TRANSPARENT);
  int oldColor = SetTextColor(mHdc, mColor.getColorRef());

  if (mFont != NULL) {
    HGDIOBJ oldFont = SelectObject(mHdc, reinterpret_cast<HGDIOBJ>(mFont->getHfont()));
    TextOut(mHdc, x, y, str.c_str(), str.size());
    SelectObject(mHdc, oldFont);
  }
  else
    TextOut(mHdc, x, y, str.c_str(), str.size());

  SetBkMode(mHdc, oldMode); 
  SetTextColor(mHdc, oldColor);
}

void Graphics::drawImage(Image &image, int x, int y)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight());
}

void Graphics::drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height)
{
  VACA_ASSERT(mHdc != NULL);

  Graphics &source = image.getGraphics();

  VACA_ASSERT(source.getHdc() != NULL);
  
  BitBlt(mHdc, dstX, dstY, width, height, source.getHdc(), srcX, srcY, SRCCOPY);
}

void Graphics::drawImage(Image &image, int x, int y, Color bgColor)
{
  drawImage(image, x, y, 0, 0, image.getWidth(), image.getHeight(), bgColor);
}

void Graphics::drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height, Color bgColor)
{
  VACA_ASSERT(mHdc != NULL);

  Graphics &source = image.getGraphics();

  VACA_ASSERT(source.getHdc() != NULL);
  
  // TODO
//   MaskBlt(mHdc, dstX, dstY, width, height,
// 	  source.getHdc(), srcX, srcY,
// 	  NULL, 0, 0, MAKEROP4(SRCCOPY, SRCCOPY));
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

void Graphics::drawLine(const Point &pt1, const Point &pt2)
{
  drawLine(pt1.x, pt1.y, pt2.x, pt2.y);
}

void Graphics::drawLine(int x1, int y1, int x2, int y2)
{
  VACA_ASSERT(mHdc != NULL);

  POINT oldPos;
  HGDIOBJ newPen = CreatePen(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ oldPen = SelectObject(mHdc, newPen);

  MoveToEx(mHdc, x1, y1, &oldPos);
  LineTo(mHdc, x2, y2);
  MoveToEx(mHdc, oldPos.x, oldPos.y, NULL);

  DeleteObject(SelectObject(mHdc, oldPen));
}

void Graphics::drawRect(const Rect &rc)
{
  drawRect(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::drawRect(int x, int y, int w, int h)
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ newBrush = GetStockObject(NULL_BRUSH);
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);

  Rectangle(mHdc, x, y, x+w, y+h);

  DeleteObject(SelectObject(mHdc, oldPen));
  SelectObject(mHdc, oldBrush);
}

void Graphics::draw3dRect(const Rect &rc, Color topLeft, Color bottomRight)
{
  draw3dRect(rc.x, rc.y, rc.w, rc.h, topLeft, bottomRight);
}

void Graphics::draw3dRect(int x, int y, int w, int h, Color topLeft, Color bottomRight)
{
  VACA_ASSERT(mHdc != NULL);

  POINT oldPos;
  HGDIOBJ oldPen = SelectObject(mHdc, CreatePen(PS_SOLID, mPenSize, topLeft.getColorRef()));
  MoveToEx(mHdc, x, y+h-2, &oldPos);
  LineTo(mHdc, x, y);
  LineTo(mHdc, x+w-1, y);

  DeleteObject(SelectObject(mHdc, CreatePen(PS_SOLID, mPenSize, bottomRight.getColorRef())));
  LineTo(mHdc, x+w-1, y+h-1);
  LineTo(mHdc, x-1, y+h-1);

  DeleteObject(SelectObject(mHdc, oldPen));
  MoveToEx(mHdc, oldPos.x, oldPos.y, NULL);
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
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ newBrush = GetStockObject(NULL_BRUSH);
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);

  Ellipse(mHdc, x, y, x+w, y+h);

  DeleteObject(SelectObject(mHdc, oldPen));
  SelectObject(mHdc, oldBrush);
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
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen = CreatePen(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ oldPen = SelectObject(mHdc, newPen);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Arc(mHdc, x, y, x+w, y+h, x1, y1, x2, y2);

  DeleteObject(SelectObject(mHdc, oldPen));
}

void Graphics::drawPie(const Rect &rc, double startAngle, double sweepAngle)
{
  drawPie(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawPie(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ newBrush = GetStockObject(NULL_BRUSH);
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(mHdc, x, y, x+w, y+h, x1, y1, x2, y2);

  DeleteObject(SelectObject(mHdc, oldPen));
  SelectObject(mHdc, oldBrush);
}

void Graphics::drawChord(const Rect &rc, double startAngle, double sweepAngle)
{
  drawChord(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::drawChord(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_SOLID, mPenSize, mColor.getColorRef());
  HGDIOBJ newBrush = GetStockObject(NULL_BRUSH);
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(mHdc, x, y, x+w, y+h, x1, y1, x2, y2);

  DeleteObject(SelectObject(mHdc, oldPen));
  SelectObject(mHdc, oldBrush);
}

void Graphics::fillRect(const Rect &rc)
{
  fillRect(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillRect(int x, int y, int w, int h)
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_NULL, 0, 0);
  HGDIOBJ newBrush = CreateSolidBrush(mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);

  Rectangle(mHdc, x, y, x+w+1, y+h+1);

  DeleteObject(SelectObject(mHdc, oldPen));
  DeleteObject(SelectObject(mHdc, oldBrush));
}

void Graphics::fillEllipse(const Rect &rc)
{
  fillEllipse(rc.x, rc.y, rc.w, rc.h);
}

void Graphics::fillEllipse(int x, int y, int w, int h)
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_NULL, 0, 0);
  HGDIOBJ newBrush = CreateSolidBrush(mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);

  Ellipse(mHdc, x, y, x+w+1, y+h+1);

  DeleteObject(SelectObject(mHdc, oldPen));
  DeleteObject(SelectObject(mHdc, oldBrush));
}

void Graphics::fillPie(const Rect &rc, double startAngle, double sweepAngle)
{
  fillPie(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillPie(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_NULL, 0, 0);
  HGDIOBJ newBrush = CreateSolidBrush(mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Pie(mHdc, x, y, x+w, y+h, x1, y1, x2, y2);

  DeleteObject(SelectObject(mHdc, oldPen));
  DeleteObject(SelectObject(mHdc, oldBrush));
}

void Graphics::fillChord(const Rect &rc, double startAngle, double sweepAngle)
{
  fillChord(rc.x, rc.y, rc.w, rc.h, startAngle, sweepAngle);
}

void Graphics::fillChord(int x, int y, int w, int h, double startAngle, double sweepAngle)
{
  VACA_ASSERT(mHdc != NULL);

  HGDIOBJ newPen   = CreatePen(PS_NULL, 0, 0);
  HGDIOBJ newBrush = CreateSolidBrush(mColor.getColorRef());
  HGDIOBJ oldPen   = SelectObject(mHdc, newPen);
  HGDIOBJ oldBrush = SelectObject(mHdc, newBrush);
  int x1, y1, x2, y2;

  x1 = x+w/2 + (int)(cos(startAngle*M_PI/180)*w);
  y1 = y+h/2 - (int)(sin(startAngle*M_PI/180)*h);
  x2 = x+w/2 + (int)(cos((startAngle+sweepAngle)*M_PI/180)*w);
  y2 = y+h/2 - (int)(sin((startAngle+sweepAngle)*M_PI/180)*h);

  Chord(mHdc, x, y, x+w, y+h, x1, y1, x2, y2);

  DeleteObject(SelectObject(mHdc, oldPen));
  DeleteObject(SelectObject(mHdc, oldBrush));
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
  VACA_ASSERT(mHdc != NULL);
  
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
  GradientFill(mHdc, vert, 2, &gRect, 1,
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
    pGF(mHdc, vert, 2, &gRect, 1,
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
  HANDLE oldBrush = SelectObject(mHdc, newBrush);

  PatBlt(mHdc, x+border,   y,          w-border,  border,   PATINVERT);
  PatBlt(mHdc, x+w-border, y+border,   border,    h-border, PATINVERT);
  PatBlt(mHdc, x,          y+h-border, w-border,  border,   PATINVERT);
  PatBlt(mHdc, x,          y,          border,    h-border, PATINVERT);

  DeleteObject(SelectObject(mHdc, oldBrush));
  DeleteObject(hbitmap);
}

Size Graphics::measureString(const String &str, int fitInWidth, int flags)
{
  VACA_ASSERT(mHdc != NULL);

  RECT rc = Rect(0, 0, fitInWidth, 0);
  HGDIOBJ oldFont;

  if (mFont != NULL)
    oldFont = SelectObject(mHdc, reinterpret_cast<HGDIOBJ>(mFont->getHfont()));

  if (!str.empty()) {
    DrawText(mHdc, str.c_str(), str.size(), &rc, flags | DT_CALCRECT);
  }
  else {
    SIZE sz;

    if (GetTextExtentPoint32(mHdc, _T(" "), 1, &sz)) {
      rc.right = sz.cx;
      rc.bottom = sz.cy;
    }
  }

  if (mFont != NULL)
    SelectObject(mHdc, oldFont);

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
  VACA_ASSERT(mHdc != NULL);

  SetROP2(mHdc, drawMode);
}

HDC Graphics::getHdc()
{
  return mHdc;
}
