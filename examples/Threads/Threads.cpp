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
#include <ctime>
#include "../resource.h"

using namespace Vaca;

Message progress_message(L"Vaca.Message.Progress");
Message kill_message(L"Vaca.Message.Kill");
Message end_message(L"Vaca.Message.End");

//////////////////////////////////////////////////////////////////////

void working_thread(Widget* dest)
{
  ThreadId id = CurrentThread::getId();

  while (true) {
    CurrentThread::sleep(250);	// do intensive I/O work

    // After doing something, we can notify the main thread for a
    // event (like disk error, end-of-file, one packet was received,
    // etc.); here we are enqueuing a message towards the widget...
    dest->enqueueMessage(Message(progress_message, (void*)id));

    // Look if the main thread send us a message to stop working
    Message message;
    if (CurrentThread::peekMessage(message) && message == kill_message)
      break;
  }

  // Notify the main thread so it can remove the related ThreadView widget
  dest->enqueueMessage(Message(end_message, (void*)id));
}

//////////////////////////////////////////////////////////////////////

class ThreadView : public Widget
{
  Thread* m_thread;
  ProgressBar m_bar;
  Button m_kill;

public:
  Signal1<void, Thread*> KillThread;

  ThreadView(Thread* thread, Widget* parent)
    : Widget(parent)
    , m_thread(thread)
    , m_bar(0, 100, this)
    , m_kill(format_string(L"Kill Thread %d", thread->getId()), this)
  {
    setLayout(new BoxLayout(Orientation::Horizontal, false, 0, 4));
    m_bar.setConstraint(new BoxConstraint(true));
    m_kill.setPreferredSize(100, m_kill.getPreferredSize().h);
    m_kill.Click.connect(Bind(&ThreadView::onKillButtonPressed, this));
  }

  Thread* getThread() {
    return m_thread;
  }

  void makeProgress()
  {
    m_bar.addValue(1);
  }

  void setupDyingThread()
  {
    m_kill.setText(format_string(L"%d is dying", m_thread->getId()));

    m_bar.setEnabled(false);
    m_kill.setEnabled(false);
  }

protected:
  void onKillButtonPressed()
  {
    // Fire 'KillThread' signal
    KillThread(m_thread);
  }
};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  typedef std::vector<Thread*> Threads;
  Button m_createThread;
  Threads m_threads;
  StatusBar m_statusBar;

public:

  MainFrame()
    : Frame(L"Threads")
    , m_createThread(L"Create Thread", this)
    , m_statusBar(this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));

    m_createThread.Click.connect(Bind(&MainFrame::onCreateThread, this));

    setSize(getBounds().w, getPreferredSize().h);
  }

  virtual ~MainFrame()
  {
    // Here we enqueue a kill-message to all threads (this message has
    // to be processed by each thread itself)...
    for (Threads::iterator it = m_threads.begin(); it != m_threads.end(); ++it) {
      Thread* thread = *it;
      thread->enqueueMessage(kill_message);
    }

    // Now we wait all threads to finish
    for (Threads::iterator it = m_threads.begin(); it != m_threads.end(); ++it) {
      Thread* thread = *it;
      thread->join();
      delete thread;
    }
  }

protected:

  void onCreateThread()
  {
    // Create the new thread
    Thread* newThread = new Thread(Bind<void>(&working_thread, this));
    m_threads.push_back(newThread);

    // Create a progress bar and a "Kill" button to stop the new thread
    ThreadView* threadView = new ThreadView(newThread, this);
    threadView->KillThread.connect(&MainFrame::onKillThread, this);

    // resize
    setSize(getBounds().w, getPreferredSize().h);
  }

  void onKillThread(Thread* thread)
  {
    // Send a message to the thread to kill it
    thread->enqueueMessage(kill_message);

    // Disable the button to kill the thread again
    if (ThreadView* threadView = getThreadView(thread->getId()))
      threadView->setupDyingThread();
  }

  // We need to override "preTranslateMessage" to get messages from
  // other threads
  virtual bool preTranslateMessage(Message& message)
  {
    if (Frame::preTranslateMessage(message))
      return true;

    if (message == progress_message) {
      ThreadId id = (ThreadId)message.getPayload();
      if (ThreadView* threadView = getThreadView(id)) {
	threadView->makeProgress();
      }
      return true;
    }
    else if (message == end_message) {
      ThreadId id = (ThreadId)message.getPayload();
      if (ThreadView* threadView = getThreadView(id)) {
	// Get the thread that sent us the "end_message"
	Thread* thread = threadView->getThread();

	// Erase it from the list of current running threads
	remove_from_container(m_threads, thread);

	// Remove the thread-view widget
	delete threadView;

	// Resize
	setSize(getBounds().w, getPreferredSize().h);

	// Join the thread and delete it
	thread->join();
	delete thread;

	// Show a status message, we are done
	m_statusBar.setText(format_string(L"Thread %d joined.", id));
      }
      return true;
    }

    return false;
  }

  ThreadView* getThreadView(ThreadId id)
  {
    WidgetList children = getChildren();
    for (WidgetList::iterator it=children.begin(); it!=children.end(); ++it)
      if (ThreadView* threadView = dynamic_cast<ThreadView*>(*it))
	if (threadView->getThread()->getId() == id)
	  return threadView;
    return NULL;
  }

};

//////////////////////////////////////////////////////////////////////

int vaca_main()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}

#include "Vaca/main.h"
