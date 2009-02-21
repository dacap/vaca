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

#include "Vaca/Thread.h"
#include "Vaca/Debug.h"
#include "Vaca/Frame.h"
#include "Vaca/Signal.h"
#include "Vaca/Timer.h"
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/Slot.h"
#include "Vaca/TimePoint.h"

#include <vector>
#include <algorithm>
#include <memory>

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

// TODO
// - replace with some C++0x's Thread-Local Storage
// - use __thread in GCC, and __declspec( thread ) in MSVC
// - use TlsAlloc

struct ThreadData
{
  /// The ID of this thread.
  /// 
  ThreadId threadId;

  /// Visible frames in this thread. A frame is an instance of Frame
  /// class.
  /// 
  std::vector<Frame*> frames;

  TimePoint updateIndicatorsMark;
  bool updateIndicators : 1;

  /// True if the message-loop must be stopped.
  /// 
  bool breakLoop : 1;

  /// Widget used to call createHandle.
  /// 
  Widget* outsideWidget;

  ThreadData(ThreadId id) {
    threadId = id;
    breakLoop = false;
    updateIndicators = true;
    outsideWidget = NULL;
  }

};

static Mutex data_mutex;
static std::vector<ThreadData*> dataOfEachThread;

static ThreadData* getThreadData()
{
  ScopedLock hold(data_mutex);
  std::vector<ThreadData*>::iterator it, end = dataOfEachThread.end();
  ThreadId id = ::GetCurrentThreadId();

  // first of all search the thread-data in the list "dataOfEachThread"...
  for (it=dataOfEachThread.begin();
       it!=end;
       ++it) {
    if ((*it)->threadId == id)	// it's already created...
      return *it;		// return it
  }

  // create the data for the this thread
  ThreadData* data = new ThreadData(id);
  VACA_TRACE("new data-thread %d\n", id);

  // add it to the list
  dataOfEachThread.push_back(data);

  // return the allocated data
  return data;
}

void Vaca::details::remove_all_thread_data()
{
  ScopedLock hold(data_mutex);
  std::vector<ThreadData*>::iterator it, end = dataOfEachThread.end();

  for (it=dataOfEachThread.begin(); it!=end; ++it) {
    VACA_TRACE("delete data-thread %d\n", (*it)->threadId);
    delete *it;
  }

  dataOfEachThread.clear();
}

Widget* Vaca::details::get_outside_widget()
{
  return getThreadData()->outsideWidget;
}

void Vaca::details::set_outside_widget(Widget* widget)
{
  getThreadData()->outsideWidget = widget;
}

//////////////////////////////////////////////////////////////////////

static DWORD WINAPI ThreadProxy(LPVOID slot)
{
  std::auto_ptr<Slot0<void> > slot_ptr(reinterpret_cast<Slot0<void>*>(slot));
  (*slot_ptr)();
  return 0;
}

Thread::Thread()
{
  m_handle = GetCurrentThread();
  m_id = GetCurrentThreadId();

  VACA_TRACE("current Thread (%p, %d)\n", this, m_id);
}

/// @throw CreateThreadException
///   If the thread couldn't be created by Win32's @c CreateThread.
/// 
/// @internal
/// 
void Thread::_Thread(const Slot0<void>& slot)
{
  Slot0<void>* slotclone = slot.clone();
  DWORD id;

  m_handle = CreateThread(NULL, 0,
			  ThreadProxy,
			  // clone the slot
			  reinterpret_cast<LPVOID>(slotclone),
			  CREATE_SUSPENDED, &id);
  if (!m_handle) {
    delete slotclone;
    throw CreateThreadException();
  }

  m_id = id;

  VACA_TRACE("new Thread (%p, %d)\n", this, m_id);
  ResumeThread(m_handle);
}

Thread::~Thread()
{
  if (isJoinable() && m_handle != NULL) {
     CloseHandle(reinterpret_cast<HANDLE>(m_handle));
     m_handle = NULL;
  }

  VACA_TRACE("delete Thread (%p, %d)\n", this, m_id);
}

/// Returns the thread ID.
/// 
/// @win32
///   This is equal to @msdn{GetCurrentThreadId} for the current
///   thread or the ID returned by @msdn{CreateThread}.
/// @endwin32
/// 
ThreadId Thread::getId() const
{
  return m_id;
}

/// Waits the thread to finish, and them closes it.
/// 
void Thread::join()
{
  assert(m_handle != NULL);
  assert(isJoinable());

  WaitForSingleObject(reinterpret_cast<HANDLE>(m_handle), INFINITE);
  CloseHandle(reinterpret_cast<HANDLE>(m_handle));
  m_handle = NULL;

  VACA_TRACE("join Thread (%p, %d)\n", this, m_id);
}

/// Returns true if this thread can be waited by the current thread.
/// 
bool Thread::isJoinable() const
{
  return m_id != ::GetCurrentThreadId();
}

/// Sets the priority of the thread.
/// 
/// Thread priority is relative to the other threads of the same
/// process. If you want to change the priority of the entire process
/// (respecting to all other processes) you have to use the
/// Application#setProcessPriority method. In other words, you should
/// use this method only if you have more than one thread in your
/// application and you want to make run faster one thread than other.
/// 
/// @param priority
///   Can be one of the following values:
///   One of the following values:
///   @li ThreadPriority::Idle
///   @li ThreadPriority::Lowest
///   @li ThreadPriority::Low
///   @li ThreadPriority::Normal
///   @li ThreadPriority::High
///   @li ThreadPriority::Highest
///   @li ThreadPriority::TimeCritical
///   
/// @see Application#setProcessPriority
/// 
void Thread::setThreadPriority(ThreadPriority priority)
{
  assert(m_handle != NULL);

  int nPriority;
  switch (priority) {
    case ThreadPriority::Idle:         nPriority = THREAD_PRIORITY_IDLE; break;
    case ThreadPriority::Lowest:       nPriority = THREAD_PRIORITY_LOWEST; break;
    case ThreadPriority::Low:          nPriority = THREAD_PRIORITY_BELOW_NORMAL; break;
    case ThreadPriority::Normal:       nPriority = THREAD_PRIORITY_NORMAL; break;
    case ThreadPriority::High:         nPriority = THREAD_PRIORITY_ABOVE_NORMAL; break;
    case ThreadPriority::Highest:      nPriority = THREAD_PRIORITY_HIGHEST; break;
    case ThreadPriority::TimeCritical: nPriority = THREAD_PRIORITY_TIME_CRITICAL; break;
    default:
      assert(false);	      // TODO throw invalid argument exception
      return;
  }

  ::SetThreadPriority(m_handle, nPriority);
}

void Thread::enqueueMessage(const Message& message)
{
  MSG const* msg = (MSG const*)message;
  ::PostThreadMessage(m_id, msg->message, msg->wParam, msg->lParam);
}

/// Does the message loop while there are
/// visible @link Vaca::Frame frames@endlink.
/// 
/// @see Frame::setVisible
/// 
void Thread::doMessageLoop()
{
  // message loop
  Message msg;
  while (getMessage(msg))
    processMessage(msg);
}

/// Does the message loop until the @a widget is hidden.
/// 
void Thread::doMessageLoopFor(Widget* widget)
{
  // get widget HWND
  HWND hwnd = widget->getHandle();
  assert(::IsWindow(hwnd));

  // get parent HWND
  HWND hparent = widget->getParentHandle();

  // disable the parent HWND
  if (hparent != NULL)
    ::EnableWindow(hparent, FALSE);

  // message loop
  Message message;
  while (widget->isVisible() && getMessage(message))
    processMessage(message);

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

void Thread::yield()
{
  ::Sleep(0);
}

void Thread::sleep(int msecs)
{
  ::Sleep(msecs);
}

/// Gets a message waiting for it: locks the execution of the program
/// until a message is received from the operating system.
/// 
/// @return 
///   True if the @a message parameter was filled (because a message was received)
///   or false if there aren't more visible @link Frame frames@endlink
///   to dispatch messages.
/// 
bool Thread::getMessage(Message& message)
{
  ThreadData* data = getThreadData();

  // break this loop? (explicit break or no-more visible frames)
  if (data->breakLoop || data->frames.empty())
    return false;

  // we have to update indicators?
  if (data->updateIndicators &&
      data->updateIndicatorsMark.elapsed() > 0.1) {
    data->updateIndicators = false;

    // for each registered frame we should call updateIndicators to
    // update the state of all visible indicators (like top-level
    // items in the menu-bar and buttons in the tool-bar)
    for (std::vector<Frame*>::iterator
	   it = data->frames.begin(),
	   end = data->frames.end(); it != end; ++it) {
      (*it)->updateIndicators();
    }
  }

  // get the message from the queue
  LPMSG msg = (LPMSG)message;
  msg->hwnd = NULL;
  BOOL bRet = ::GetMessage(msg, NULL, 0, 0);

  // WM_QUIT received?
  if (bRet == 0)
    return false;

  // WM_NULL message... maybe Timers or CallInNextRound
  if (msg->message == WM_NULL)
    Timer::pollTimers();

  return true;
}

/// Gets a message without waiting for it, if the queue is empty, this
/// method returns false.
/// 
/// The message is removed from the queue.
/// 
/// @return
///   Returns true if the @a msg parameter was filled with the next message
///   in the queue or false if the queue was empty.
/// 
bool Thread::peekMessage(Message& message)
{
  LPMSG msg = (LPMSG)message;
  msg->hwnd = NULL;
  return ::PeekMessage(msg, NULL, 0, 0, PM_REMOVE) != FALSE;
}

void Thread::processMessage(Message& message)
{
  LPMSG msg = (LPMSG)message;
  
  if (!preTranslateMessage(message)) {
    // Send preTranslateMessage to the active window (useful for
    // modeless dialogs). WARNING: Don't use GetForegroundWindow
    // because it returns windows from other applications
    HWND hactive = GetActiveWindow();
    if (hactive != NULL && hactive != msg->hwnd) {
      Widget* activeWidget = Widget::fromHandle(hactive);
      if (activeWidget != NULL && activeWidget->preTranslateMessage(message))
	return;
    }

    //if (!TranslateAccelerator(msg->hwnd, hAccelTable, msg)) 
    //{
    ::TranslateMessage(msg);
    ::DispatchMessage(msg);
    //}
  }
}

/// Pretranslates the message. The main function is to retrieve the
/// Widget pointer (using Widget::fromHandle()) and then (if it isn't
/// NULL), call its Widget#preTranslateMessage.
/// 
bool Thread::preTranslateMessage(Message& message)
{
  LPMSG msg = (LPMSG)message;

  // TODO process messages that produce a update-indicators event
  if ((msg->message == WM_ACTIVATE) ||
      (msg->message == WM_CLOSE) ||
      (msg->message == WM_SETFOCUS) ||
      (msg->message == WM_KILLFOCUS) ||
      (msg->message >= WM_LBUTTONDOWN && msg->message <= WM_MBUTTONDBLCLK) ||
      (msg->message >= WM_KEYDOWN && msg->message <= WM_DEADCHAR)) {
    ThreadData* data = getThreadData();
    data->updateIndicators = true;
    data->updateIndicatorsMark = TimePoint();
  }

  if (msg->hwnd != NULL) {
    Widget* widget = Widget::fromHandle(msg->hwnd);
    if (widget && widget->preTranslateMessage(message))
      return true;
  }

  return false;
}

void Thread::addFrame(Frame* frame)
{
  getThreadData()->frames.push_back(frame);
}

void Thread::removeFrame(Frame* frame)
{
  remove_from_container(getThreadData()->frames, frame);

  // when this thread doesn't have more Frames to continue we must to
  // break the current message loop
  if (getThreadData()->frames.empty())
    Thread::breakMessageLoop();
}
