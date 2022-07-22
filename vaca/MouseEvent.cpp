// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/MouseEvent.h"
#include "vaca/Widget.h"

#ifdef VACA_WINDOWS
  #include <shellapi.h>
#endif

using namespace vaca;

/**
   Creates an event from the mouse.

   @param source Source of the event (a Widget or Component)
   @param point Point where the mouse happened
   @param clicks How many clicks the user pressed? 1 means click-down, 2 means double-click.
   @param flags Flags used internally (see MK_* constants of Win32)
   @param trigger Which buttons were pressed
   @param delta The wheel delta (Z axis) counted in notches. @todo positive is up or down?
*/
MouseEvent::MouseEvent(Widget* source, Point point, int clicks,
		       int flags, MouseButton trigger, int delta)
  : ConsumableEvent(source)
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
   Returns the X component of the mouse position (#getPoint).

   The position is relative to the client-bounds of the source.

   @see #getSource, Widget::getClientBounds
*/
int MouseEvent::getX() const
{
  return m_point.x;
}

/**
   Returns the Y component of the mouse position (#getPoint).

   The position is relative to the client-bounds of the source.

   @see #getSource, Widget::getClientBounds
*/
int MouseEvent::getY() const
{
  return m_point.y;
}

/**
   Returns the mouse position relative to the Widget client area.

   @see Widget#getClientBounds
*/
Point MouseEvent::getPoint() const
{
  return m_point;
}

/**
   Returns how many clicks the user did:
   @li 0 = mouse movement;
   @li 1 = single click;
   @li 2 = double click.
*/
int MouseEvent::getClicks() const
{
  return m_clicks;
}

/**
   Returns the button that trigger the event (if the event is
   Widget#onMouseUp or Widget#onMouseDown).
*/
MouseButton MouseEvent::getButton() const
{
  return m_trigger;
}

/**
   Returns true if the left button was pressed when the event was
   generated.

   @see getButton
*/
bool MouseEvent::isLeftButtonPressed() const
{
  return (m_flags & MK_LBUTTON) != 0;
}

/**
   Returns true if the left button was pressed when the event was
   generated.

   @see getButton
*/
bool MouseEvent::isRightButtonPressed() const
{
  return (m_flags & MK_RBUTTON) != 0;
}

/**
   Returns true if the middle button was pressed when the event was
   generated. The middle button could be the wheel in modern mouses
   with wheel.

   @see getButton
*/
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
  // TODO MK_ALT doesn't exist for mouse message flags
  //return (m_flags & MK_ALT) != 0;
  return false;
}

int MouseEvent::getDelta() const
{
  return m_delta;
}
