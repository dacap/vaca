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

class Clock : public Panel
{
  Timer m_timer;
  Label m_label;
  
public:

  Clock(Widget* parent)
    : Panel(parent)
    , m_timer(1000)
    , m_label("", this)
  {
    setLayout(new ClientLayout);
    m_label.setBgColor(Color::White);

    m_timer.Tick.connect(&Clock::onTick, this);
    m_timer.start();
  }

private:

  void onTick()
  {
    std::tm now = *std::localtime(NULL);
    m_label.setText(String::format("%02d/%02d/%04d %02:%02.%02",
				   now.tm_mon+1, now.tm_mday, now.tm_year+1900,
				   now.tm_hour, now.tm_min, now.tm_sec));
  }
   

};

void create_clock(Frame* parent)
{
  // Clock clk(parent);
  // Frame frm(title);                    // creates the main window
  // Label lbl("Hello World!", &frm);     // creates a label for that window
  // frm.setLayout(new ClientLayout);     // the label'll use the client area
  // frm.setSize(frm.getPreferredSize()); // set the preferred size
  // frm.setVisible(true);                // make the window visible
  // Thread::doMessageLoop();
}

int VACA_MAIN()
{
  Application app;
  Frame frm("Threads");
  Panel panel(&frm);
  Thread thread(Bind<void>(&create_clock, &frm));

  frm.setVisible(true);

  app.run();

  // // join all threads
  // for (it=threads.begin(); it!=threads.end(); ++it) {
  //   Thread* t = *it;
  //   t->join();
  //   delete t;
  // }

  return 0;
}
