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

#ifndef VACA_MESSAGE_H
#define VACA_MESSAGE_H

#include "Vaca/base.h"
#include "Vaca/NonCopyable.h"
#include "Vaca/Exception.h"

namespace Vaca {

/**
 * Thrown when a Message cannot be registered/created.
 */
class MessageException : public Exception
{
public:

  MessageException() { }
  MessageException(const String& message) : Exception(message) { }
  virtual ~MessageException() throw() { }

};

/**
 * A message that comes from the operating system.
 *
 * @win32
 *   This is a wrapper for a @msdn{MSG}.
 * @endwin32
 */
class VACA_DLL Message : private NonCopyable
{
  friend class Thread;
  friend class Widget;

  MSG m_msg;

public:

  Message();
  Message(const String& name);
  virtual ~Message();

  inline bool operator==(const Message& message) const {
    return m_msg.message == message.m_msg.message;
  }

  inline operator MSG*() { return &m_msg; }
  inline operator MSG const *() const { return &m_msg; }

};

} // namespace Vaca

#endif // VACA_MESSAGE_H
