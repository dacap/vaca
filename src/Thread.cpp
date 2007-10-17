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

#include "stdvaca.h"
#include "Vaca/Thread.h"
#include "Vaca/Debug.h"
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/Frame.h"

#include <algorithm>

using namespace Vaca;

static Mutex activeThreadsMutex; // used to access "activeThreads"
static std::list<Thread *> activeThreads;

static DWORD WINAPI ThreadProxy(LPVOID thread)
{
  reinterpret_cast<Thread *>(thread)->run();
  return 0;
}

/**
 * @param useCurrent true means that you don't want to create a new
 * thread, just use the current thread information.
 */
Thread::Thread(bool useCurrent)
{
  mFrameCount = 0;

  if (useCurrent) {
    mJoinable = false;
    mHANDLE = GetCurrentThread();
    mId = GetCurrentThreadId();
  }
  else {
    mJoinable = true;
    mHANDLE = CreateThread(NULL, 0,
			    ThreadProxy,
			    reinterpret_cast<void *>(this),
			    CREATE_SUSPENDED, &mId);

    VACA_TRACE("new Thread (%p, %d) {\n", mHANDLE, mId);
  }

  activeThreadsMutex.lock();
  activeThreads.push_back(this);
  activeThreadsMutex.unlock();
}

Thread::~Thread()
{
  if (mJoinable && mHANDLE != NULL) {
    CloseHandle(reinterpret_cast<HANDLE>(mHANDLE));
    mHANDLE = NULL;

    VACA_TRACE("} delete Thread (%p, %d)\n", this, mId);
  }

  activeThreadsMutex.lock();
  remove_element_from_container(activeThreads, this);
  activeThreadsMutex.unlock();
}

/**
 * Returns the thread ID.
 */
int Thread::getId()
{
  return mId;
}

/**
 * Starts the execution of the thread. You must to call this routine
 * one time to start the thread execution (the call to the run()
 * method).
 */
void Thread::execute()
{
  assert(mJoinable != false);

  resume();
}

/**
 * Suspends a thread in execution (Win32 SuspendThread).
 */
void Thread::suspend()
{
  assert(mHANDLE != NULL);

  SuspendThread(mHANDLE);
}

/**
 * Resumes the suspended thread (Win32 ResumeThread).
 */
void Thread::resume()
{
  assert(mHANDLE != NULL);

  ResumeThread(mHANDLE);
}

/**
 * Waits the thread to finish (returns from run() method), and them
 * closes it.
 */
void Thread::join()
{
  assert(mHANDLE != NULL);
  assert(mJoinable != false);

  WaitForSingleObject(reinterpret_cast<HANDLE>(mHANDLE), INFINITE);
  CloseHandle(reinterpret_cast<HANDLE>(mHANDLE));
  mHANDLE = NULL;

  VACA_TRACE("} join Thread (%p, %d)\n", this, mId);
}

/**
 * @param priority Can be one of the following values:
 * @li THREAD_PRIORITY_ABOVE_NORMAL
 * @li THREAD_PRIORITY_BELOW_NORMAL
 * @li THREAD_PRIORITY_HIGHEST
 * @li THREAD_PRIORITY_IDLE
 * @li THREAD_PRIORITY_LOWEST
 * @li THREAD_PRIORITY_NORMAL
 * @li THREAD_PRIORITY_TIME_CRITICAL 
 */
void Thread::setPriority(int priority)
{
  assert(mHANDLE != NULL);

  SetThreadPriority(mHANDLE, priority);
}

/**
 * Returns a pointer of the current thread. It's used by
 * Dialog::doModal() to call the doMessageLoopFor().
 */
Thread *Thread::getCurrent()
{
  ScopedLock lock(activeThreadsMutex);
  int threadId = Thread::getCurrentId();

  for (std::list<Thread *>::iterator it=activeThreads.begin();
       it!=activeThreads.end();
       ++it)
    if ((*it)->getId() == threadId)
      return *it;

  // impossible!!!
  assert(false);
  return NULL;
}

/**
 * Returns the current thread's ID.
 *
 * @return ID of the thread where we are.
 */
int Thread::getCurrentId()
{
  return GetCurrentThreadId();
}

/**
 * Sends a WM_QUIT message to the queue of this thread. In practice,
 * this method breaks doMessageLoop() or doMessageLoopFor().
 */
void Thread::postQuitMessage()
{
  PostThreadMessage(getId(), WM_QUIT, 0, 0);
}

/**
 * Does the message loop until the first quit message is received. A
 * quit message (PostQuitMessage) is generated when a Frame that isn't
 * an MdiChild is destroyed.
 */
void Thread::doMessageLoop()
{
  assert(getId() == Thread::getCurrentId());

  // there are Frames in this thread?
//   if (Frame::getVisibleFramesByThread(getId()) == 0)
  if (!hasFrames())
    return;

  // message loop
  MSG msg;
  while (getMessage(msg))
    processMessage(msg);
}

/**
 * Does the message loop until the @a widget is hidden.
 */
void Thread::doMessageLoopFor(Widget *widget)
{
  assert(getId() == Thread::getCurrentId());

  // get widget HWND
  HWND hwnd = widget->getHWND();
  assert(hwnd != NULL);

  // get parent HWND
  HWND hparent = widget->getParentHWND();

  // disable the parent HWND
  if (hparent != NULL)
    ::EnableWindow(hparent, FALSE);

  // message loop
  MSG msg;
  while (::IsWindowVisible(hwnd) &&
	 getMessage(msg))
    processMessage(msg);

  // enable the parent HWND
  if (hparent)
    ::EnableWindow(hparent, TRUE);
}

void Thread::pumpMessageQueue()
{
  MSG msg;
  while (peekMessage(msg))
    processMessage(msg);
}

bool Thread::getMessage(MSG &msg)
{
  msg.hwnd = NULL;
  BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);

  // done
  if (bRet == 0) {
    return false;
  }
  // error
  else if (bRet == -1) {
    /// TODO check the error
  }

  return true;
}

bool Thread::peekMessage(MSG &msg)
{
  msg.hwnd = NULL;
  return ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE;
}

void Thread::processMessage(MSG &msg)
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
bool Thread::preTranslateMessage(MSG &msg)
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

bool Thread::hasFrames()
{
  return mFrameCount > 0;
}

void Thread::addFrame()
{
  ++mFrameCount;
}

void Thread::removeFrame()
{
  --mFrameCount;
}

