// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Mutex.h"

#if defined(VACA_WINDOWS)
  #include "win32/MutexImpl.h"
#else
  #include "unix/MutexImpl.h"
#endif

using namespace vaca;

/**
   Creates a new mutex.

   @win32
     It uses @msdn{InitializeCriticalSection}.
   @endwin32
*/
Mutex::Mutex()
{
  m_impl = new MutexImpl();
}

/**
   Destroys the mutex.

   @win32
     It uses @msdn{DeleteCriticalSection}.
   @endwin32
*/
Mutex::~Mutex()
{
  delete m_impl;
}

/**
   Locks the mutex to enter in a critical section.

   Locks the mutex if it is free (not locked by another thread) or
   waits the mutex to be unlocked.

   @see unlock, ScopedLock, Thread

   @win32
     It uses @msdn{EnterCriticalSection}.
   @endwin32
*/
void Mutex::lock()
{
  return m_impl->lock();
}

/**
   Tries to lock the mutex and returns true if it was locked.

   @see lock

   @win32
     It uses @msdn{TryEnterCriticalSection}.
   @endwin32
*/
bool Mutex::tryLock()
{
  return m_impl->tryLock();
}

/**
   Unlocks the mutex so another thread can lock it.

   You have to unlock a mutex to give access to another thread to
   enter in the critical section and use the shared resources that
   this mutex is guarding.

   @see lock, ScopedLock, Thread

   @win32
     It uses @msdn{LeaveCriticalSection}.
   @endwin32
*/
void Mutex::unlock()
{
  return m_impl->unlock();
}
