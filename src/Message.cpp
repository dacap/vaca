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

#include "Vaca/Message.h"
#include "Vaca/String.h"

using namespace Vaca;

Message::Message()
{
}

/**
   Creates a custom message that can be sent to a specific Widget.
   
   Messages can be sent through widgets of differents threads. To catch
   a customized message you have to overload the Widget#preTranslateMessage
   method.

   @see Widget#enqueueMessage, Widget#preTranslateMessage,
	@ref page_tn_015, @link page_examples Threads example@endlink
*/
Message::Message(const String& name)
{
  UINT message = ::RegisterWindowMessage(name.c_str());
  if (message == 0 || message < 0xC000 || message > 0xFFFF)
    throw MessageException(format_string(L"Error registering type of message '%s'.", name.c_str()));

  m_msg.hwnd = NULL;
  m_msg.message = message;
  m_msg.wParam = 0;
  m_msg.lParam = 0;
  m_msg.time = 0;
}

/**
   Creates a copy of the specified message with the specified user
   data as payload.

   @param msg
     A custom message previously created with Message(const String&) constructor.

   @param payload
     User data that can be obtained later by the destination using
     Message#getPayload method.

   @see Message#Message(const String&)
*/
Message::Message(const Message& msg, void* payload)
{
  m_msg.hwnd = msg.m_msg.hwnd;
  m_msg.message = msg.m_msg.message;
  m_msg.wParam = NULL;
  m_msg.lParam = reinterpret_cast<LPARAM>(payload);
  m_msg.time = 0;
}

/**
   Destroys the event.

   Does nothing.
*/
Message::~Message()
{
}

/**
   Returns the user data specified in the creation of this message.

   @win32
   The payload is saved in the lParam field of the @msdn{MSG} structure.
   @endwin32

   @see Message#Message(const Message&, void*)
*/
void* Message::getPayload()
{
  return reinterpret_cast<void*>(m_msg.lParam);
}

