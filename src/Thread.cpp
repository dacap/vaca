// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include "stdvaca.h"
#include "Vaca/Thread.hpp"
#include "Vaca/Debug.hpp"
#include "Vaca/Frame.hpp"
#include "Vaca/Timer.hpp"

#include <vector>
#include <algorithm>

using namespace Vaca;

struct ThreadData
{
  int threadId;
  int frameCount;
  bool breakLoop;
  Widget *outsideWidget; // widget used to call createHWND
//   void *data;
  boost::signal<void ()> callInNextRound;

  ThreadData(int id) {
    threadId = id;
    frameCount = 0;
    breakLoop = false;
    outsideWidget = NULL;

//     VACA_TRACE("thread begin (%d)\n", ::GetCurrentThreadId());
  }

//   ~ThreadData() {
//     VACA_TRACE("thread end (%d)\n", ::GetCurrentThreadId());
//   }

};

// static boost::thread_specific_ptr<ThreadData> threadData;

static boost::mutex data_mutex;
static std::vector<ThreadData *> dataOfEachThread;

static ThreadData *getThreadData()
{
  boost::mutex::scoped_lock lock(data_mutex);
  std::vector<ThreadData *>::iterator it;
  int id = ::GetCurrentThreadId();

  // first of all search the thread-data in the list "dataOfEachThread"...
  for (it=dataOfEachThread.begin(); it!=dataOfEachThread.end(); ++it)
    if ((*it)->threadId == id)	// it's already created...
      return *it;		// return it

  // create the data for the this thread
  ThreadData *data = new ThreadData(id);
  VACA_TRACE("new data-thread %d\n", id);

  // add it to the list
  dataOfEachThread.push_back(data);

  // return the allocated data
  return data;
  
//   if (threadData.get() == NULL)
//     threadData.reset(new ThreadData);

//   return threadData.get();
}

void Vaca::__vaca_remove_all_thread_data()
{
  boost::mutex::scoped_lock lock(data_mutex);
  std::vector<ThreadData *>::iterator it;

  for (it=dataOfEachThread.begin(); it!=dataOfEachThread.end(); ++it) {
    VACA_TRACE("delete data-thread %d\n", (*it)->threadId);
    delete *it;
  }

  dataOfEachThread.clear();
}

Widget *Vaca::__vaca_get_outside_widget()
{
  return getThreadData()->outsideWidget;
}

void Vaca::__vaca_set_outside_widget(Widget *widget)
{
  getThreadData()->outsideWidget = widget;
}

Thread::Thread()
{
}

Thread::Thread(const boost::function0<void>& threadfunc)
  : boost::thread(threadfunc)
{
}

Thread::~Thread()
{
}

/**
 * Does the message loop while there are visible @ref Frame Frames.
 *
 * @see Frame::setVisible
 */
void Thread::doMessageLoop()
{
  // message loop
  Message msg;
  while (getMessage(msg))
    processMessage(msg);
}

/**
 * Does the message loop until the @a widget is hidden.
 */
void Thread::doMessageLoopFor(Widget *widget)
{
  // get widget HWND
  HWND hwnd = widget->getHWND();
  assert(::IsWindow(hwnd));

  // get parent HWND
  HWND hparent = widget->getParentHWND();

  // disable the parent HWND
  if (hparent != NULL)
    ::EnableWindow(hparent, FALSE);

  // message loop
  Message msg;
  while (widget->isVisible() && getMessage(msg))
    processMessage(msg);

  // enable the parent HWND
  if (hparent)
    ::EnableWindow(hparent, TRUE);
}

void Thread::pumpMessageQueue()
{
  Message msg;
  while (peekMessage(msg))
    processMessage(msg);
}

void Thread::breakMessageLoop()
{
  getThreadData()->breakLoop = true;
  ::PostThreadMessage(::GetCurrentThreadId(), WM_NULL, 0, 0);
}

void Thread::callInNextRound(const boost::signal<void ()>::slot_type& functor)
{
  getThreadData()->callInNextRound.connect(functor);
  ::PostThreadMessage(::GetCurrentThreadId(), WM_NULL, 0, 0);
}

/**
 * Gets a message in a blocking way, returns true if the msg parameter
 * was filled
 */
bool Thread::getMessage(Message &msg)
{
  ThreadData *threadData = getThreadData();

  // break this loop? (explicit break or no-more visible frames)
  if (threadData->breakLoop || threadData->frameCount == 0)
    return false;

  // get the message from the queue
  msg.hwnd = NULL;
  BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);

  // WM_QUIT received?
  if (bRet == 0) {
    return false;
  }
  // error
  else if (bRet == -1) {
    // TODO check the error
  }

  // WM_NULL message... maybe Timers or callInNextRound
  if (msg.message == WM_NULL) {
    Timer::pollTimers();

    // make the call and remove all the slots
    if (!threadData->callInNextRound.empty()) {
      threadData->callInNextRound();
      threadData->callInNextRound.disconnect_all_slots();
    }
  }

  return true;
}

/**
 * Gets a message in a non-blocking way, returns true if the msg
 * parameter was filled
 */
bool Thread::peekMessage(Message &msg)
{
  msg.hwnd = NULL;
  return ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE;
}

void Thread::processMessage(Message &msg)
{
  if (!preTranslateMessage(msg)) {
    // Send preTranslateMessage to the active window (useful for
    // modeless dialogs). WARNING: Don't use GetForegroundWindow
    // because it returns windows from other applications
    HWND hactive = GetActiveWindow();
    if (hactive != NULL && hactive != msg.hwnd) {
      Widget *activeWidget = Widget::fromHWND(hactive);
      if (activeWidget->preTranslateMessage(msg))
	return;
    }

    //if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
    //{
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
    //}
  }
}

/**
 * Pretranslates the message. The main function is to retrieve the
 * Widget pointer (using Widget::fromHWND()) and then (if it isn't
 * NULL), call its Widget::preTranslateMessage().
 */
bool Thread::preTranslateMessage(Message &msg)
{
  if (msg.hwnd != NULL) {
    Widget *widget = Widget::fromHWND(msg.hwnd);

    // with WinXP there is a "CicMarshalWndClass" that sends messages
    // to the application, I really don't why, and what is that window,
    // but exist
    // 
    // TODO warning, if that CicMarshalWndClass has something in the
    //      GWL_USERDATA we must to do identify an Vaca Widget with
    //      another method (like properties)
    if (widget == NULL)
      return false;

    if (widget->preTranslateMessage(msg))
      return true;
  }

  return false;
}

void Thread::addFrame(Frame *frame)
{
  ++getThreadData()->frameCount;
}

void Thread::removeFrame(Frame *frame)
{
  --getThreadData()->frameCount;

  // when this thread doesn't have more Frames to continue we must to
  // break the current message loop
  if (getThreadData()->frameCount == 0)
    Thread::breakMessageLoop();
}
