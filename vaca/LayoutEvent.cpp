// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/LayoutEvent.h"
#include "vaca/Widget.h"

using namespace vaca;

LayoutEvent::LayoutEvent(Widget* source, const Rect& bounds)
  : Event(source)
  , m_bounds(bounds)
{
}

/**
   Destroys the LayoutEvent.
*/
LayoutEvent::~LayoutEvent()
{
}

/**
   Returns the area where Widget#onLayout member function should
   put children widgets.

   It is generally the client bounds, but other widgets (like Tab)
   could reduce this rectangle to use a small area inside the widget.

   @see Widget#getClientBounds
*/
Rect LayoutEvent::getBounds() const
{
  return m_bounds;
}

void LayoutEvent::setBounds(const Rect& bounds)
{
  m_bounds = bounds;
}
