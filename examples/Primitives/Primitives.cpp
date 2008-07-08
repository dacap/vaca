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
    : Frame("Graphics Primitives")
    , m_fill("Fill", this)
  {
    setLayout(new AnchorLayout(Size(100, 100)));
    setSize(getNonClientSize()+Size(256, 256));

    m_fill.setConstraint(new Anchor(Rect(10, 75, 80, 20), Borders::Bottom));

    // when the "Fill" button is pressed, we must to call:
    // ((MainFrame *)this)->invalidate(true);
    m_fill.Action.connect(Bind(&MainFrame::invalidate, this, true));
  }

  virtual void onResize(const Size &sz)
  {
    Frame::onResize(sz);
    invalidate(true);
  }

  virtual void onPaint(Graphics &g)
  {
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
      pen.setColor(Color::Black);
      g.drawRect(pen, Rect(rc.getCenter()-Point(extra), extra*2));

      pen.setColor(Color::Green);
      g.drawEllipse(pen, Rect(rc.x, rc.getCenter().y-rc.h/4, rc.w, rc.h/2));

      pen.setColor(Color::Blue);
      g.drawPie(pen, rc, 135, 45);

      pen.setColor(Color::Magenta);
      g.drawChord(pen, rc, 45+2, 90-4);
    }
    else {
      Brush brush(Color::Black);
      g.fillRect(brush, Rect(rc.getCenter()-Point(extra), extra*2));

      brush.setColor(Color::Green);
      g.fillEllipse(brush, Rect(rc.x, rc.getCenter().y-rc.h/4, rc.w, rc.h/2));

      brush.setColor(Color::Blue);
      g.fillPie(brush, rc, 135, 45);

      brush.setColor(Color::Magenta);
      g.fillChord(brush, rc, 45+2, 90-4);
    }
  }

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
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
