// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

#ifndef VACA_MOUSEEVENT_H
#define VACA_MOUSEEVENT_H

#include "Vaca/base.h"
#include "Vaca/Event.h"
#include "Vaca/Point.h"

namespace Vaca {

class Widget;

struct MouseButtons {
  /**
   * Enumeration to known which mouse's button triggers the MouseEvent.
   */
  enum Type {
    None,
    Left,
    Right,
    Middle,
  };
};

/**
 * An event from the mouse. It has a trigger mouse button (the button
 * of the mouse that was the trigger of the event), a point where the
 * event was happend (relative to the widget's client area).
 *
 * getButton returns MouseButton::None if the event is about mouse
 * movement (no button was pressed to trigger the event).
 */
class VACA_DLL MouseEvent : public Event
{
  Point mPoint;
  int mClicks;
  int mFlags;
  MouseButtons::Type mTrigger;
  int mDelta;

public:

  MouseEvent(Widget *source, Point point, int clicks, int flags, MouseButtons::Type trigger, int delta = 0);
  virtual ~MouseEvent();

  int getX() const;
  int getY() const;
  Point getPoint() const;

  int getClicks() const;
  MouseButtons::Type getButton() const;

  bool isLeftButtonPressed() const;
  bool isRightButtonPressed() const;
  bool isMiddleButtonPressed() const;
  bool isShiftKeyPressed() const;
  bool isControlKeyPressed() const;
  bool isAltKeyPressed() const;

  int getDelta() const;

};

} // namespace Vaca

#endif