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

#include "Vaca/win32.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"
#include "Vaca/Rect.h"
#include "Vaca/Color.h"

using namespace Vaca;

template<> POINT Vaca::convert_to(const Point& pt)
{
  POINT res = { pt.x, pt.y };
  return res;
}

template<> Point Vaca::convert_to(const POINT& pt)
{
  return Point(pt.x, pt.y);
}

template<> Point Vaca::convert_to(const POINTS& pt)
{
  return Point(pt.x, pt.y);
}

template<> SIZE Vaca::convert_to(const Size& sz)
{
  SIZE res = { sz.w, sz.h };
  return res;
}

template<> Size Vaca::convert_to(const SIZE& sz)
{
  return Size(sz.cx, sz.cy);
}

/**
   Converts a Vaca's rectangle (Rect class) to a Win32's rectangle
   (RECT structure).

   @internal
*/
template<> RECT Vaca::convert_to(const Rect& rc)
{
  RECT res = { rc.x, rc.y, rc.x+rc.w, rc.y+rc.h };
  return res;
}

/**
   Converts a Win32's rectangle (RECT structure) to a Vaca's rectangle
   (Rect class).

   @internal
*/
template<> Rect Vaca::convert_to(const RECT& rc)
{
  return Rect(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
}

template<> COLORREF Vaca::convert_to(const Color& color)
{
  return RGB(color.getR(),
	     color.getG(),
	     color.getB());
}

template<> Color Vaca::convert_to(const COLORREF& colorref)
{
  return Color(GetRValue(colorref),
	       GetGValue(colorref),
	       GetBValue(colorref));
}
