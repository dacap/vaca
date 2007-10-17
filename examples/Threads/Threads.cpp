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

#include "Vaca/Vaca.h"

using namespace Vaca;

static Mutex beepMutex;	  // mutex used to access to the Beep function

class MainFrame : public Frame
{
  Label mLabel;
public:
  MainFrame(int num)
    : Frame(String("Thread"))
    , mLabel(String("Thread ID ")+String::fromInt(num), this)
  {
    setLayout(new ClientLayout);
    setSize(preferredSize());
  }
};

class ThreadWithFrame : public Thread
{
public:

  virtual void run()
  {
    MainFrame frame1(getId());
    MainFrame frame2(getId());
    frame1.setVisible(true);
    frame2.setVisible(true);

    doMessageLoop();

    // this thread finish!
    ScopedLock lock(beepMutex);
    Beep(100, 50);
  }

};

class Example : public Application
{
public:

  virtual void main(std::vector<String> args)
  {
    ThreadWithFrame threads[4];

    Frame supremeFrame("Threads");
    Label label("Press the button to kill all threads. "
		"Each thread has two windows, when you "
		"close both windows, you should hear a noise "
		"(which means that the thread was joined). ", &supremeFrame);
    label.setConstraint(new BoxConstraint(true));

    Button killAllButton("Kill All", &supremeFrame);
    killAllButton.Action.connect(Bind(&Example::onKillAll, this,
				      threads, 4));

    supremeFrame.setLayout(new BoxLayout(Vertical, false));
    supremeFrame.setSize(Size(256, 256));
    supremeFrame.setVisible(true);

    // execute all threads
    for (int c=0; c<4; c++)
      threads[c].execute();

    // do the message loop for the "supremeFrame"
    doMessageLoop();

    // join with all threads
    for (int c=0; c<4; c++)
      threads[c].join();
  }

  void onKillAll(ThreadWithFrame *threads, int size)
  {
    for (int c=0; c<size; ++c)
      threads[c].postQuitMessage();
  }

};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;

  // the application finish!
  ScopedLock lock(beepMutex);
  Beep(900, 100);

  return 0;
}
