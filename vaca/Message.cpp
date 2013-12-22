// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

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
   member function.

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
     Message#getPayload member function.

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

