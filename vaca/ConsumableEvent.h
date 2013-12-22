// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CONSUMABLEEVENT_H
#define VACA_CONSUMABLEEVENT_H

#include "Vaca/Event.h"

namespace Vaca {

/**
   Event that can be consumed.
*/
class VACA_DLL ConsumableEvent : public Event
{
  /**
     The event was consumed.
  */
  bool m_consumed;

public:

  ConsumableEvent(Component* source);
  virtual ~ConsumableEvent();

  void consume();
  bool isConsumed() const;

};

} // namespace Vaca

#endif // VACA_CONSUMABLEEVENT_H
