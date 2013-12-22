// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/Event.h"

using namespace Vaca;

/**
   Creates a new event specifying that it was generated
   from the @a source component.

   @param source
     The component which generates the event.
*/
Event::Event(Component* source)
  : m_source(source)
{
}

/**
   Destroys the event.
*/
Event::~Event()
{
}

/**
   Returns the event's source.

   @return
     The component which generates the event.
*/
Component* Event::getSource()
{
  return m_source;
}
