// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

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

int VACA_MAIN()
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
