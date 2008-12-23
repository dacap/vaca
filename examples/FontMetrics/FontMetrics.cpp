// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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
    , m_sampleText("abgijqM")
    , m_font("Times New Roman", 128)
  {
    setBgColor(Color::White);
    setFgColor(Color::Black);

    setDoubleBuffered(true);
  }

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
    g.drawString(m_sampleText, Color(100, 100, 100), origin+Point(1, 1));
    // (b) Draw the same string (in origin point) but with our code
    forEachChar(g, Color(200, 200, 200), drawChr());

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
    g.drawString("Bounding box", Color::Blue,
		 origin.x,
		 origin.y-fontMetrics2.getHeight());

    // Draw "Base line" text
    g.drawString(baseLineStr, Color::Red,
		 origin.x-baseLineStrSize.w-4,
		 origin.y+fontMetrics.getAscent()-baseLineStrSize.h);

    // Draw the sampleText width
    drawMeasure(g, "width=" + String::fromInt(textSize.w),
		Color::Black,
		origin.x, origin.y+textSize.h,
		textSize.w, Orientation::Horizontal);

    // Draw the sampleText height
    drawMeasure(g, "height=" + String::fromInt(textSize.h),
		Color::Black,
		origin.x+textSize.w, origin.y,
		textSize.h, Orientation::Vertical);
  }

private:

  void drawMeasure(Graphics& g,
		   const String& text,
		   const Color& color,
		   int x, int y, int size,
		   Orientation orientation)
  {
    switch (orientation) {

      case Orientation::Horizontal: {
	Size textSize = g.measureString(text);
	g.drawString(text, color, x+size/2-textSize.w/2, y);

	Pen pen(color);
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

	  Size textSize = g.measureString(text);
	  g.drawString(text, color, x, y+size/2+textSize.w/2);

	  Pen pen(color);
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
    void operator()(Graphics& g, const String& str, const Color& color, const Rect& rc)
    {
      g.drawString(str, color, rc.getOrigin());
    }
  };
    
  template<typename Functor>
  void forEachChar(Graphics& g, const Color& color, Functor functor)
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

      if (*it == '\r') {
	output.x = origin.x;
      }
      else if (*it == '\n') {
	output.y += fontMetrics.getHeight();
      }
      else {
	functor(g, chrText, color, output);
	output.x += output.w;
      }
    }
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setVisible(true);
  app.run();
  return 0;
}
