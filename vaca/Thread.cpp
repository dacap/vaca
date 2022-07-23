// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Thread.h"
#include "vaca/Debug.h"
#include "vaca/Frame.h"
#include "vaca/Signal.h"
#include "vaca/Timer.h"
#include "vaca/Mutex.h"
#include "vaca/ScopedLock.h"
#include "vaca/Slot.h"
#include "vaca/TimePoint.h"

#include <algorithm>
#include <memory>
#include <vector>

using namespace vaca;

// ======================================================================

// TODO
// - replace with some C++0x's Thread-Local Storage
// - use __thread in GCC, and __declspec( thread ) in MSVC
// - use TlsAlloc

struct ThreadData
{
  /**
     The ID of this thread.
  */
  ThreadId threadId;

  /**
     Visible frames in this thread. A frame is an instance of Frame
     class.
  */
  std::vector<Frame*> frames;

  TimePoint updateIndicatorsMark;
  bool updateIndicators : 1;

  /**
     True if the message-loop must be stopped.
  */
  bool breakLoop : 1;

  /**
     Widget used to call createHandle.
  */
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

static ThreadData* get_thread_data()
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

// ======================================================================

static DWORD WINAPI ThreadProxy(LPVOID data)
{
  // Force the creation of a message queue in this new thread...
  {
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
  }

  std::unique_ptr<std::function<void()>> fptr(
    reinterpret_cast<std::function<void()>*>(data));
  (*fptr)();
  return 0;
}

Thread::Thread()
{
  m_handle = ::GetCurrentThread();
  m_id = ::GetCurrentThreadId();

  VACA_TRACE("current Thread (%p, %d)\n", this, m_id);
}

/**
   Creates a new thread to execute the specified slot in that thread.

   @throw CreateThreadException
     If the thread could not be created.

   @internal
*/
void Thread::_Thread(std::function<void()>&& f)
{
  auto fptr = std::make_unique<std::function<void()>>(std::move(f));
  DWORD id;

  m_handle = CreateThread(NULL, 0,
			  ThreadProxy, fptr.get(),
			  CREATE_SUSPENDED, &id);
  if (!m_handle)
    throw CreateThreadException();

  fptr.release();
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

/**
   Returns the thread ID.

   @win32
     This is equal to @msdn{GetCurrentThreadId} for the current
     thread or the ID returned by @msdn{CreateThread}.
   @endwin32
*/
ThreadId Thread::getId() const
{
  return m_id;
}

/**
   Waits the thread to finish, and them closes it.
*/
void Thread::join()
{
  assert(m_handle != NULL);
  assert(isJoinable());

  WaitForSingleObject(reinterpret_cast<HANDLE>(m_handle), INFINITE);
  CloseHandle(reinterpret_cast<HANDLE>(m_handle));
  m_handle = NULL;

  VACA_TRACE("join Thread (%p, %d)\n", this, m_id);
}

/**
   Returns true if this thread can be waited by the current thread.
*/
bool Thread::isJoinable() const
{
  return m_id != ::GetCurrentThreadId();
}

/**
   Sets the priority of the thread.

   Thread priority is relative to the other threads of the same
   process. If you want to change the priority of the entire process
   (respecting to all other processes) you have to use the
   Application#setProcessPriority member function. In other words,
   you should use this member function only if you have more than one thread in
   your application and you want to make run faster one thread than other.

   @param priority
     Can be one of the following values:
     One of the following values:
     @li ThreadPriority::Idle
     @li ThreadPriority::Lowest
     @li ThreadPriority::Low
     @li ThreadPriority::Normal
     @li ThreadPriority::High
     @li ThreadPriority::Highest
     @li ThreadPriority::TimeCritical

   @see Application#setProcessPriority
*/
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

  // Post the message
  while (::PostThreadMessage(m_id, msg->message, msg->wParam, msg->lParam) == 0) {
    // ...If the routine fail is because the thread does not have a message queue yet...
    // Wait some time and retry...
    ::Sleep(10);
  }
}

// ======================================================================
// CurrentThread

ThreadId CurrentThread::getId()
{
  return ::GetCurrentThreadId();
}

void CurrentThread::enqueueMessage(const Message& message)
{
  Thread currentThread;
  currentThread.enqueueMessage(message);
}

/**
   Does the message loop while there are
   visible @link vaca::Frame frames@endlink.

   @see Frame::setVisible
*/
void vaca::CurrentThread::doMessageLoop()
{
  // message loop
  Message msg;
  while (getMessage(msg))
    processMessage(msg);
}

/**
   Does the message loop until the @a widget is hidden.
*/
void vaca::CurrentThread::doMessageLoopFor(Widget* widget)
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

void vaca::CurrentThread::pumpMessageQueue()
{
  Message msg;
  while (peekMessage(msg))
    processMessage(msg);
}

void vaca::CurrentThread::breakMessageLoop()
{
  get_thread_data()->breakLoop = true;
  ::PostThreadMessage(::GetCurrentThreadId(), WM_NULL, 0, 0);
}

void vaca::CurrentThread::yield()
{
  ::Sleep(0);
}

void vaca::CurrentThread::sleep(int msecs)
{
  ::Sleep(msecs);
}

/**
   Gets a message waiting for it: locks the execution of the program
   until a message is received from the operating system.

   @return
     True if the @a message parameter was filled (because a message was received)
     or false if there aren't more visible @link Frame frames@endlink
     to dispatch messages.
*/
bool vaca::CurrentThread::getMessage(Message& message)
{
  ThreadData* data = get_thread_data();

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

/**
   Gets a message without waiting for it, if the queue is empty, this
   member function returns false.

   The message is removed from the queue.

   @return
     Returns true if the @a msg parameter was filled with the next message
     in the queue or false if the queue was empty.
*/
bool CurrentThread::peekMessage(Message& message)
{
  LPMSG msg = (LPMSG)message;
  msg->hwnd = NULL;
  return ::PeekMessage(msg, NULL, 0, 0, PM_REMOVE) != FALSE;
}

void CurrentThread::processMessage(Message& message)
{
  LPMSG msg = (LPMSG)message;

  if (!CurrentThread::details::preTranslateMessage(message)) {
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

// ======================================================================
// Vaca internals

/**
   Pretranslates the message. The main function is to retrieve the
   Widget pointer (using Widget::fromHandle()) and then (if it isn't
   NULL), call its Widget#preTranslateMessage.
*/
bool CurrentThread::details::preTranslateMessage(Message& message)
{
  LPMSG msg = (LPMSG)message;

  // TODO process messages that produce a update-indicators event
  if ((msg->message == WM_ACTIVATE) ||
      (msg->message == WM_CLOSE) ||
      (msg->message == WM_SETFOCUS) ||
      (msg->message == WM_KILLFOCUS) ||
      (msg->message >= WM_LBUTTONDOWN && msg->message <= WM_MBUTTONDBLCLK) ||
      (msg->message >= WM_KEYDOWN && msg->message <= WM_DEADCHAR)) {
    ThreadData* data = get_thread_data();
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

/**
    @internal
 */
Widget* CurrentThread::details::getOutsideWidget()
{
  return get_thread_data()->outsideWidget;
}

/**
   @internal
 */
void CurrentThread::details::setOutsideWidget(Widget* widget)
{
  get_thread_data()->outsideWidget = widget;
}

/**
   @internal
 */
void CurrentThread::details::addFrame(Frame* frame)
{
  get_thread_data()->frames.push_back(frame);
}

/**
   @internal
 */
void CurrentThread::details::removeFrame(Frame* frame)
{
  remove_from_container(get_thread_data()->frames, frame);

  // when this thread doesn't have more Frames to continue we must to
  // break the current message loop
  if (get_thread_data()->frames.empty())
    CurrentThread::breakMessageLoop();
}

void details::removeAllThreadData()
{
  ScopedLock hold(data_mutex);
  std::vector<ThreadData*>::iterator it, end = dataOfEachThread.end();

  for (it=dataOfEachThread.begin(); it!=end; ++it) {
    VACA_TRACE("delete data-thread %d\n", (*it)->threadId);
    delete *it;
  }

  dataOfEachThread.clear();
}
