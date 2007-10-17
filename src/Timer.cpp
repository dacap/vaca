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
#include "Vaca/Timer.h"
#include "Vaca/Thread.h"
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/Debug.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class Vaca::_TimerThread;

static Vaca::_TimerThread *timerThread = NULL;
static Mutex timerMutex;

//////////////////////////////////////////////////////////////////////
// Thread to control Timers. This thread is created when the first
// Timer is started. Then it continues running until ~Application stop
// it.

class Vaca::_TimerThread : public Thread
{
  std::vector<Timer *> mTimers;
  HANDLE mStopEvent;
  HANDLE mWakeUpEvent;

public:

  _TimerThread()
  {
    setPriority(THREAD_PRIORITY_TIME_CRITICAL);
    mStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    mWakeUpEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  }

  virtual ~_TimerThread()
  {
    CloseHandle(mStopEvent);
    CloseHandle(mWakeUpEvent);
  }

  void stop()
  {
    SetEvent(mStopEvent);
  }

  void wakeUp()
  {
    SetEvent(mWakeUpEvent);
  }

  virtual void run()
  {
    DWORD res, start, end, period, delay;
    std::vector<Timer *>::iterator it;
    Timer *timer;
    HANDLE events[2] = { mStopEvent,  mWakeUpEvent };

    start = timeGetTime();

    while (true) {
      end = timeGetTime();
      period = end-start;    // how much time pass from the last event
      start = end;

      // "delay" will contain the minimum account of time to reach the
      // next Timer's event
      delay = INFINITE;

      // timers loop
      {
	ScopedLock lock(timerMutex);

	for (it = mTimers.begin(); it != mTimers.end(); ++it) {
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

	    // wake up message queue of the thread which creates this
	    // timer (to process through fireAllTicksForThread() all
	    // ticks of this timer from the its thread)
	    ::PostThreadMessage(timer->m_threadOwnerId, WM_NULL, 0, 0);
	  }

	  // maybe the "delay" is too big to reach the next event
	  if (delay == INFINITE ||
	      delay > static_cast<DWORD>(timer->m_timeCounter))
	    delay = timer->m_timeCounter;
	}
      }

      // wait for: "delay" period of time, or stop event, or wake up event
      res = WaitForMultipleObjects(2, events, FALSE, delay);

      // stop event
      if (res == WAIT_OBJECT_0)
	break;
      // no time-out and no wake-up?
      else if (res != WAIT_TIMEOUT && res != WAIT_OBJECT_0+1)
	break;
    }

    // done, delete the TimerThread (Timer::start() should create a
    // new instance of TimerThread to continue processing new timers)
    delete timerThread;
    timerThread = NULL;
  }

  void addTimer(Timer *timer)
  {
    ScopedLock lock(timerMutex);
    mTimers.push_back(timer);
  }

  void removeTimer(Timer *timer)
  {
    ScopedLock lock(timerMutex);
    remove_element_from_container(mTimers, timer);
  }

  void fireAllTicksForThread(int threadId)
  {
    std::vector<Timer *>::iterator it;
    std::vector<Timer *> timers;
    Timer *timer;

    // make a copy of mTimers
    {
      ScopedLock lock(timerMutex);
      timers = mTimers;
    }

    for (it = timers.begin(); it != timers.end(); ) {
      timer = *(it++);

      {
	ScopedLock lock(timerMutex);
	if (timer->m_threadOwnerId == threadId) {
	  while (timer->m_tickCounter > 0) {
	    timer->m_tickCounter--;
	    timer->onAction();
	  }
	}
      }
    }
  }

};

//////////////////////////////////////////////////////////////////////

/**
 * @param interval In milliseconds.
 * 
 */
Timer::Timer(int interval)
  : m_threadOwnerId(Thread::getCurrentId())
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
  ScopedLock lock(timerMutex);

  // create the TimerThread?
  if (timerThread == NULL) {
    timerThread = new _TimerThread();
    timerThread->execute();
  }

  if (!m_running) {
    timerThread->addTimer(this);
    m_running = true;
  }

  m_firstTick = true;
  m_timeCounter = m_interval;
  m_tickCounter = 0;

  timerThread->wakeUp();
}

/**
 * Stops the timer if it's running, in other case, it does nothing.
 */
void Timer::stop()
{
  if (m_running) {
    ScopedLock lock(timerMutex);
    timerThread->removeTimer(this);
    m_running = false;
    m_timeCounter = 0;
    m_tickCounter = 0;
  }
}

/**
 * Polls all timers for the current thread. Fires all ticks for each
 * Timer that belong to the current thread.  It's used from
 * Thread::getMessage when process the WM_NULL message, but it's safe
 * to call this routine from anywhere (for example, in a do-while
 * block).
 * 
 */
void Timer::pollTimers()
{
  if (timerThread != NULL)
    timerThread->fireAllTicksForThread(Thread::getCurrentId());
}

/**
 * When the timer is running, this event is generated for each tick.
 * 
 */
void Timer::onAction()
{
  // fire the signal
  Action();
}

/**
 * Stops the TimerThread (called from ~Application()).
 * 
 */
void Timer::finishTimerThread()
{
  if (timerThread != NULL) {
    timerThread->stop();

    while (WaitForSingleObject(timerThread, 100) == WAIT_TIMEOUT);
  }
}
