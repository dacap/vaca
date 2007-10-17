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
#include "Vaca/Mutex.h"
#include "Vaca/Debug.h"

using namespace Vaca;

/**
 * @param multiProcess @c True means it's a Win32 Mutex HANDLE, @c
 * false means it's a CRITICAL_SECTION.
 * @param mutexName Can be specified only when @a multiProcess is true. It's
 * useful to known a Mutex in multiple-processes by its name.
 */
Mutex::Mutex(bool multiProcess, LPCTSTR mutexName)
{
  mCriticalSection = !multiProcess;
  
  if (mCriticalSection) {
    // critical sections can't have a name
    VACA_ASSERT(mutexName == NULL);

    mData = new CRITICAL_SECTION;
    InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mData));
  }
  else {
    mData = CreateMutex(NULL, FALSE, mutexName);
    // if (mData == 0 || mData == INVALID_HANDLE_VALUE)
    // throw 
  }
}

Mutex::~Mutex()
{
  if (mCriticalSection) {
    DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mData));
    delete reinterpret_cast<LPCRITICAL_SECTION>(mData);
  }
  else {
    CloseHandle(mData);
  }
}

void Mutex::lock()
{
  if (mCriticalSection)
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mData));
  else
    WaitForSingleObject(mData, INFINITE);
}

void Mutex::unlock()
{
  if (mCriticalSection)
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mData));
  else
    ReleaseMutex(mData);
}
