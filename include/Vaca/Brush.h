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

#ifndef VACA_BRUSH_HPP
#define VACA_BRUSH_HPP

#include "Vaca/base.h"
#include "Vaca/Enum.h"
#include "Vaca/Color.h"

namespace Vaca {

//////////////////////////////////////////////////////////////////////
// Brush Style

// struct _BrushStyle
// {
//   enum enumeration {
//     Solid,
//     Dash,
//     Dot,
//     DashDot,
//     DashDotDot,
//     Null,
//     InsideFrame
//   };
//   static const enumeration default_value = Solid;
// };

// typedef Enum<_BrushStyle> BrushStyle;

//////////////////////////////////////////////////////////////////////
// Brush End Cap

// struct _BrushEndCap
// {
//   enum enumeration {
//     Round,
//     Square,
//     Flat
//   };
//   static const enumeration default_value = Round;
// };

// typedef Enum<_BrushEndCap> BrushEndCap;

//////////////////////////////////////////////////////////////////////
// Brush Join

// struct _BrushJoin
// {
//   enum enumeration {
//     Round,
//     Bevel,
//     Miter
//   };
//   static const enumeration default_value = Round;
// };

// typedef Enum<_BrushJoin> BrushJoin;

//////////////////////////////////////////////////////////////////////
// Brush

/**
 * A brush.
 */
class VACA_DLL Brush
{
  HBRUSH m_HBRUSH;
  bool m_modified;
  Color m_color;
//   int mWidth;
//   BrushStyle mStyle;
//   BrushEndCap mEndCap;
//   BrushJoin mJoin;
  
public:

  // Cosmetic Brush
  explicit Brush(Color color// , int width = 1
		 );
//   Brush(Brush& brush, int width = 1);
//   Brush(const Brush& brush);
  virtual ~Brush();

  Color getColor() const;
  void setColor(Color color);

//   int getWidth() const;
//   void setWidth(int width);

//   BrushStyle getStyle() const;
//   void setStyle(BrushStyle style);

//   BrushEndCap getEndCap() const;
//   void setEndCap(BrushEndCap endCap);

//   BrushJoin getJoin() const;
//   void setJoin(BrushJoin join);

  HBRUSH getHBRUSH();

private:

  void initialize();
  void destroy();
  
};

} // namespace Vaca

#endif
