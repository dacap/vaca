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

#ifndef VACA_THREAD_H
#define VACA_THREAD_H

#include "Vaca/base.h"

#include <boost/noncopyable.hpp>

namespace Vaca {

class Widget;

/**
 * A thread of execution. You should derivate this class to create
 * your own thread. Remember this: all threads begin suspended, so you
 * must to call the execute() method to initialize them.
 *
 * @warning If you derivate this class, you shouldn't create any
 * widget (Widgets, Frames, etc.) in the constructor (or as its
 * members), because the constructor is executed in the parent thread,
 * the new thread execution begin in the run() method:
 *
 * @code
 * class MyThread : public Thread
 * {
 *   Frame frameOwnedByTheThreadThatCreatesThisInstance;
 *
 * public:
 *
 *   MyThread()
 *     : frameOwnedByTheThreadThatCreatesThisInstance(...)
 *   {
 *     // here we are in the parent thread, not in the new one
 *     ...
 *   }
 *
 *   virtual void run()		// here we are in the new thread
 *   {
 *     Frame frameOwnedByTheNewThread(...);
 *     frameOwnedByTheNewThread.setVisible(true);
 *
 *     doMessageLoop();
 *   }
 * };
 * @endcode
 */
class VACA_DLL Thread : private boost::noncopyable
{
  bool mJoinable;
  HANDLE mHANDLE;
  DWORD mId;
  int mFrameCount;

public:

  Thread(bool useCurrent = false);
  virtual ~Thread();

  int getId();

  void execute();
  void suspend();
  void resume();
  void join();

  void setPriority(int priority);

  /**
   * Override this routine to do what you want in the new thread of
   * execution.
   */
  virtual void run() = 0;

  static Thread *getCurrent();
  static int getCurrentId();

  //////////////////////////////////////////////////////////////////////
  // For threads with message queue

  virtual void postQuitMessage();

  virtual void doMessageLoop();
  virtual void doMessageLoopFor(Widget *widget);

  virtual void pumpMessageQueue();

  bool hasFrames();
  void addFrame();
  void removeFrame();

protected:

  virtual bool getMessage(MSG &msg);
  virtual bool peekMessage(MSG &msg);
  virtual void processMessage(MSG &msg);
  virtual bool preTranslateMessage(MSG &msg);

};

} // namespace Vaca

#endif
