// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#ifndef VACA_CONDITION_HPP
#define VACA_CONDITION_HPP

#include "Vaca/base.hpp"
#include "Vaca/NonCopyable.hpp"
#include "Vaca/Exception.hpp"

namespace Vaca {

class ScopedLock;

class CreateConditionException : public Exception
{
public:

  CreateConditionException() : Exception() { }
  CreateConditionException(const String& message) : Exception(message) { }
  virtual ~CreateConditionException() throw() { }

};

class VACA_DLL Condition : private NonCopyable
{
  void* m_gate;
  void* m_queue;
  void* m_mutex;
  unsigned m_gone;         // # threads that timed out and never made it to m_queue
  unsigned long m_blocked; // # threads blocked on the condition
  unsigned m_waiting;      // # threads no longer waiting for the condition but
			   // still waiting to be removed from m_queue
public:

  Condition();
  ~Condition();

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

#endif
