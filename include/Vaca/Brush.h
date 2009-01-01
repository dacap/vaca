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

#ifndef VACA_BRUSH_H
#define VACA_BRUSH_H

#include "Vaca/base.h"
#include "Vaca/GdiObject.h"
#include "Vaca/SmartPtr.h"

namespace Vaca {

class Color;

/**
 * A brush can be used to fill rectangles, ellipses, and paths.
 *
 * This is a SmartPtr, so if you copy instances of brushes they will be
 * referencing to the same place. You can't clone brushes because you can't
 * modify them.
 *
 * @win32
 *   This is a @msdn{HBRUSH} wrapper.
 * @endwin32
 *
 * @see Graphics, Graphics#fillRect, Graphics#fillPath
 */
class VACA_DLL Brush : private SmartPtr<GdiObject<HBRUSH> >
{
public:

  Brush();
  Brush(const Brush& brush);
  explicit Brush(const Color& color);
  virtual ~Brush();

  Brush& operator=(const Brush& brush);

  Color getColor() const;

  HBRUSH getHandle() const;

};

} // namespace Vaca

#endif
