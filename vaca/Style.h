// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_STYLE_H
#define VACA_STYLE_H

#include "vaca/base.h"

namespace vaca {

/**
   A widget style.

   Each widget has a style that can be configured to show little
   variations of its look and feel.

   You can add and substract styles with the + and - operators.

   @code
   {
     Frame frame(..., FrameStyle + ClientEdgeStyle - MinimizableFrameStyle)
     Edit edit("", EditStyle + ClientEdgeStyle)
     ...
   }
   @endcode

   @win32
     It is a wrapper for regular and extended window styles
     (@msdn{WS_} and @msdn{WS_EX_}).
   @endwin32
*/
struct VACA_DLL Style
{
  int regular;
  int extended;

  Style(int regular, int extended);
  Style(const Style& style);

  Style operator+(const Style& style) const;
  Style operator|(const Style& style) const;
  Style operator-(const Style& style) const;
  Style operator&(const Style& style) const;
  Style operator~() const;

  bool operator==(const Style& style) const;
  bool operator!=(const Style& style) const;

};

} // namespace vaca

#endif // VACA_STYLE_H
