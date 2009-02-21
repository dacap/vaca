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

#include "Vaca/Mutex.h"

using namespace Vaca;

/// Creates a new mutex.
/// 
/// @win32
///   It uses @msdn{InitializeCriticalSection}.
/// @endwin32
/// 
Mutex::Mutex()
{
  InitializeCriticalSection(&m_cs);
}

/// Destroys the mutex.
/// 
/// @win32
///   It uses @msdn{DeleteCriticalSection}.
/// @endwin32
/// 
Mutex::~Mutex()
{
  DeleteCriticalSection(&m_cs);
}

/// Locks the mutex to enter in a critical section.
/// 
/// Locks the mutex if it is free (not locked by another thread) or
/// waits the mutex to be unlocked.
/// 
/// @see unlock, ScopedLock, Thread
/// 
/// @win32
///   It uses @msdn{EnterCriticalSection}.
/// @endwin32
/// 
void Mutex::lock()
{
  EnterCriticalSection(&m_cs);
}

/// Tries to lock the mutex and returns true if it was locked.
/// 
/// @see lock
/// 
/// @win32
///   It uses @msdn{TryEnterCriticalSection}.
/// @endwin32
/// 
bool Mutex::tryLock()
{
  return TryEnterCriticalSection(&m_cs) ? true: false;
}

/// Unlocks the mutex so another thread can lock it.
/// 
/// You have to unlock a mutex to give access to another thread to
/// enter in the critical section and use the shared resources that
/// this mutex is guarding.
/// 
/// @see lock, ScopedLock, Thread
/// 
/// @win32
///   It uses @msdn{LeaveCriticalSection}.
/// @endwin32
/// 
void Mutex::unlock()
{
  LeaveCriticalSection(&m_cs);
}
