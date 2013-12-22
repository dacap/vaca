// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/Anchor.h"

using namespace Vaca;

/**
   Creates an anchor constraint to be used in a child that has a
   parent with an AnchorLayout assigned.

   The AnchorLayout tries to maintain the distance of each anchored
   @a refRect's side with each side of the @c Rect(Point(0,0),refSize)
   rectangle, where @c refSize is specified in the
   @link AnchorLayout#AnchorLayout AnchorLayout constructor@endlink.

   @param refRect
     The rectangle of reference. This rectangle should be
     inside @c Rect(Point(0,0),refSize).

   @param sides
     Which sides of the @a refRect do you want to anchor.
     Where "to anchor" means "maintain the specified sides in the same
     distance to the @c refSize specified in the
     @link AnchorLayout#AnchorLayout AnchorLayout constructor@endlink".
     Zero or more of the following values:
       @li Sides::Left
       @li Sides::Top
       @li Sides::Right
       @li Sides::Bottom

     You can combine sides using @c | operator. E.g:
     @code
     myWidget.setConstraint(new Anchor(Rect(...),
                                       Sides::Left |
                                       Sides::Top));
     @endcode
*/
Anchor::Anchor(const Rect& refRect, Sides sides)
  : m_refRect(refRect)
  , m_sides(sides)
{
}

/**
   Destroys the Anchor contraint.
 */
Anchor::~Anchor()
{
}

Rect Anchor::getRefRect() const
{
  return m_refRect;
}

void Anchor::setRefRect(const Rect& refRect)
{
  m_refRect = refRect;
}

Sides Anchor::getSides() const
{
  return m_sides;
}

void Anchor::setSides(Sides sides)
{
  m_sides = sides;
}
