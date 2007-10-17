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

#include "stdvaca.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/Widget.h"

using namespace Vaca;

MouseEvent::MouseEvent(Widget *source, Point point, int clicks,
		       int flags, MouseButtons::Type trigger, int delta)
  : Event(source)
  , m_point(point)
  , m_clicks(clicks)
  , m_flags(flags)
  , m_trigger(trigger)
  , m_delta(delta)
{
}

MouseEvent::~MouseEvent()
{
}

/**
 * Returns the X component of the mouse position (getPoint).
 */
int MouseEvent::getX() const
{
  return m_point.x;
}

/**
 * Returns the Y component of the mouse position (getPoint).
 */
int MouseEvent::getY() const
{
  return m_point.y;
}

/**
 * Returns the mouse position relative to the Widget client area
 * (@ref Widget::getClientBounds()).
 */
Point MouseEvent::getPoint() const
{
  return m_point;
}

/**
 * Returns how many clicks the user did:
 * @li 0 = mouse movement;
 * @li 1 = single click;
 * @li 2 = double click.
 */
int MouseEvent::getClicks() const
{
  return m_clicks;
}

/**
 * Returns the button that trigger the event (if the event is
 * Widget::mouseUp or Widget::mouseDown).
 */
MouseButtons::Type MouseEvent::getButton() const
{
  return m_trigger;
}

bool MouseEvent::isLeftButtonPressed() const
{
  return (m_flags & MK_LBUTTON) != 0;
}

bool MouseEvent::isRightButtonPressed() const
{
  return (m_flags & MK_RBUTTON) != 0;
}

bool MouseEvent::isMiddleButtonPressed() const
{
  return (m_flags & MK_MBUTTON) != 0;
}

bool MouseEvent::isShiftKeyPressed() const
{
  return (m_flags & MK_SHIFT) != 0;
}

bool MouseEvent::isControlKeyPressed() const
{
  return (m_flags & MK_CONTROL) != 0;
}

bool MouseEvent::isAltKeyPressed() const
{
  return (m_flags & MK_ALT) != 0;
}

int MouseEvent::getDelta() const
{
  return m_delta;
}
