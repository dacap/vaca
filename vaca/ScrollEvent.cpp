// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/ScrollEvent.h"
#include "Vaca/Widget.h"

using namespace Vaca;

ScrollEvent::ScrollEvent(Widget* source, Orientation orien, ScrollRequest req, int pos)
  : Event(source)
  , m_orientation(orien)
  , m_request(req)
  , m_position(pos)
{
}

ScrollEvent::~ScrollEvent()
{
}

Orientation ScrollEvent::getOrientation() const
{
  return m_orientation;
}

ScrollRequest ScrollEvent::getRequest() const
{
  return m_request;
}

int ScrollEvent::getPosition() const
{
  return m_position;
}
