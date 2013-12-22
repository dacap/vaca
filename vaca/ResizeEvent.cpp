// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/ResizeEvent.h"

using namespace Vaca;

ResizeEvent::ResizeEvent(Component* source, const Size& size)
  : Event(source)
  , m_size(size)
{
}

/**
   Destroys the ResizeEvent.
*/
ResizeEvent::~ResizeEvent()
{
}

Size ResizeEvent::getSize() const
{
  return m_size;
}
