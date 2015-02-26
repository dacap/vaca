// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SETCURSOREVENT_H
#define VACA_SETCURSOREVENT_H

#include "vaca/base.h"
#include "vaca/MouseEvent.h"
#include "vaca/WidgetHit.h"

namespace vaca {

class VACA_DLL SetCursorEvent : public MouseEvent
{
  WidgetHit m_hit;

public:

  SetCursorEvent(Widget* source, Point point, WidgetHit hit);
  virtual ~SetCursorEvent();

  void setCursor(const Cursor& cursor);

  WidgetHit getWidgetHit() const;

};

} // namespace vaca

#endif // VACA_SETCURSOREVENT_H
