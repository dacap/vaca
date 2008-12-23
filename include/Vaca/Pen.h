// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

#ifndef VACA_PEN_H
#define VACA_PEN_H

#include "Vaca/base.h"
#include "Vaca/Enum.h"
#include "Vaca/GdiObject.h"
#include "Vaca/SmartPtr.h"

namespace Vaca {

class Color;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for PenStyle.
 * 
 * @see PenStyle
 */
struct PenStyleEnum
{
  enum enumeration {
    Solid,
    Dash,
    Dot,
    DashDot,
    DashDotDot,
    Null,
    InsideFrame
  };
  static const enumeration default_value = Solid;
};

/**
 * Speficies the style to be used when draw a line. One of the
 * following values:
 * @li PenStyle::Solid
 * @li PenStyle::Dash
 * @li PenStyle::Dot
 * @li PenStyle::DashDot
 * @li PenStyle::DashDotDot
 * @li PenStyle::Null
 * @li PenStyle::InsideFram
 */
typedef Enum<PenStyleEnum> PenStyle;

//////////////////////////////////////////////////////////////////////

/**
 * It's like a namespace for PenEndCap.
 * 
 * @see PenEndCap
 */
struct PenEndCapEnum
{
  enum enumeration {
    Round,
    Square,
    Flat
  };
  static const enumeration default_value = Round;
};

/**
 * Speficies how to end the lines of an open GraphicsPath. One of the
 * following values:
 * @li PenEndCap::Round (default)
 * @li PenEndCap::Square
 * @li PenEndCap::Flat
 */
typedef Enum<PenEndCapEnum> PenEndCap;

//////////////////////////////////////////////////////////////////////
// Pen Join

/**
 * It's like a namespace for PenJoin.
 * 
 * @see PenJoin
 */
struct PenJoinEnum
{
  enum enumeration {
    Round,
    Bevel,
    Miter
  };
  static const enumeration default_value = Round;
};

/**
 * Specifies how to join the corners of a GraphicsPath. One of the
 * following values:
 * @li PenJoin::Round (default)
 * @li PenJoin::Bevel
 * @li PenJoin::Miter
 */
typedef Enum<PenJoinEnum> PenJoin;

//////////////////////////////////////////////////////////////////////

/**
 * A pen can be used to draw lines, edges of rectangles, ellipses, and
 * paths.
 *
 * This is a SmartPtr, so if you copy instances of pens they will be
 * referencing to the same place. You can't clone pens because you can't
 * modify them.
 *
 * @win32
 *   This is a @msdn{HPEN} wrapper.
 * @endwin32
 *
 * @see Graphics, Graphics#drawLine, Graphics#drawRect, Graphics#strokePath
 */
class VACA_DLL Pen : private SmartPtr<GdiObject<HPEN> >
{
public:

  Pen();
  Pen(const Pen& pen);
  explicit Pen(const Color& color, int width = 1);
  explicit Pen(const Color& color, int width,
	       PenStyle style, // = PenStyle::Solid,
	       PenEndCap endCap = PenEndCap::Round,
	       PenJoin join = PenJoin::Round);
  virtual ~Pen();

  Pen& operator=(const Pen& pen);

  Color getColor() const;
  int getWidth() const;
  PenStyle getStyle() const;
  PenEndCap getEndCap() const;
  PenJoin getJoin() const;

  HPEN getHandle() const;
  
};

} // namespace Vaca

#endif // VACA_PEN_H
