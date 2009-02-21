// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the author nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VACA_SCROLLEVENT_H
#define VACA_SCROLLEVENT_H

#include "Vaca/base.h"
#include "Vaca/Enum.h"
#include "Vaca/Event.h"
#include "Vaca/Point.h"

namespace Vaca {

/**
 * It's like a namespace for ScrollRequest.
 * 
 * @see ScrollRequest
 */
struct ScrollRequestEnum
{
  enum enumeration {
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

  static const enumeration default_value = FullBackward;
};

/**
 * Defines how does the user want to scroll.
 *
 * One of the following values:
 * @li ScrollRequest::FullBackward: go to the first page or beginning of line (Home).
 * @li ScrollRequest::FullForward: go to the last page or end of line (End).
 * @li ScrollRequest::LineBackward: go to to the previous line or character (Left/Up).
 * @li ScrollRequest::LineForward: go to to the next line or character (Right/Down).
 * @li ScrollRequest::PageBackward: go to the previous page (PageUp).
 * @li ScrollRequest::PageForward: go to to the next page (PageDown).
 * @li ScrollRequest::BoxPosition: the user moved the scroll box (also called "thumb").
 * @li ScrollRequest::BoxTracking: the is moving the scroll box.
 * @li ScrollRequest::EndScroll: the scroll request finished.
 */
typedef Enum<ScrollRequestEnum> ScrollRequest;

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

} // namespace Vaca

#endif // VACA_SCROLLEVENT_H
