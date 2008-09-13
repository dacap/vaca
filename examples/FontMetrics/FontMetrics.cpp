// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include <Vaca/Vaca.h>

#include <ctime>

using namespace Vaca;

class MainFrame : public Frame
{
  String m_sampleText;
  Font m_font;

public:

  MainFrame()
    : Frame("FontMetrics")
    // , m_sampleText("abgijqM\r\nAbc")
    , m_sampleText("abgijqM")
    , m_font("Times New Roman", 128)
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

//     CreateCaret(getHandle(), NULL, 1, g.measureString("tmp").h);
//     ShowCaret(getHandle());
//     SetCaretPos(origin.x, origin.y);
//   }

//   virtual void onLostFocus(Event &ev)
//   {
//     Frame::onLostFocus(ev);

//     HideCaret(getHandle());
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
    Pen blue(Color::Blue);
    Pen red(Color::Red);

    String baseLineStr = "Base line";
    Size baseLineStrSize = g.measureString(baseLineStr);
    
    // Measure the sampleText string
    FontMetrics fontMetrics;
    g.setFont(m_font);
    g.getFontMetrics(fontMetrics);

    Size textSize = g.measureString(m_sampleText);
    Point origin = rc.getCenter()-Point(textSize/2);

    // Draw the sampleText string:
    // (a) First with drawString we can draw a shadow
    g.setColor(Color(100, 100, 100));
    g.drawString(m_sampleText, origin+Point(1, 1));
    // (b) Draw the same string (in origin point) but with our code
    g.setColor(Color(200, 200, 200));
    forEachChar(g, drawChr());

    // Bounding rectangle of the sampleText
    g.drawRect(blue, Rect(origin, textSize));

    // Use Ascent and Descent to draw the base line, (1) and (2) must be equal
    g.drawLine(red,
	       origin.x-baseLineStrSize.w-4,
	       origin.y+fontMetrics.getAscent(), // (1)
	       origin.x+textSize.w,
	       origin.y+textSize.h-fontMetrics.getDescent()); // (2)
    
    // Restore to the default widget font
    FontMetrics fontMetrics2;
    g.setFont(getFont());
    g.getFontMetrics(fontMetrics2);

    // Draw origin
    g.setColor(Color::Blue);
    g.drawString("Bounding box",
		 origin.x,
		 origin.y-fontMetrics2.getHeight());

    // Draw "Base line" text
    g.setColor(Color::Red);
    g.drawString(baseLineStr,
		 origin.x-baseLineStrSize.w-4,
		 origin.y+fontMetrics.getAscent()-baseLineStrSize.h);

    // Draw the sampleText width
    g.setColor(Color::Black);
    drawMeasure(g, "width=" + String::fromInt(textSize.w),
		origin.x, origin.y+textSize.h,
		textSize.w, Orientation::Horizontal);

    // Draw the sampleText height
    drawMeasure(g, "height=" + String::fromInt(textSize.h),
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
	Pen pen(g.getColor());
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
	Font currentFont = g.getFont();

	if (currentFont.getLogFont(&lf)) {
	  lf.lfEscapement = 900;
	  lf.lfOrientation = 900;

	  Font verticalFont(&lf);

	  g.setFont(verticalFont);

	  Pen pen(g.getColor());
	  Size textSize = g.measureString(text);
	  g.drawString(text, x, y+size/2+textSize.w/2);

	  g.drawLine(pen, x, y, x+textSize.h, y);
	  g.drawLine(pen, x+textSize.h/2, y, x+textSize.h/2, y+size/2-textSize.w/2-2);
	  g.drawLine(pen, x+textSize.h/2, y+size/2+textSize.w/2+2, x+textSize.h/2, y+size);
	  g.drawLine(pen, x, y+size-1, x+textSize.h, y+size-1);

	  g.setFont(currentFont);
	}
	break;
      }
    }
  }

  struct drawChr {
    void operator()(Graphics& g, const String& str, const Rect& rc)
    {
      g.drawString(str, rc.getOrigin());
    }
  };
    
  template<typename Functor>
  void forEachChar(Graphics& g, Functor functor)
  {
    Rect rc = getClientBounds();
    Size textSize = g.measureString(m_sampleText);
    Point origin = rc.getCenter()-Point(textSize/2);
    Rect output(origin, Size(0, 0));
    FontMetrics fontMetrics;

    g.getFontMetrics(fontMetrics);

    for (String::iterator
	   it=m_sampleText.begin(); it!=m_sampleText.end(); ++it) {
      String chrText;
      chrText.push_back(*it);

      output.setSize(g.measureString(chrText));

//       ABC abc;
//       GetCharABCWidths(g.getHDC(), *it, *it, &abc);
//       output.w = (abc.abcA+abc.abcB+abc.abcC) * 72 / GetDeviceCaps(g.getHDC(), LOGPIXELSX);

      if (*it == '\r') {
	output.x = origin.x;
      }
      else if (*it == '\n') {
	output.y += fontMetrics.getHeight();
      }
      else {
	functor(g, chrText, output);

	// int interChr = GetTextCharacterExtra(g.getHDC());
	// output.x += output.w + interChr;
	output.x += output.w;
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

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Application app;
  MainFrame frm;
  frm.setVisible(true);
  app.run();
  return 0;
}
