// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_FOCUSEVENT_H
#define VACA_FOCUSEVENT_H

#include "Vaca/base.h"
#include "Vaca/Event.h"

namespace Vaca {

class VACA_DLL FocusEvent : public Event
{
  Widget* m_oldFocus;
  Widget* m_newFocus;

public:

  FocusEvent(Widget* source, Widget* oldFocus, Widget* newFocus);
  virtual ~FocusEvent();

  Widget* getOldFocus() const;
  Widget* getNewFocus() const;

};

} // namespace Vaca

#endif // VACA_FOCUSEVENT_H
