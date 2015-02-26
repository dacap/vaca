// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include <ctime>
#include "../resource.h"

using namespace vaca;

class MainFrame : public Frame
{
  String m_sampleText;
  Font m_font;

public:

  MainFrame()
    : Frame(L"FontMetrics")
    , m_sampleText(L"abgijqM")
    , m_font(L"Times New Roman", 128)
  {
    setBgColor(Color::White);
    setFgColor(Color::Black);

    setDoubleBuffered(true);
  }

  // after the size is changed
  virtual void onResize(ResizeEvent& ev)
  {
    // when the window is resized, repaint the client area
    invalidate(true);

    Frame::onResize(ev);
  }

  // the paint the calendar
  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    Rect rc = getClientBounds();
    Pen blue(Color::Blue);
    Pen red(Color::Red);

    String baseLineStr = L"Base line";
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
    g.drawString(L"Bounding box", Color::Blue,
		 origin.x,
		 origin.y-fontMetrics2.getHeight());

    // Draw "Base line" text
    g.drawString(baseLineStr, Color::Red,
		 origin.x-baseLineStrSize.w-4,
		 origin.y+fontMetrics.getAscent()-baseLineStrSize.h);

    // Draw the sampleText width
    drawMeasure(g, L"width=" + convert_to<String>(textSize.w),
		Color::Black,
		origin.x, origin.y+textSize.h,
		textSize.w, Orientation::Horizontal);

    // Draw the sampleText height
    drawMeasure(g, L"height=" + convert_to<String>(textSize.h),
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
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
