// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

class TimerViewer : public Widget
{
  Timer m_timer;
  bool m_on;

public:

  TimerViewer(int msecs, Color color, Widget* parent)
    : Widget(parent)
    , m_timer(msecs)
    , m_on(false)
  {
    setBgColor(color);
    setPreferredSize(Size(64, 64));
    m_timer.Tick.connect(&TimerViewer::onTick, this);
    m_timer.start();
  }

protected:

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();
    Rect rc = getClientBounds();
    Color bg = getBgColor();
    Pen blackPen(Color::Black);
    Brush foreBrush(m_on ? getBgColor()+Color(200, 200, 200):
			   getBgColor());

    g.drawRect(blackPen, rc);

    rc.shrink(1);
    g.fillRect(foreBrush, rc);
  }

private:

  void onTick()
  {
    // switch state
    m_on = !m_on;
    invalidate(true);
  }


};

// the main window
class MainFrame : public Frame
{
  Label m_label1;
  Label m_label2;
  Label m_label3;
  TimerViewer m_timer1;
  TimerViewer m_timer2;
  TimerViewer m_timer3;

public:

  MainFrame()
    : Frame(L"Timers", NULL, Frame::Styles::Default
			     - Frame::Styles::Resizable
			     - Frame::Styles::Maximizable)
    , m_label1(L"1 sec", this)
    , m_label2(L"2 sec", this)
    , m_label3(L"4 sec", this)
    , m_timer1(1000, Color::Red, this)
    , m_timer2(2000, Color(0, 128, 0), this)
    , m_timer3(4000, Color::Blue, this)
  {
    setLayout(Bix::parse(L"XY[%,%,%;%,%,%]",
			 &m_label1, &m_label2, &m_label3,
			 &m_timer1, &m_timer2, &m_timer3));
    setSize(getPreferredSize());
  }

};

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
