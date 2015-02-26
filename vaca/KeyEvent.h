// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_KEYEVENT_H
#define VACA_KEYEVENT_H

#include "vaca/base.h"
#include "vaca/ConsumableEvent.h"
#include "vaca/Keys.h"

namespace vaca {

/**
   Data for an event that comes from the keyboard.
*/
class VACA_DLL KeyEvent : public ConsumableEvent
{
  /**
     Virtual-key code.

     @see #getKeyCode, Keys
  */
  int m_keys;

  /**
     Character-key code.

     @see #getCharCode
  */
  Char m_charCode;

public:

  KeyEvent(Widget* source, Keys::Type keys, Char charCode);
  virtual ~KeyEvent();

  Keys::Type getKeyCode() const;
  Keys::Type getModifiers() const;
  Char getCharCode() const;

  bool isShift() const;
  bool isControl() const;
  bool isAlt() const;

};

} // namespace vaca

#endif // VACA_KEYEVENT_H
