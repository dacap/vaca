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
