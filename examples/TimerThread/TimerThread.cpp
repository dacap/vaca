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

class TimerThread : public Thread
{
  Thread *mParentThread;
  int mCounter;

public:

  TimerThread()
  {
    // here we are in the parent thread, the new thread start
    // in the run() method
    mParentThread = Thread::getCurrent();
    mCounter = 0;
  }

  virtual void run()
  {
    while (mCounter < 3) {
      Sleep(1000);
      Beep(400, 100);

      mCounter++;
    }

    mParentThread->postQuitMessage();
  }

};

class MainFrame : public Dialog
{
  Label mLabel;

public:

  MainFrame()
    : Dialog(String("TimerThread"), NULL,
	     DialogStyle - WithSystemMenuFrameStyle)
    , mLabel("In tree seconds this window'll be self destroyed", this)
  {
    setLayout(new BoxLayout(Vertical, true));
    setSize(preferredSize());
  }

};

class Example : public Application
{
  MainFrame mMainFrame;
  TimerThread mTimerThread;

public:

  virtual ~Example() {
    // wait the thread to finish
    mTimerThread.join();
  }

  virtual void main(std::vector<String> args) {
    // execute the timer thread
    mTimerThread.execute();

    // do modal (until the mTimerThread send us the WM_QUIT message)
    mMainFrame.doModal();

    // dispose the dialog
    mMainFrame.dispose();
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
