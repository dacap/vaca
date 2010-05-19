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

class Vaca::Brush::BrushImpl : public GdiObject<HBRUSH>
{
public:

  BrushImpl()
    : GdiObject<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0))) {
  }

  BrushImpl(const Color& color)
    : GdiObject<HBRUSH>(CreateSolidBrush(convert_to<COLORREF>(color))) {
  }

  ~BrushImpl() {
  }

  Color getColor() const {
    LOGBRUSH lb;
    assert(getHandle());
    ::GetObject(getHandle(), sizeof(LOGBRUSH), &lb);
    return convert_to<Color>(lb.lbColor);
  }

};

template<> HBRUSH Vaca::convert_to(const Brush& brush)
{
  // 1) getHandle is defined in GdiObject<HBRUSH> class
  // 2) convert_to<> is friend of Brush class
  return brush.m_impl->getHandle();
}
