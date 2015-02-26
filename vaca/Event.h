// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_EVENT_H
#define VACA_EVENT_H

#include "vaca/base.h"

namespace vaca {

/**
   Base class for every kind of event.
*/
class VACA_DLL Event
{
  /**
     The component which generates the event. It's specified in the
     @link Event#Event(Component*) Event's constructor@endlink
  */
  Component* m_source;

public:

  Event(Component* source);
  virtual ~Event();

  Component* getSource();

};

} // namespace vaca

#endif // VACA_EVENT_H
