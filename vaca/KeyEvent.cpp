// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/KeyEvent.h"
#include "vaca/Widget.h"

using namespace vaca;

KeyEvent::KeyEvent(Widget* source, Keys::Type keys, Char charCode)
  : ConsumableEvent(source)
  , m_keys(keys)
  , m_charCode(charCode)
{
}

KeyEvent::~KeyEvent()
{
}

Keys::Type KeyEvent::getKeyCode() const
{
  return m_keys & Keys::KeyCode;
}

Keys::Type KeyEvent::getModifiers() const
{
  return m_keys & Keys::Modifiers;
}

Char KeyEvent::getCharCode() const
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
