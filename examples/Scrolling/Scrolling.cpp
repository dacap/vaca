// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
#include "../resource.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class ScrollableTest : public ScrollableWidget
{
  // true if we have to wait some milliseconds after update the
  // invalidated area
  bool m_sleep;

public:
  ScrollableTest(Widget* parent)
    : ScrollableWidget(parent)
    , m_sleep(false)
  {
    setFullSize(Size(2000, 1500));

    // the red color will be used to erase the background of
    // invalidated area, but in onPaint the red will be re-erased with
    // a white brush
    setBgColor(Color::Red);
  }

  void setSleep(bool sleep)
  {
    m_sleep = sleep;
  }

protected:

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();

    // this sleep have the purpose to show the invalidated areas 100
    // milliseconds (the background filled by default with the
    // getBgColor, that in this case is Color::Red)
    if (m_sleep)
      CurrentThread::sleep(100);

    // draw the (white) background
    Brush brush(Color::White);
    g.fillRect(brush, getClientBounds());

    // draw the shapes (ellipses and round-rectangles)
    Pen pen(Color::Blue);
    Point offset = -getScrollPoint();
    for (int r=0; r<10; ++r) {
      Rect rc(offset, getFullSize());

      rc.shrink(50 * r);

      g.drawEllipse(pen, rc);
      g.drawRoundRect(pen, rc, Size(32, 32));
    }
  }
  
};

//////////////////////////////////////////////////////////////////////

void updateSleep(ScrollableTest* s, CheckBox* cb)
{
  s->setSleep(cb->isSelected());
}

int vaca_main()
{
  Application app;
  Frame frm(L"Scrolling");
  ScrollableTest wgt(&frm);
  CheckBox sleep(L"Show invalidated areas for some milliseconds", &frm);

  sleep.Click.connect(Bind<void>(&updateSleep, &wgt, &sleep));

  wgt.setConstraint(new BoxConstraint(true));
  
  frm.setLayout(new BoxLayout(Orientation::Vertical, false));
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);

  app.run();
  return 0;
}

#include "Vaca/main.h"
