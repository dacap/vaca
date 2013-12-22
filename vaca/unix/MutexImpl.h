// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <pthread.h>
#include <errno.h>

class Vaca::Mutex::MutexImpl
{
  pthread_mutex_t m_handle;

public:

  MutexImpl()
  {
    pthread_mutex_init(&m_handle, NULL);
  }

  ~MutexImpl()
  {
    pthread_mutex_destroy(&m_handle);
  }

  void lock()
  {
    pthread_mutex_lock(&m_handle);
  }

  bool tryLock()
  {
    return pthread_mutex_trylock(&m_handle) != EBUSY;
  }

  void unlock()
  {
    pthread_mutex_unlock(&m_handle);
  }

};

