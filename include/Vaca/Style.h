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

#ifndef VACA_STYLE_H
#define VACA_STYLE_H

#include "Vaca/base.h"

namespace Vaca {

/**
 * A widget style.
 *
 * Each widget has a style that can be configured to show little
 * variations of its look and feel.
 *
 * You can add and substract styles with the + and - operators.
 *
 * @code
 * {
 *   Frame frame(..., FrameStyle + ClientEdgeStyle - MinimizableFrameStyle)
 *   Edit edit("", EditStyle + ClientEdgeStyle)
 *   ...
 * }
 * @endcode
 * 
 * @win32 
 *   It is a wrapper for regular and extended window styles
 *   (@msdn{WS_} and @msdn{WS_EX_}).
 * @endwin32
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

} // namespace Vaca

#endif // VACA_STYLE_H

