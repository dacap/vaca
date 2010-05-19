// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Vaca/Color.h"
#include "Vaca/GdiObject.h"
#include "Vaca/win32.h"

class Vaca::Pen::PenImpl : public GdiObject<HPEN>
{
public:

  PenImpl()
    : GdiObject<HPEN>(CreatePen(PS_COSMETIC | PS_SOLID, 1,
				RGB(0, 0, 0))) {
  }

  PenImpl(const Color& color, int width)
    : GdiObject<HPEN>(CreatePen(PS_COSMETIC | PS_SOLID, width,
				convert_to<COLORREF>(color))) {
  }

  PenImpl(const Color& color, int width,
	  PenStyle style, PenEndCap endCap, PenJoin join)
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
    lb.lbColor = convert_to<COLORREF>(color);
    lb.lbHatch = 0;

    HPEN handle;
    if (width == 1)
      handle = ExtCreatePen(PS_COSMETIC | flags, width, &lb, 0, NULL);
    else
      handle = ExtCreatePen(PS_GEOMETRIC | flags, width, &lb, 0, NULL);

    setHandle(handle);
  }

  ~PenImpl() {
  }

  Color getColor() const {
    EXTLOGPEN elp;
    assert(getHandle());
    ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);
    return convert_to<Color>(elp.elpColor);
  }

  int getWidth() const {
    EXTLOGPEN elp;
    assert(getHandle());
    ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);
    return elp.elpWidth;
  }

  PenStyle getStyle() const {
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

  PenEndCap getEndCap() const {
    EXTLOGPEN elp;
    assert(getHandle());
    ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);

    PenEndCap endCap;

    if (elp.elpPenStyle  & PS_ENDCAP_ROUND) { endCap = PenEndCap::Round; }
    else if (elp.elpPenStyle  & PS_ENDCAP_SQUARE) { endCap = PenEndCap::Square; }
    else if (elp.elpPenStyle  & PS_ENDCAP_FLAT) { endCap = PenEndCap::Flat; }

    return endCap;
  }

  PenJoin getJoin() const {
    EXTLOGPEN elp;
    assert(getHandle());
    ::GetObject(getHandle(), sizeof(EXTLOGPEN), &elp);

    PenJoin join;

    if (elp.elpPenStyle  & PS_JOIN_ROUND) { join = PenJoin::Round; }
    else if (elp.elpPenStyle  & PS_JOIN_BEVEL) { join = PenJoin::Bevel; }
    else if (elp.elpPenStyle  & PS_JOIN_MITER) { join = PenJoin::Miter; }

    return join;
  }

};

template<> HPEN Vaca::convert_to(const Pen& pen)
{
  // getHandle is defined in GdiObject<HPEN> class
  return pen.m_impl->getHandle();
}
