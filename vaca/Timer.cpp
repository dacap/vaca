// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifdef VACA_WINDOWS
  #include <windows.h>
#endif

#include "vaca/Timer.h"
#include "vaca/Thread.h"
#include "vaca/Debug.h"
#include "vaca/Mutex.h"
#include "vaca/ScopedLock.h"
#include "vaca/TimePoint.h"
#include "vaca/ConditionVariable.h"

#include <limits>
#include <limits.h>		// because MSVC

using namespace vaca;

static Mutex               timer_mutex;		// monitor
static Thread*             timer_thread = NULL; // the thread that process timers
static std::vector<Timer*> timers;              // list of timers to be processed
static bool                timer_break = false; // break the loop in timer_thread_proc()
static ConditionVariable   wakeup_condition;    // wake-up the timer thread loop

/**
   @param interval In milliseconds.
*/
Timer::Timer(int interval)
  : m_threadOwnerId(::GetCurrentThreadId())
  , m_running(false)
  , m_interval(interval)
  , m_timeCounter(0)
  , m_tickCounter(0)
{
  assert(interval > 0);
}

Timer::~Timer()
{
  stop();
}

/**
   Returns the period of time which is waited to generate tick events.

   @see #onTick
*/
int Timer::getInterval()
{
  return m_interval;
}

/**
   Sets the period of time to wait to reach each tick.
*/
void Timer::setInterval(int interval)
{
  assert(interval > 0);

  bool running = isRunning();
  if (running) stop();

  m_interval = interval;

  if (running) start();
}

/**
   Returns true if the timer is running (generating ticks).
*/
bool Timer::isRunning()
{
  return m_running;
}

/**
   Starts the ticks generation.

   You can use this member function even when timer is running (to restart it).
*/
void Timer::start()
{
  // start/create timer thread (if this is the first Timer started)
  Timer::start_timer_thread();

  {
    ScopedLock hold(timer_mutex);

    if (!m_running) {
      // add timer
      timers.push_back(this);
      m_running = true;
    }

    m_firstTick = true;
    m_timeCounter = m_interval;
    m_tickCounter = 0;

    // wake up timer thread
    wakeup_condition.notifyOne();
  }
}

/**
   Stops the timer if it's running, in other case, it does nothing.
*/
void Timer::stop()
{
  if (m_running) {
    Timer::remove_timer(this);

    m_running = false;
    m_timeCounter = 0;
    m_tickCounter = 0;
  }
}

/**
   Polls all timers for the current thread.

   Fires all events for each Timer that belong to the current thread.
   It's used from Thread#getMessage when process the WM_NULL message,
   but it's safe to call this routine from anywhere (for example, in a
   do-while block).
*/
void Timer::pollTimers()
{
  Timer::fire_timers_for_thread();
}

/**
   When the timer is running, this event is generated for each tick.
*/
void Timer::onTick()
{
  // fire the signal
  Tick();
}

// ======================================================================

/**
   Thread to control Timers. This thread is created when the first
   Timer is started. Then it continues running until ~Application stop
   it.

   @internal
*/
void Timer::run_timer_thread()
{
  ScopedLock hold(timer_mutex);
  unsigned int start, end, period, delay;
  unsigned int inf = UINT_MAX; // std::numeric_limits<unsigned int>::max();
  std::vector<Timer*>::iterator it;
  Timer* timer;

  // is it needed?
  // ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

  start = timeGetTime();

  while (!timer_break) {
    // yield
    ::Sleep(0);

    end = timeGetTime();
    period = end-start;    // how much time pass from the last event
    start = end;

    // "delay" will contain the minimum account of time to reach
    // the next Timer's event
    delay = inf;

    // threads to send a NULL message to wake up
    std::vector<int> threads;

    // timers loop
    for (it = timers.begin(); it != timers.end(); ++it) {
      timer = *it;

      // decrement "mTimeCounter" to known that we pass a "period"
      // amount of time
      if (!timer->m_firstTick)
	timer->m_timeCounter -= period;
      else
	timer->m_firstTick = false;

      // if we accomplish one (or more) interval(s) of this timer...
      if (timer->m_timeCounter <= 0) {
	// ...we must to generate ticks for each time that we
	// accomplish the time interval
	while (timer->m_timeCounter <= 0) {
	  // generate one tick for each "m_interval" period of time
	  timer->m_tickCounter++;
	  timer->m_timeCounter += timer->m_interval;
	}

	// thread owner ID
	int id = timer->m_threadOwnerId;

	// wake up message queue of the thread which creates this
	// timer (to process through Timer::pollTimers() all
	// ticks of this timer from its thread)
	if (std::find(threads.begin(), threads.end(), id) == threads.end()) {
	  threads.push_back(id);
	  ::PostThreadMessage(id, WM_NULL, 0, 0);
	}
      }

      // maybe the "delay" is too big to reach the next event
      if (delay == inf ||
	  delay > static_cast<unsigned int>(timer->m_timeCounter))
	delay = static_cast<unsigned int>(timer->m_timeCounter);
    }

    // wait wake-up condition or the "delay" to process the next timer-event
    if (delay == inf)
      wakeup_condition.wait(hold);
    else
      wakeup_condition.waitFor(hold, delay / 1000.0);
  }
}

/**
   @internal
*/
void Timer::start_timer_thread()
{
  ScopedLock hold(timer_mutex);

  if (timer_thread == NULL)
    timer_thread = new Thread(&run_timer_thread);
}

/**
   Stops the timer_thread (called from ~Application()).

   @internal
*/
void Timer::stop_timer_thread()
{
  Thread* thrd = NULL;

  {
    ScopedLock hold(timer_mutex);

    if (timer_thread == NULL)
      return;

    thrd = timer_thread;
    timer_thread = NULL;

    timer_break = true;
    wakeup_condition.notifyOne();
  }

  thrd->join();

  delete thrd;
  thrd = NULL;
}

/**
   @internal
*/
void Timer::remove_timer(Timer* t)
{
  ScopedLock hold(timer_mutex);

  remove_from_container(timers, t);
}

/**
   @internal
*/
void Timer::fire_timers_for_thread()
{
  ThreadId currentThreadId = ::GetCurrentThreadId();
  std::vector<Timer*> timers_for_thread;
  std::vector<Timer*>::iterator it;
  Timer* timer;

  // make a copy of timers for this thread only
  {
    ScopedLock hold(timer_mutex);

    for (it = timers.begin(); it != timers.end(); ) {
      timer = *(it++);

      // is it for this thread?
      if (timer->m_threadOwnerId == currentThreadId)
	timers_for_thread.push_back(timer);
    }
  }

  // iterate timers for this thread
  for (it = timers_for_thread.begin();
       it != timers_for_thread.end(); ) {
    timer = *(it++);

    TimePoint warning_time;
    double timeout = timer->m_interval / 1000.0;

    // for each accumulated tick
    while (timer->m_tickCounter > 0) {
      timer->m_tickCounter--;

      // fire event
      timer->onTick();

      // warning! if this is taking to long, we have to force a break
      // of the loop discarding the rest of ticks (m_tickCounter)
      if (warning_time.elapsed() > timeout)
	timer->m_tickCounter = 0;
    }
  }

  CurrentThread::yield();
}
