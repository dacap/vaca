// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_LAYOUTEVENT_H
#define VACA_LAYOUTEVENT_H

#include "vaca/base.h"
#include "vaca/Event.h"
#include "vaca/Rect.h"

namespace vaca {

class VACA_DLL LayoutEvent : public Event
{
  Rect m_bounds;

public:

  LayoutEvent(Widget* source, const Rect& bounds);
  virtual ~LayoutEvent();

  Rect getBounds() const;
  void setBounds(const Rect& rc);

};

} // namespace vaca

#endif // VACA_LAYOUTEVENT_H
