// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, 2009, David Capello
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

#include "Vaca/Pen.h"
#include "Vaca/Color.h"
#include <cassert>

using namespace Vaca;

Pen::Pen()
  : SharedPtr<GdiObject<HPEN> >(new GdiObject<HPEN>(CreatePen(PS_COSMETIC | PS_SOLID, 1,
							     RGB(0, 0, 0))))
{
}

Pen::Pen(const Pen& pen)
  : SharedPtr<GdiObject<HPEN> >(pen)
{
}

/**
 * Creates a pen.
 *
 * @param color Color of the pen.
 * @param width Width of the pen. If it's 1, the pen will be cosmetic,
 *              if width > 0 the pen will be geometric.
 */
Pen::Pen(const Color& color, int width)
  : SharedPtr<GdiObject<HPEN> >(new GdiObject<HPEN>(CreatePen(PS_COSMETIC | PS_SOLID, width,
								 color.getColorRef())))
{
}

Pen::Pen(const Color& color, int width,
	 PenStyle style, PenEndCap endCap, PenJoin join)
  : SharedPtr<GdiObject<HPEN> >(new GdiObject<HPEN>)
{
  int flags = 0;

  // pen style
  switch (style) {
    case PenStyle::Solid:       flags |= PS_SOLID;       break;
    case PenStyle::Dash:        flags |= PS_DASH;        break;
    case PenStyle::Dot:         flags |= PS_DOT;         break;
    case PenStyle::DashDot:     flags |= PS_DASHDOT;     break;
    case PenStyle::DashDotDot:  flags |= PS_DASHDOTDOT;  break;
    case PenStyle::Null:        flags |= PS_NULL;        break;
    case PenStyle::InsideFrame: flags |= PS_INSIDEFRAME; break;
  }

  // end cap style
  switch (endCap) {
    case PenEndCap::Round:  flags |= PS_ENDCAP_ROUND;  break;
    case PenEndCap::Square: flags |= PS_ENDCAP_SQUARE; break;
    case PenEndCap::Flat:   flags |= PS_ENDCAP_FLAT;   break;
  }

  // join style
  switch (join) {
    case PenJoin::Round: flags |= PS_JOIN_ROUND; break;
    case PenJoin::Bevel: flags |= PS_JOIN_BEVEL; break;
    case PenJoin::Miter: flags |= PS_JOIN_MITER; break;
  }

  LOGBRUSH lb;
  lb.lbStyle = BS_SOLID;
  lb.lbColor = color.getColorRef();
  lb.lbHatch = 0;

  HPEN handle;
  if (width == 1)
    handle = ExtCreatePen(PS_COSMETIC | flags, width, &lb, 0, NULL);
  else
    handle = ExtCreatePen(PS_GEOMETRIC | flags, width, &lb, 0, NULL);

  get()->setHandle(handle);
}

/**
 * Destroys this pen reference.
 *
 * @warning
 *   The last reference to be destroyed will call Win32's DeleteObject to destroy
 *   the HPEN handle.
 */
Pen::~Pen()
{
}

Pen& Pen::operator=(const Pen& pen)
{
  SharedPtr<GdiObject<HPEN> >::operator=(pen);
  return *this;
}

/**
 * Gets pen's color.
 * 
 * @return Current pen's color.
 */
Color Pen::getColor() const
{
  EXTLOGPEN elp;
  assert(getHandle());
  ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);
  return Color(elp.elpColor);
}

/**
 * Gets pen's width (in pixels).
 * 
 * @return Current pen's width.
 */
int Pen::getWidth() const
{
  EXTLOGPEN elp;
  assert(getHandle());
  ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);
  return elp.elpWidth;
}

/**
 * Gets pen's style.
 *
 * @return Current pen's style.
 */
PenStyle Pen::getStyle() const
{
  EXTLOGPEN elp;
  assert(getHandle());
  ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);

  PenStyle style;

  if (elp.elpPenStyle  & PS_SOLID) { style = PenStyle::Solid; }
  else if (elp.elpPenStyle  & PS_DASH) { style = PenStyle::Dash; }
  else if (elp.elpPenStyle  & PS_DOT) { style = PenStyle::Dot; }
  else if (elp.elpPenStyle  & PS_DASHDOT) { style = PenStyle::DashDot; }
  else if (elp.elpPenStyle  & PS_DASHDOTDOT) { style = PenStyle::DashDotDot; }
  else if (elp.elpPenStyle  & PS_NULL) { style = PenStyle::Null; }
  else if (elp.elpPenStyle  & PS_INSIDEFRAME) { style = PenStyle::InsideFrame; }

  return style;
}

PenEndCap Pen::getEndCap() const
{
  EXTLOGPEN elp;
  assert(getHandle());
  ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);

  PenEndCap endCap;

  if (elp.elpPenStyle  & PS_ENDCAP_ROUND) { endCap = PenEndCap::Round; }
  else if (elp.elpPenStyle  & PS_ENDCAP_SQUARE) { endCap = PenEndCap::Square; }
  else if (elp.elpPenStyle  & PS_ENDCAP_FLAT) { endCap = PenEndCap::Flat; }

  return endCap;
}

PenJoin Pen::getJoin() const
{
  EXTLOGPEN elp;
  assert(getHandle());
  ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);

  PenJoin join;

  if (elp.elpPenStyle  & PS_JOIN_ROUND) { join = PenJoin::Round; }
  else if (elp.elpPenStyle  & PS_JOIN_BEVEL) { join = PenJoin::Bevel; }
  else if (elp.elpPenStyle  & PS_JOIN_MITER) { join = PenJoin::Miter; }

  return join;
}

/**
 * Returns the Win32's handle.
 */
HPEN Pen::getHandle() const
{
  return get()->getHandle();
}
