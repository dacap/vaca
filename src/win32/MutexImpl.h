// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Vaca::Mutex::MutexImpl
{
  CRITICAL_SECTION m_handle;

public:

  MutexImpl()
  {
    InitializeCriticalSection(&m_handle);
  }

  ~MutexImpl()
  {
    DeleteCriticalSection(&m_handle);
  }

  void lock()
  {
    EnterCriticalSection(&m_handle);
  }

  bool tryLock()
  {
#if(_WIN32_WINNT >= 0x0400)
    return TryEnterCriticalSection(&m_handle) ? true: false;
#else
    #error Vaca does not support the target platform
#endif
  }

  void unlock()
  {
    LeaveCriticalSection(&m_handle);
  }

};
