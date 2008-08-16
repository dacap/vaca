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

#include "Vaca/KeyEvent.h"
#include "Vaca/Widget.h"

using namespace Vaca;

KeyEvent::KeyEvent(Widget* source, Keys::Type keys, Character charCode)
  : Event(source)
  , m_consumed(false)
  , m_keys(keys)
  , m_charCode(charCode)
{
}

KeyEvent::~KeyEvent()
{
}

void KeyEvent::consume()
{
  m_consumed = true;
}

bool KeyEvent::isConsumed() const
{
  return m_consumed;
}

Keys::Type KeyEvent::getKeyCode() const
{
  return m_keys & Keys::KeyCode;
}

Keys::Type KeyEvent::getModifiers() const
{
  return m_keys & Keys::Modifiers;
}

Character KeyEvent::getCharCode() const
{
  return m_charCode;
}

bool KeyEvent::isShift() const
{
  return (m_keys & Keys::Shift) != 0;
}

bool KeyEvent::isControl() const
{
  return (m_keys & Keys::Control) != 0;
}

bool KeyEvent::isAlt() const
{
  return (m_keys & Keys::Alt) != 0;
}
