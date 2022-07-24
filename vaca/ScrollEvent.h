// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SCROLLEVENT_H
#define VACA_SCROLLEVENT_H

#include "vaca/base.h"
#include "vaca/Enum.h"
#include "vaca/Event.h"
#include "vaca/Point.h"

namespace vaca {

/**
   Defines how does the user want to scroll.

   One of the following values:
   @li ScrollRequest::FullBackward: go to the first page or beginning of line (Home).
   @li ScrollRequest::FullForward: go to the last page or end of line (End).
   @li ScrollRequest::LineBackward: go to to the previous line or character (Left/Up).
   @li ScrollRequest::LineForward: go to to the next line or character (Right/Down).
   @li ScrollRequest::PageBackward: go to the previous page (PageUp).
   @li ScrollRequest::PageForward: go to to the next page (PageDown).
   @li ScrollRequest::BoxPosition: the user moved the scroll box (also called "thumb").
   @li ScrollRequest::BoxTracking: the is moving the scroll box.
   @li ScrollRequest::EndScroll: the scroll request finished.
*/
enum class ScrollRequest
{
  FullBackward,
  FullForward,
  PageBackward,
  PageForward,
  LineBackward,
  LineForward,
  BoxPosition,
  BoxTracking,
  EndScroll,
};

class VACA_DLL ScrollEvent : public Event
{
  Orientation m_orientation;
  ScrollRequest m_request;
  int m_position;

public:

  ScrollEvent(Widget* source, Orientation orien, ScrollRequest req, int pos);
  virtual ~ScrollEvent();

  Orientation getOrientation() const;
  ScrollRequest getRequest() const;
  int getPosition() const;

};

} // namespace vaca

#endif // VACA_SCROLLEVENT_H
