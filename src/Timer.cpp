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
#include "Vaca/Timer.hpp"
#include "Vaca/Thread.hpp"
#include "Vaca/Debug.hpp"

#include <boost/timer.hpp>

using namespace Vaca;

static boost::mutex        timer_mutex;		// monitor
static boost::thread*      timer_thread = NULL; // the thread that process timers
static std::vector<Timer*> timers;              // list of timers to be processed
static bool                timer_break = false; // break the loop in timer_thread_proc()
static boost::condition    wakeup_condition;    // wake-up the timer thread loop

// returns current time plus "msecs" milliseconds, to be used in
// Boost.Threads waiting routines
static inline boost::xtime xt_delay(int msecs)
{
  const int MILLISECONDS_PER_SECOND = 1000;
  const int NANOSECONDS_PER_SECOND = 1000000000;
  const int NANOSECONDS_PER_MILLISECOND = 1000000;

  // convert millisecconds to seconds/nanoseconds
  int d_secs = msecs / MILLISECONDS_PER_SECOND;
  int d_nsecs = (msecs % MILLISECONDS_PER_SECOND) * NANOSECONDS_PER_MILLISECOND;

  boost::xtime xt;
  boost::xtime_get(&xt, boost::TIME_UTC);

  // apply delta-seconds/nanoseconds
  xt.nsec = (xt.nsec+d_nsecs) % NANOSECONDS_PER_SECOND;
  xt.sec = xt.sec+d_secs + (d_nsecs / NANOSECONDS_PER_SECOND);

  return xt;
}

/**
 * @param interval In milliseconds.
 * 
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

int Timer::getInterval()
{
  return m_interval;
}

/**
 * Sets the period of time to wait to reach each tick.
 * 
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
 * Returns true if the timer is running (generating ticks).
 * 
 */
bool Timer::isRunning()
{
  return m_running;
}

/**
 * Starts the ticks generation. You can use this method even when
 * timer is running (to restart it).
 */
void Timer::start()
{
  // start/create timer thread (if this is the first Timer started)
  Timer::start_timer_thread();

  {
    boost::mutex::scoped_lock lock(timer_mutex);

    if (!m_running) {
      // add timer
      timers.push_back(this);
      m_running = true;
    }

    m_firstTick = true;
    m_timeCounter = m_interval;
    m_tickCounter = 0;

    // wake up timer thread
    wakeup_condition.notify_one();
  }
}

/**
 * Stops the timer if it's running, in other case, it does nothing.
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
 * Polls all timers for the current thread. Fires all actions for each
 * Timer that belong to the current thread.  It's used from
 * Thread::getMessage when process the WM_NULL message, but it's safe
 * to call this routine from anywhere (for example, in a do-while
 * block).
 * 
 */
void Timer::pollTimers()
{
  Timer::fire_actions_for_thread();
}

/**
 * When the timer is running, this event is generated for each tick.
 */
void Timer::onAction()
{
  // fire the signal
  Action();
}

//////////////////////////////////////////////////////////////////////

/**
 * Thread to control Timers. This thread is created when the first
 * Timer is started. Then it continues running until ~Application stop
 * it.
 *
 * @internal 
 */
void Timer::run_timer_thread()
{
  boost::mutex::scoped_lock lock(timer_mutex);
  unsigned int start, end, period, delay;
  unsigned int inf = UINT_MAX;//std::numeric_limits<unsigned int>::max();
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
      
    // wait wake-up condition or the "delay" to process the next timer-action
    if (delay == inf)
      wakeup_condition.wait(lock);
    else
      wakeup_condition.timed_wait(lock, xt_delay(delay));
  }
}

/**
 * @internal
 */
void Timer::start_timer_thread()
{
  boost::mutex::scoped_lock lock(timer_mutex);

  if (timer_thread == NULL)
    timer_thread = new boost::thread(&run_timer_thread);
}

/**
 * Stops the timer_thread (called from ~Application()).
 * 
 * @internal
 */
void Timer::stop_timer_thread()
{
  boost::thread* thrd;
  
  {
    boost::mutex::scoped_lock lock(timer_mutex);

    if (timer_thread == NULL)
      return;

    thrd = timer_thread;
    timer_thread = NULL;

    timer_break = true;
    wakeup_condition.notify_one();
  }

  thrd->join();

  delete thrd;
  thrd = NULL;
}

/**
 * @internal
 */
void Timer::remove_timer(Timer* t)
{
  boost::mutex::scoped_lock lock(timer_mutex);

  remove_element_from_container(timers, t);
}

/**
 * @internal
 */
void Timer::fire_actions_for_thread()
{
  int currentThreadId = ::GetCurrentThreadId();
  std::vector<Timer*> timers_for_thread;
  std::vector<Timer*>::iterator it;
  Timer* timer;

  // make a copy of timers for this thread only
  {
    boost::mutex::scoped_lock lock(timer_mutex);

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

    boost::timer warning_time;
    double timeout = timer->m_interval / 1000.0;

    while (timer->m_tickCounter > 0) {
      timer->m_tickCounter--;

      // fire action
      timer->onAction();

      if (warning_time.elapsed() > timeout)
	timer->m_tickCounter = 0;
    }
  }

  // yield
  ::Sleep(0);
}
