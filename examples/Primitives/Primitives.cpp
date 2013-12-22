// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <Vaca/Vaca.h>
#include "../resource.h"

#include <cmath>
#ifndef M_PI
#  define M_PI 3.14159265358979323846 
#endif

using namespace Vaca;

class MainFrame : public Frame
{
  ToggleButton m_fill;

public:

  MainFrame()
    : Frame(L"Graphics Primitives")
    , m_fill(L"Fill", this)
  {
    setLayout(new AnchorLayout(Size(100, 100)));
    setSize(getNonClientSize()+Size(256, 256));

    m_fill.setConstraint(new Anchor(Rect(10, 75, 80, 20), Sides::Bottom));

    // when the "Fill" button is pressed, we must to call:
    // ((MainFrame*)this)->invalidate(true);
    m_fill.Click.connect(Bind(&MainFrame::invalidate, this, true));
  }

  virtual void onResize(ResizeEvent& ev)
  {
    invalidate(true);
    Frame::onResize(ev);
  }

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    bool fill = m_fill.isSelected();
    Rect rc = getClientBounds();

    Size extra = Size(static_cast<int>(std::cos(M_PI*45/180)*rc.w/2),
		      static_cast<int>(std::sin(M_PI*45/180)*rc.h/2));

    Pen pen(Color::Red);
    g.drawArc(pen, rc, 0, 45);
    g.drawLine(pen,
	       Point(rc.x, rc.y+rc.h/2),
	       rc.getCenter()+Point(-extra.w, extra.h));

    if (!fill) {
      g.drawRect(Pen(Color::Black), Rect(rc.getCenter()-Point(extra), extra*2));
      g.drawEllipse(Pen(Color::Green), Rect(rc.x, rc.getCenter().y-rc.h/4, rc.w, rc.h/2));
      g.drawPie(Pen(Color::Blue), rc, 135, 45);
      g.drawChord(Pen(Color::Magenta), rc, 45+2, 90-4);
    }
    else {
      g.fillRect(Brush(Color::Black), Rect(rc.getCenter()-Point(extra), extra*2));
      g.fillEllipse(Brush(Color::Green), Rect(rc.x, rc.getCenter().y-rc.h/4, rc.w, rc.h/2));
      g.fillPie(Brush(Color::Blue), rc, 135, 45);
      g.fillChord(Brush(Color::Magenta), rc, 45+2, 90-4);
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
