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

#include <Vaca/Vaca.h>

using namespace Vaca;

class TimerViewer : public Panel
{
  Timer m_timer;
  bool m_on;
  
public:

  TimerViewer(int msecs, Color color, Widget *parent)
    : Panel(parent)
    , m_timer(msecs)
    , m_on(false)
  {
    setBgColor(color);
    setPreferredSize(Size(64, 64));
    m_timer.Action.connect(Bind(&TimerViewer::onTick, this));
    m_timer.start();
  }

protected:

  virtual void onPaint(Graphics &g)
  {
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
    : Frame("Timers", NULL, FrameStyle
			    - ResizableFrameStyle
			    - MaximizableFrameStyle)
    , m_label1("1 sec", this)
    , m_label2("2 sec", this)
    , m_label3("4 sec", this)
    , m_timer1(1000, Color::Red, this)
    , m_timer2(2000, Color(0, 128, 0), this)
    , m_timer3(4000, Color::Blue, this)
  {
    setLayout(Bix::parse("XY[%,%,%;%,%,%]",
			 &m_label1, &m_label2, &m_label3,
			 &m_timer1, &m_timer2, &m_timer3));
    setSize(getPreferredSize());
  }

};

// entry point of the program
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Application app;
  MainFrame mainFrame;
  mainFrame.setVisible(true);
  app.run();
  return 0;
}
