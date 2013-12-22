// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_PAINTEVENT_H
#define VACA_PAINTEVENT_H

#include "Vaca/base.h"
#include "Vaca/Event.h"

namespace Vaca {

/**
   Event to paint a widget.
*/
class VACA_DLL PaintEvent : public Event
{
  Graphics& m_graphics;
  bool m_painted;

public:

  PaintEvent(Widget* source, Graphics& graphics);
  virtual ~PaintEvent();

  Graphics& getGraphics();

  bool isPainted() const;
};

} // namespace Vaca

#endif // VACA_PAINTEVENT_H
