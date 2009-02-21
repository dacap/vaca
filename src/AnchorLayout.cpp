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

#include "Vaca/AnchorLayout.h"
#include "Vaca/Anchor.h"
#include "Vaca/Size.h"
#include "Vaca/Point.h"
#include "Vaca/Debug.h"

using namespace Vaca;

/**
 * Creates a new anchor layout.
 *
 * @param refSize
 *   The size of the imaginary rectangle @c Rect(Point(0,0),refSize).
 *   All @c refRect specified in @link Anchor#Anchor Anchor constructor@endlink
 *   should be inside this rectangle, and the distance of each
 *   anchored side in an Anchor will be kept equal.
 */
AnchorLayout::AnchorLayout(const Size& refSize)
  : m_refSize(refSize)
{
}

void AnchorLayout::layout(Widget* parent, Widget::Container& widgets, const Rect& parentRc)
{
  Size delta(parentRc.getSize() - m_refSize);

  beginMovement(widgets);

  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget* widget = *it;

    if (widget->isLayoutFree())
      continue;

    Constraint* constraint = widget->getConstraint();
    if (constraint == NULL)
      continue;

    Anchor* anchor = static_cast<Anchor*>(constraint);
    assert(anchor != NULL);

    Sides sides = anchor->getSides();
    Rect rc = anchor->getRefRect();

    bool left   = (sides & Sides::Left  ) != Sides::None;
    bool top    = (sides & Sides::Top   ) != Sides::None;
    bool right  = (sides & Sides::Right ) != Sides::None;
    bool bottom = (sides & Sides::Bottom) != Sides::None;

    if (left && right) {
      rc.w += delta.w;
    }
    else if (left) {
      // do nothing
    }
    else if (right) {
      rc.x += delta.w;
    }
    else {
      // distribute the extra spaces in both direction
      // (maintain the widget centered)
      rc.x += delta.w/2;
    }

    if (top && bottom) {
      rc.h += delta.h;
    }
    else if (top) {
      // do nothing
    }
    else if (bottom) {
      rc.y += delta.h;
    }
    else {
      // distribute the extra spaces in both direction
      // (maintain the widget centered)
      rc.y += delta.h/2;
    }

    moveWidget(widget, rc);
  }

  endMovement();
}
