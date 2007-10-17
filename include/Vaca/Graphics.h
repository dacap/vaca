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

#ifndef VACA_GRAPHICS_H
#define VACA_GRAPHICS_H

#include "Vaca/base.h"
#include "Vaca/Rect.h"
#include "Vaca/Color.h"
#include "Vaca/String.h"

#include <boost/noncopyable.hpp>

namespace Vaca {

class Font;
class Image;
class Widget;

/**
 * Class to handle a graphics context.  It's used to handle the Win32 HDCs.
 */
class VACA_DLL Graphics : private boost::noncopyable
{
  HDC mHdc;
  bool mRelease : 1;
  bool mDelete : 1;
  bool mNoPaint : 1;
  Color mColor;
  Font *mFont;
  int mPenSize;

protected:
  
  Graphics();

public:

  Graphics(HDC hdc);
  Graphics(HDC hdc, Image &image);
//   Graphics(Widget *widget);
  virtual ~Graphics();

  Graphics getScreen();

  void noPaint();
  bool wasPainted();

  Rect getClipBounds();
  void excludeClip(const Rect &rc);
  void intersectClip(const Rect &rc);
  bool isVisible(const Point &pt);
  bool isVisible(const Rect &rc);

//   void setPaintMode();
//   void setXORMode();

  Color getColor();
  void setColor(const Color &color);

  Font &getFont();
  void setFont(Font &font);

  int getPenSize();
  void setPenSize(int penSize);

  Color getPixel(const Point &pt);
  Color getPixel(int x, int y);
  void setPixel(const Point &pt, Color color);
  void setPixel(int x, int y, Color color);

  void moveTo(const Point &pt);
  void lineTo(const Point &pt);
  void moveTo(int x, int y);
  void lineTo(int x, int y);

  void beginPath();
  void closePath();
  void stroke();
  void fill();
  // void strokeAndFill();
  // void quadTo(int x1, int y1, int x2, int y2);
  // void curveTo(int x1, int y1, int x2, int y2, int x3, int y3);

  void drawString(const String &str, const Point &pt);
  void drawString(const String &str, int x, int y);

  void drawImage(Image &image, int x, int y);
  void drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height);
  void drawImage(Image &image, int x, int y, Color bgColor);
  void drawImage(Image &image, int dstX, int dstY, int srcX, int srcY, int width, int height, Color bgColor);
  void drawImage(Image &image, const Point &pt);
  void drawImage(Image &image, const Point &pt, const Rect &rc);
  void drawImage(Image &image, const Point &pt, Color bgColor);
  void drawImage(Image &image, const Point &pt, const Rect &rc, Color bgColor);

  void drawLine(const Point &pt1, const Point &pt2);
  void drawLine(int x1, int y1, int x2, int y2);
  void drawRect(const Rect &rc);
  void drawRect(int x, int y, int w, int h);
  void draw3dRect(const Rect &rc, Color topLeft, Color bottomRight);
  void draw3dRect(int x, int y, int w, int h, Color topLeft, Color bottomRight);
  void drawEllipse(const Rect &rc);
  void drawEllipse(int x, int y, int w, int h);
  void drawArc(const Rect &rc, int startAngle, int sweepAngle);
  void drawArc(int x, int y, int w, int h, int startAngle, int sweepAngle);
  void drawPie(const Rect &rc, double startAngle, double sweepAngle);
  void drawPie(int x, int y, int w, int h, double startAngle, double sweepAngle);
  void drawChord(const Rect &rc, double startAngle, double sweepAngle);
  void drawChord(int x, int y, int w, int h, double startAngle, double sweepAngle);

  void fillRect(const Rect &rc);
  void fillRect(int x, int y, int w, int h);
  void fillEllipse(const Rect &rc);
  void fillEllipse(int x, int y, int w, int h);
  void fillPie(const Rect &rc, double startAngle, double sweepAngle);
  void fillPie(int x, int y, int w, int h, double startAngle, double sweepAngle);
  void fillChord(const Rect &rc, double startAngle, double sweepAngle);
  void fillChord(int x, int y, int w, int h, double startAngle, double sweepAngle);

  void fillGradientRect(const Rect &rc, Color startColor, Color endColor, Orientation orientation);
  void fillGradientRect(int x, int y, int w, int h, Color startColor, Color endColor, Orientation orientation);
  void drawGradientRect(const Rect &rc, Color topLeft, Color topRight, Color bottomLeft, Color bottomRight);
  void drawGradientRect(int x, int y, int w, int h, Color topLeft, Color topRight, Color bottomLeft, Color bottomRight);

  void drawXorFrame(const Rect &rc, int border = 3);
  void drawXorFrame(int x, int y, int w, int h, int border = 3);

  // 32767 is the limit for Win98
  Size measureString(const String &str, int fitInWidth = 32767, int flags = DT_WORDBREAK);

  // SetROP2 wrapper
  void setRop2(int drawMode);

  HDC getHdc();

};

class VACA_DLL ScreenGraphics : public Graphics
{
public:
  ScreenGraphics();
};

} // namespace Vaca

#endif
