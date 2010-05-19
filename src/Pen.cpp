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

#include "Vaca/Pen.h"

#if defined(VACA_WINDOWS)
  #include "win32/PenImpl.h"
#else
  #error Implement Pen class in your platform
#endif

using namespace Vaca;

Pen::Pen()
  : m_impl(new PenImpl())
{
}

Pen::Pen(const Pen& pen)
  : m_impl(pen.m_impl)		// Copy shared pointers
{
}

/**
   Creates a pen.

   @param color Color of the pen.
   @param width Width of the pen. If it's 1, the pen will be cosmetic,
		if width > 0 the pen will be geometric.
*/
Pen::Pen(const Color& color, int width)
  : m_impl(new PenImpl(color, width))
{
}

Pen::Pen(const Color& color, int width,
	 PenStyle style, PenEndCap endCap, PenJoin join)
  : m_impl(new PenImpl(color, width, style, endCap, join))
{
}

/**
   Destroys this pen reference.

   @warning
     The last reference to be destroyed will call Win32's DeleteObject to destroy
     the HPEN handle.
*/
Pen::~Pen()
{
}

Pen& Pen::operator=(const Pen& pen)
{
  // Copy shared pointers
  m_impl = pen.m_impl;
  return *this;
}

/**
   Gets pen's color.

   @return Current pen's color.
*/
Color Pen::getColor() const
{
  return m_impl->getColor();
}

/**
   Gets pen's width (in pixels).

   @return Current pen's width.
*/
int Pen::getWidth() const
{
  return m_impl->getWidth();
}

/**
   Gets pen's style.

   @return Current pen's style.
*/
PenStyle Pen::getStyle() const
{
  return m_impl->getStyle();
}

PenEndCap Pen::getEndCap() const
{
  return m_impl->getEndCap();
}

PenJoin Pen::getJoin() const
{
  return m_impl->getJoin();
}
