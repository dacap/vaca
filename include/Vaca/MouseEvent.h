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

#include "Vaca/Point.h"
#include "Vaca/WidgetEvent.h"

namespace Vaca {

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
class MouseEvent : public WidgetEvent
{
  Point mPoint;
  int mClickCount;
  MouseButtons::Type mTrigger;
  int mFlags;
  int mDelta;

public:

  MouseEvent(Widget *widget, Point point, int clickCount, MouseButtons::Type trigger, int flags, int delta = 0)
    : WidgetEvent(widget)
    , mPoint(point)
    , mClickCount(clickCount)
    , mTrigger(trigger)
    , mFlags(flags)
    , mDelta(delta)
  {
  }

  /**
   * Returns the X component of the mouse position (getPoint).
   */
  int getX() { return mPoint.x; }

  /**
   * Returns the Y component of the mouse position (getPoint).
   */
  int getY() { return mPoint.y; }

  /**
   * Returns the mouse position relative to the control that received
   * the event.
   */
  Point getPoint() { return mPoint; }

  /**
   * Returns how many clicks the user did:
   * @li 0 = just mouse movement;
   * @li 1 = single click;
   * @li 2 = double click.
   */
  int getClickCount() { return mClickCount; }

  /**
   * Returns the button that trigger the event (if the event is
   * Widget::mouseUp or Widget::mouseDown).
   */
  MouseButtons::Type getButton() { return mTrigger; }

  bool isLeftButtonPressed()   { return (mFlags & MK_LBUTTON) != 0; }
  bool isRightButtonPressed()  { return (mFlags & MK_RBUTTON) != 0; }
  bool isMiddleButtonPressed() { return (mFlags & MK_MBUTTON) != 0; }
  bool isShiftKeyPressed()     { return (mFlags & MK_SHIFT  ) != 0; }
  bool isControlKeyPressed()   { return (mFlags & MK_CONTROL) != 0; }
  bool isAltKeyPressed()       { return (mFlags & MK_ALT    ) != 0; }

  int getDelta() { return mDelta; }

};

} // namespace Vaca

#endif
