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

#ifndef VACA_THREAD_H
#define VACA_THREAD_H

#include "Vaca/base.h"
#include "Vaca/Exception.h"
#include "Vaca/Message.h"
#include "Vaca/NonCopyable.h"
#include "Vaca/Slot.h"

namespace Vaca {

// ======================================================================

/**
   It's like a namespace for ThreadPriority.

   @see ThreadPriority
*/
struct ThreadPriorityEnum
{
  enum enumeration {
    Idle,
    Lowest,
    Low,
    Normal,
    High,
    Highest,
    TimeCritical,
  };
  static const enumeration default_value = Normal;
};

/**
   Thread priority.

   One of the following values:
   @li ThreadPriority::Idle
   @li ThreadPriority::Lowest
   @li ThreadPriority::Low
   @li ThreadPriority::Normal
   @li ThreadPriority::High
   @li ThreadPriority::Highest
   @li ThreadPriority::TimeCritical
*/
typedef Enum<ThreadPriorityEnum> ThreadPriority;

// ======================================================================

/**
   This exception is thrown when a new Thread couldn't be created.
*/
class CreateThreadException : public Exception
{
public:

  CreateThreadException() : Exception() { }
  CreateThreadException(const String& message) : Exception(message) { }
  virtual ~CreateThreadException() throw() { }

};

/**
   A thread of execution.

   Each thread that has widgets has a queue of messages, each message
   represent an event that the user generated with the input devices
   or a signal from the operating system (OS). The #doMessageLoop is
   the member function that gets messages from the OS and distributes
   them to widgets.

   @see doMessageLoop
*/
class VACA_DLL Thread : public NonCopyable
{
  friend class Frame;

private:

  HANDLE m_handle;
  ThreadId m_id;

public:

  Thread();
  ~Thread();

  /**
     Creates a new thread running the specified function or functor @a f.

     @throw CreateThreadException
       If the thread couldn't be created by Win32's @c CreateThread.
  */
  template<typename F>
  explicit Thread(F f) {
    _Thread(Slot0_fun<void, F>(f));
  }

  ThreadId getId() const;

  void join();
  bool isJoinable() const;

  void setThreadPriority(ThreadPriority priority);

  // ===============================================================
  // MESSAGES
  // ===============================================================

  void enqueueMessage(const Message& message);

private:
  void _Thread(const Slot0<void>& slot);

};

namespace CurrentThread
{
  VACA_DLL ThreadId getId();

  VACA_DLL void enqueueMessage(const Message& message);

  VACA_DLL void doMessageLoop();
  VACA_DLL void doMessageLoopFor(Widget* widget);
  VACA_DLL void pumpMessageQueue();
  VACA_DLL void breakMessageLoop();

  VACA_DLL void yield();
  VACA_DLL void sleep(int msecs);

  VACA_DLL bool getMessage(Message& msg);
  VACA_DLL bool peekMessage(Message& msg);
  VACA_DLL void processMessage(Message& msg);

  namespace details {
    VACA_DLL bool preTranslateMessage(Message& message);

    VACA_DLL Widget* getOutsideWidget();
    VACA_DLL void setOutsideWidget(Widget* widget);

    VACA_DLL void addFrame(Frame* frame);
    VACA_DLL void removeFrame(Frame* frame);
  }

};

namespace details {
  VACA_DLL void removeAllThreadData();
}

} // namespace Vaca

#endif // VACA_THREAD_H
