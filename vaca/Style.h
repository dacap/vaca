// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
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

  // This constructor must be here to export const styles
  // (e.g. Widget::Styles::Default, etc.)
  constexpr Style(int regular, int extended)
    : regular(regular),
      extended(extended) {
  }

  constexpr Style(const Style& style)
    : regular(style.regular),
      extended(style.extended) {
  }

  constexpr Style operator+(const Style& style) const {
    return Style(regular | style.regular,
                 extended | style.extended);
  }

  constexpr Style operator|(const Style& style) const {
    return Style(regular | style.regular,
                 extended | style.extended);
  }

  constexpr Style operator-(const Style& style) const {
    return Style(regular & ~style.regular,
                 extended & ~style.extended);
  }


  constexpr Style operator&(const Style& style) const {
    return Style(regular & style.regular,
                 extended & style.extended);
  }

  constexpr Style operator~() const {
    return Style(~regular, ~extended);
  }

  constexpr bool operator==(const Style& style) const {
    return
      regular == style.regular &&
      extended == style.extended;
  }

  constexpr bool operator!=(const Style& style) const {
    return
      regular != style.regular ||
      extended != style.extended;
  }

};

} // namespace vaca

#endif // VACA_STYLE_H
