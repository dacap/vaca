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

class ThreadKiller : public Thread
{
  Thread *m_parentThread;
  int m_counter;

public:

  ThreadKiller()
  {
    // here we are in the parent thread, the new thread start
    // in the run() method
    m_parentThread = Thread::getCurrent();
    m_counter = 0;
  }

  virtual void run()
  {
    while (m_counter < 3) {
      ::Sleep(1000);		// Win32 call: sleep 1 second
      ::Beep(400, 100);		// play a sound

      m_counter++;
    }

    m_parentThread->breakMessageLoop();
  }

};

class MainFrame : public Dialog
{
  Label m_label;

public:

  MainFrame()
    : Dialog(String("ThreadKiller"), NULL,
	     DialogStyle - WithSystemMenuFrameStyle)
    , m_label("In tree seconds this window'll be self destroyed", this)
  {
    setLayout(new BoxLayout(Vertical, true));
    setSize(getPreferredSize());
  }

};

class Example : public Application
{
  MainFrame m_mainFrame;
  ThreadKiller m_threadKiller;

public:

  virtual ~Example() {
    // wait the thread to finish
    m_threadKiller.join();
  }

  virtual void main(std::vector<String> args) {
    // execute the thread killer
    m_threadKiller.execute();

    // do modal (until the m_threadKiller break us the message loop)
    m_mainFrame.doModal();

    // hide the dialog (so the default doMessageLoop() after main()
    // isn't executed)
    m_mainFrame.setVisible(false);
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
