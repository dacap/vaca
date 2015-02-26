// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Brush.h"

#if defined(VACA_WINDOWS)
  #include "win32/BrushImpl.h"
#else
  #error Implement Brush class in your platform
#endif

using namespace vaca;

Brush::Brush()
  : m_impl(new BrushImpl())
{
}

Brush::Brush(const Brush& brush)
  : m_impl(brush.m_impl)		// Copy shared pointers
{
}

Brush::Brush(const Color& color)
  : m_impl(new BrushImpl(color))
{
}

Brush::~Brush()
{
}

Brush& Brush::operator=(const Brush& brush)
{
  // Copy shared pointers
  m_impl = brush.m_impl;
  return *this;
}

Color Brush::getColor() const
{
  return m_impl->getColor();
}
