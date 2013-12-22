// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TIMER_H
#define VACA_TIMER_H

#include "Vaca/base.h"
#include "Vaca/Signal.h"
#include "Vaca/NonCopyable.h"
#include "Vaca/Thread.h"

namespace Vaca {

/**
   Class to schedule events every @e x milliseconds.

   @warning
     The Tick event is generated in the same thread which was
     created the Timer.

   @win32
     It doesn't use @msdn{WM_TIMER} message. In Vaca all timers
     are controlled in a separated thread for this specific purpose.
   @endwin32
*/
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

  // Signals
  Signal0<void> Tick;   ///< @see onTick

protected:

  // Events
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
