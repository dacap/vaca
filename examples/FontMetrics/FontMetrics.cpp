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

#include <Vaca/Vaca.hpp>

#include <ctime>

using namespace Vaca;

class MainFrame : public Frame
{
  String m_sampleText;
  Font m_font;

public:

  MainFrame()
    : Frame("FontMetrics (WIP)")
//     , m_sampleText("abgijqM\r\nAbc")
    , m_sampleText("abgijqM")
    , m_font("Verdana", 24)
  {
    setBgColor(Color::White);
    setFgColor(Color::Black);

    setDoubleBuffered(true);
  }

//   virtual void onGotFocus(Event &ev)
//   {
//     Frame::onGotFocus(ev);

//     // caret position
//     Rect rc = getClientBounds();
//     ScreenGraphics g;
//     g.setFont(&m_font);
//     Size textSize = g.measureString(m_sampleText);
//     Point origin = rc.getCenter()-Point(textSize/2);

//     CreateCaret(getHWND(), NULL, 1, g.measureString("tmp").h);
//     ShowCaret(getHWND());
//     SetCaretPos(origin.x, origin.y);
//   }

//   virtual void onLostFocus(Event &ev)
//   {
//     Frame::onLostFocus(ev);

//     HideCaret(getHWND());
//     DestroyCaret();
//   }

  // after the size is changed
  virtual void onResize(const Size &sz)
  {
    Frame::onResize(sz);

    // when the window is resized, repaint the client area
    invalidate(true);
  }

  // the paint the calendar
  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();

//     String text("gM\r\ngR");

//     g.setColor(Color(180, 200, 240));
//     g.drawRect(Rect(origin, textSize));

//     g.setColor(getFgColor());
//     g.drawString(text, Rect(origin, textSize));

//     SetTextCharacterExtra(g.getHDC(), 32);
    forEachChar(&g, drawChr());

    // Basic font information
    g.setFont(getFont());
    int y = 0, h = g.measureString("tmp").h;

//     g.drawString(String("Font size: ") + String::fromInt(MulDiv(m_font.getSize(), GetDeviceCaps(g.getHDC(), LOGPIXELSY), 72)), 0, y);
//     y += h;

//     g.drawString(String("Measured size: ") +
// 		 String::fromInt(textSize.w) + "x" +
// 		 String::fromInt(textSize.h), 0, y);
//     y += h;

    // --separator--
//     y += 1;
//     g.drawLine(0, y, 128, y);
//     y += 2;

    // Font metrics
    TEXTMETRIC tm;
    GetTextMetrics(g.getHDC(), &tm);

//     NEWTEXTMETRIC tm;
//     GetTextMetrics(g.getHDC(), &tm);

    // DeviceUnits = (DesignUnits/unitsPerEm) * DeviceResolution * (PointSize/72)
    
    g.drawString(String("Text output height: ") +
		 String::fromInt((tm.tmHeight+
				  tm.tmExternalLeading+
				  tm.tmInternalLeading)//  / ntm.ntmSizeEM 
				 * GetDeviceCaps(g.getHDC(), LOGPIXELSY)
				 * m_font.getPointSize() / 72), 0, y);
    y += h;

    g.drawString(String("tmHeight: ") + String::fromInt(tm.tmHeight), 0, y);
    y += h;
    g.drawString(String("tmAscent: ") + String::fromInt(tm.tmAscent), 0, y);
    y += h;
    g.drawString(String("tmDescent: ") + String::fromInt(tm.tmDescent), 0, y);
    y += h;
    g.drawString(String("tmInternalLeading: ") + String::fromInt(tm.tmInternalLeading), 0, y);
    y += h;
    g.drawString(String("tmExternalLeading: ") + String::fromInt(tm.tmExternalLeading), 0, y);
    y += h;
    g.drawString(String("tmAveCharWidth: ") + String::fromInt(tm.tmAveCharWidth), 0, y);
    y += h;
    g.drawString(String("tmMaxCharWidth: ") + String::fromInt(tm.tmMaxCharWidth), 0, y);
    y += h;
    g.drawString(String("tmWeight: ") + String::fromInt(tm.tmWeight), 0, y);
    y += h;
    g.drawString(String("tmOverhang: ") + String::fromInt(tm.tmOverhang), 0, y);
    y += h;
    g.drawString(String("tmDigitizedAspectX: ") + String::fromInt(tm.tmDigitizedAspectX), 0, y);
    y += h;
    g.drawString(String("tmDigitizedAspectY: ") + String::fromInt(tm.tmDigitizedAspectY), 0, y);
    y += h;
    g.drawString(String("tmFirstChar: ") + String::fromInt(tm.tmFirstChar), 0, y);
    y += h;
    g.drawString(String("tmLastChar: ") + String::fromInt(tm.tmLastChar), 0, y);
    y += h;
    g.drawString(String("tmDefaultChar: ") + String::fromInt(tm.tmDefaultChar), 0, y);
    y += h;
    g.drawString(String("tmBreakChar: ") + String::fromInt(tm.tmBreakChar), 0, y);
    y += h;
    g.drawString(String("tmItalic: ") + String::fromInt(tm.tmItalic), 0, y);
    y += h;
    g.drawString(String("tmUnderlined: ") + String::fromInt(tm.tmUnderlined), 0, y);
    y += h;
    g.drawString(String("tmStruckOut: ") + String::fromInt(tm.tmStruckOut), 0, y);
    y += h;
    g.drawString(String("tmPitchAndFamily: ") + String::fromInt(tm.tmPitchAndFamily), 0, y);
    y += h;
    g.drawString(String("tmCharSet: ") + String::fromInt(tm.tmCharSet), 0, y);
    y += h;

    // Font sample box
    g.setFont(&m_font);
    Size textSize = g.measureString(m_sampleText);
    Point origin = rc.getCenter()-Point(textSize/2);
    g.setFont(getFont());

    // Font sample width
    drawMeasure(g, String::fromInt(textSize.w),
		origin.x, origin.y+textSize.h,
		textSize.w, Orientation::Horizontal);

    // Font sample height
    drawMeasure(g, String::fromInt(textSize.h),
		origin.x+textSize.w, origin.y,
		textSize.h, Orientation::Vertical);
  }

private:

  void drawMeasure(Graphics &g,
		   const String &text,
		   int x, int y, int size,
		   Orientation orientation)
  {
    switch (orientation) {

      case Orientation::Horizontal: {
	Pen pen(Color::Black);
	Size textSize = g.measureString(text);
	g.drawString(text, x+size/2-textSize.w/2, y);

	g.drawLine(pen, x, y, x, y+textSize.h);
	g.drawLine(pen, x, y+textSize.h/2, x+size/2-textSize.w/2-2, y+textSize.h/2);
	g.drawLine(pen, x+size/2+textSize.w/2+2, y+textSize.h/2, x+size, y+textSize.h/2);
	g.drawLine(pen, x+size-1, y, x+size-1, y+textSize.h);
	break;
      }

      case Orientation::Vertical: {
	LOGFONT lf;
	Font* currentFont = g.getFont();

	if (currentFont->getLogFont(&lf)) {
	  lf.lfEscapement = 900;
	  lf.lfOrientation = 900;

	  Font verticalFont(&lf);

	  g.setFont(&verticalFont);

	  Pen pen(Color::Black);
	  Size textSize = g.measureString(text);
	  g.drawString(text, x, y+size/2+textSize.h/2);

	  g.drawLine(pen, x, y, x+textSize.w, y);
	  g.drawLine(pen, x+textSize.w/2, y, x+textSize.w/2, y+size/2-textSize.h/2-2);
	  g.drawLine(pen, x+textSize.w/2, y+size/2+textSize.h/2+2, x+textSize.w/2, y+size);
	  g.drawLine(pen, x, y+size-1, x+textSize.w, y+size-1);

	  g.setFont(currentFont);
	}
	break;
      }
    }
  }

  struct drawChr {
    void operator()(Graphics* g, const String& str, const Rect& rc)
    {
      g->drawString(str, rc.getOrigin());
    }
  };
    
  template<typename Functor>
  void forEachChar(Graphics* g, Functor functor)
  {
    g->setFont(&m_font);

    Rect rc = getClientBounds();
    Size textSize = g->measureString(m_sampleText);
    Point origin = rc.getCenter()-Point(textSize/2);
    Rect output(origin, Size(0, 0));

    for (String::iterator it = m_sampleText.begin();
	 it != m_sampleText.end(); ++it) {
      String chrText;
      chrText.push_back(*it);

      output.setSize(g->measureString(chrText));

//       ABC abc;
//       GetCharABCWidths(g->getHDC(), *it, *it, &abc);
//       output.w = (abc.abcA+abc.abcB+abc.abcC) * 72 / GetDeviceCaps(g->getHDC(), LOGPIXELSX);

      if (*it == '\r') {
	output.x = origin.x;
      }
      else if (*it == '\n') {
	output.y += g->measureString("t").h;
      }
      else {
	functor(g, chrText, output);

	int interChr = GetTextCharacterExtra(g->getHDC());
	output.x += output.w + interChr;
      }
    }
  }

//   void onChangeFont(Font &font)
//   {
//     FontDialog dialog(&font, this);
//     if (dialog.doModal()) {
//       layout();
//       invalidate(false);
//     }
//   }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example* app(new Example);
  app->run();
  delete app;
  return 0;
}
