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

#ifndef VACA_TIMER_H
#define VACA_TIMER_H

#include "Vaca/base.h"
#include "Vaca/Signal.h"
#include "Vaca/NonCopyable.h"
#include "Vaca/Thread.h"

namespace Vaca {

/// Class to schedule events every @e x milliseconds.
/// 
/// @warning
///   The Tick event is generated in the same thread which was
///   created the Timer.
/// 
/// @win32
///   It doesn't use @msdn{WM_TIMER} message. In Vaca all timers
///   are controlled in a separated thread for this specific purpose.
/// @endwin32
/// 
class VACA_DLL Timer : private NonCopyable
{
  friend class Application;

  ThreadId m_threadOwnerId;
  bool m_running : 1;
  bool m_firstTick : 1;
  int m_interval;
  int m_timeCounter;
  int m_tickCounter;

public:

  Timer(int interval);
  virtual ~Timer();

  int getInterval();
  void setInterval(int interval);

  bool isRunning();

  void start();
  void stop();

  static void pollTimers();

  // signals
  Signal0<void> Tick;   ///< @see onTick

protected:

  // events
  virtual void onTick();

private:

  static void run_timer_thread();
  static void start_timer_thread();
  static void stop_timer_thread();
  static void remove_timer(Timer* t);
  static void fire_timers_for_thread();
  
};

} // namespace Vaca

#endif // VACA_TIMER_H
