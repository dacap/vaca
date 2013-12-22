// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CONDITIONVARIABLE_H
#define VACA_CONDITIONVARIABLE_H

#include "Vaca/base.h"
#include "Vaca/NonCopyable.h"
#include "Vaca/Exception.h"

namespace Vaca {

/**
   This exception is thrown when a ConditionVariable couldn't be created.
*/
class CreateConditionVariableException : public Exception
{
public:

  CreateConditionVariableException() : Exception() { }
  CreateConditionVariableException(const String& message) : Exception(message) { }
  virtual ~CreateConditionVariableException() throw() { }

};

class VACA_DLL ConditionVariable : private NonCopyable
{
  HANDLE m_gate;
  HANDLE m_queue;
  HANDLE m_mutex;
  unsigned m_gone;         // # threads that timed out and never made it to m_queue
  unsigned long m_blocked; // # threads blocked on the condition
  unsigned m_waiting;      // # threads no longer waiting for the condition but
			   // still waiting to be removed from m_queue
public:

  ConditionVariable();
  ~ConditionVariable();

  void notifyOne();
  void notifyAll();
  void wait(ScopedLock& lock);
  bool waitFor(ScopedLock& lock, double seconds);

  template<typename Predicate>
  void wait(ScopedLock& lock, Predicate pred) {
    while (!pred())
      wait(lock);
  }

  template<typename Predicate>
  void waitFor(ScopedLock& lock, double seconds, Predicate pred) {
    while (!pred())
      if (!waitFor(lock, seconds))
	return false;
    return true;
  }

private:
  void enterWait();

};

} // namespace Vaca

#endif // VACA_CONDITIONVARIABLE_H
