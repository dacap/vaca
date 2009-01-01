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

Message print_message(L"Vaca.Message.Print");
Message die_message(L"Vaca.Message.Die");

//////////////////////////////////////////////////////////////////////

void working_thread(Widget* dest)
{
  while (true) {
    Thread::sleep(1000);	// do intensive I/O work

    // after doing something, we can notify the main thread for a
    // event (like disk error, end-of-file, one packet was received,
    // etc.); here we are enqueuing a message towards the widget...
    dest->enqueueMessage(print_message);

    // look if the main thread send us a message to stop working
    Message message;
    if (Thread::peekMessage(message) && message == die_message)
      break;
  }
}

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  TextEdit m_text;

public:

  MainFrame()
    : Frame(L"Threads")
    , m_text(L"", this, TextEdit::Styles::TextArea +
			Widget::Styles::Scroll)
  {
    setLayout(new ClientLayout);
  }

  virtual bool preTranslateMessage(Message& message)
  {
    if (Frame::preTranslateMessage(message))
      return true;

    if (message == print_message) {
      print(L"A message from the working thread was received...\r\n");
      return true;
    }

    return false;
  }

  void print(const String& str)
  {
    m_text.setText(m_text.getText() + str);
    m_text.scrollLines(m_text.getLineCount());
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setVisible(true);

  Thread thread(Bind<void>(&working_thread, &frm));

  // process the message queue of the main thread
  Thread::doMessageLoop(); // ...it is the same to call "app.run()"

  // the message loop stops when the frame is closed, so we can make
  // it visible again to show a final message of "we are trying to
  // kill the working thread"...
  frm.setVisible(true);
  frm.print(L"Killing working thread...");
  frm.update();

  // here we enqueue a message to the thread (this message has to be
  // processed by the thread itself, because it is not going no any
  // specific widget)...
  thread.enqueueMessage(die_message);

  // we have to wait to the thread to finish correctly
  thread.join();

  return 0;
}
