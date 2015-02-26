// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/AnchorLayout.h"
#include "vaca/Anchor.h"
#include "vaca/Size.h"
#include "vaca/Point.h"
#include "vaca/Debug.h"
#include "vaca/Widget.h"

using namespace vaca;

/**
   Creates a new anchor layout.
   
   @param refSize
     The size of the imaginary rectangle @c Rect(Point(0,0),refSize).
     All @c refRect specified in @link Anchor#Anchor Anchor constructor@endlink
     should be inside this rectangle, and the distance of each
     anchored side in an Anchor will be kept equal.
*/
AnchorLayout::AnchorLayout(const Size& refSize)
  : m_refSize(refSize)
{
}

void AnchorLayout::layout(Widget* parent, WidgetList& widgets, const Rect& parentRc)
{
  Size delta(parentRc.getSize() - m_refSize);
  WidgetsMovement movement(widgets);

  for (WidgetList::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
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

    movement.moveWidget(widget, rc);
  }
}
