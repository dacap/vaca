// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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
// * Neither the name of the Vaca nor the names of its contributors
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

#ifndef VACA_PEN_HPP
#define VACA_PEN_HPP

#include "Vaca/base.h"
#include "Vaca/Enum.h"
#include "Vaca/Color.h"

namespace Vaca {

//////////////////////////////////////////////////////////////////////
// Pen Style

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

typedef Enum<PenStyleEnum> PenStyle;

//////////////////////////////////////////////////////////////////////
// Pen End Cap

struct PenEndCapEnum
{
  enum enumeration {
    Round,
    Square,
    Flat
  };
  static const enumeration default_value = Round;
};

typedef Enum<PenEndCapEnum> PenEndCap;

//////////////////////////////////////////////////////////////////////
// Pen Join

struct PenJoinEnum
{
  enum enumeration {
    Round,
    Bevel,
    Miter
  };
  static const enumeration default_value = Round;
};

typedef Enum<PenJoinEnum> PenJoin;

//////////////////////////////////////////////////////////////////////
// Pen

/**
 * A pen. You can use pens to draw lines, edges of rectangles and
 * ellipses, etc.
 *
 * @see #Vaca::Graphics, #Vaca::Graphics::drawLine
 */
class VACA_DLL Pen
{
  HPEN m_HPEN;
  bool m_modified;
  Color m_color;
  int m_width;
  PenStyle m_style;
  PenEndCap m_endCap;
  PenJoin m_join;
  
public:

  // Cosmetic Pen
  explicit Pen(Color color, int width = 1);
//   Pen(Brush& brush, int width = 1);
//   Pen(const Pen& pen);
  virtual ~Pen();

  Color getColor() const;
  void setColor(Color color);

  int getWidth() const;
  void setWidth(int width);

  PenStyle getStyle() const;
  void setStyle(PenStyle style);

  PenEndCap getEndCap() const;
  void setEndCap(PenEndCap endCap);

  PenJoin getJoin() const;
  void setJoin(PenJoin join);

  HPEN getHPEN();

private:

  void initialize();
  void destroy();
  
};

} // namespace Vaca

#endif
