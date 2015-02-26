// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CLOSEEVENT_H
#define VACA_CLOSEEVENT_H

#include "vaca/CancelableEvent.h"

namespace vaca {

/**
   Event generated when a Frame is being closed.

   This is a CancelableEvent, so you can call #cancel to abort the
   close operation (it is useful to show a MsgBox like "Do you want
   to save your changes before close?" before the Frame is finally
   closed).
*/
class VACA_DLL CloseEvent : public CancelableEvent
{
public:

  CloseEvent(Frame* source);
  virtual ~CloseEvent();

};

} // namespace vaca

#endif // VACA_CLOSEEVENT_H
