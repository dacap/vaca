// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/SetCursorEvent.h"
#include "Vaca/Widget.h"
#include "Vaca/Cursor.h"

using namespace Vaca;

SetCursorEvent::SetCursorEvent(Widget* source, Point point, WidgetHit hit)
  : MouseEvent(source, point, 0, 0, MouseButton::None)
  , m_hit(hit)
{
}

SetCursorEvent::~SetCursorEvent()
{
}

void SetCursorEvent::setCursor(const Cursor& cursor)
{
  ::SetCursor(const_cast<Cursor*>(&cursor)->getHandle());
  consume();
}

WidgetHit SetCursorEvent::getWidgetHit() const
{
  return m_hit;
}
