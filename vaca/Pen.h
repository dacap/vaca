// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_PEN_H
#define VACA_PEN_H

#include "vaca/base.h"
#include "vaca/Enum.h"
#include "vaca/SharedPtr.h"

namespace vaca {

// ======================================================================

/**
   It's like a namespace for PenStyle.

   @see PenStyle
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
   Speficies the style to be used when draw a line. One of the
   following values:
   @li PenStyle::Solid
   @li PenStyle::Dash
   @li PenStyle::Dot
   @li PenStyle::DashDot
   @li PenStyle::DashDotDot
   @li PenStyle::Null
   @li PenStyle::InsideFram
*/
typedef Enum<PenStyleEnum> PenStyle;

// ======================================================================

/**
   It's like a namespace for PenEndCap.

   @see PenEndCap
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
   Speficies how to end the lines of an open GraphicsPath. One of the
   following values:
   @li PenEndCap::Round (default)
   @li PenEndCap::Square
   @li PenEndCap::Flat
*/
typedef Enum<PenEndCapEnum> PenEndCap;

// ======================================================================
// Pen Join

/**
   It's like a namespace for PenJoin.

   @see PenJoin
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
   Specifies how to join the corners of a GraphicsPath. One of the
   following values:
   @li PenJoin::Round (default)
   @li PenJoin::Bevel
   @li PenJoin::Miter
*/
typedef Enum<PenJoinEnum> PenJoin;

// ======================================================================

/**
   A pen can be used to draw lines, edges of rectangles, ellipses, and
   paths.

   This is a SharedPtr, so if you copy instances of pens they will be
   referencing to the same place. You can't clone pens because you can't
   modify them.

   @win32
     This is a @msdn{HPEN} wrapper.
   @endwin32

   @see Graphics, Graphics#drawLine, Graphics#drawRect, Graphics#strokePath
*/
class VACA_DLL Pen
{
  template<typename To, typename From>
  friend To convert_to(const From& from);

public:
  Pen();
  Pen(const Pen& pen);
  explicit Pen(const Color& color, int width = 1);
  Pen(const Color& color, int width,
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

private:
  class PenImpl;
  SharedPtr<PenImpl> m_impl;
};

} // namespace vaca

#endif // VACA_PEN_H
