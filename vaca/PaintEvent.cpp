// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/PaintEvent.h"
#include "vaca/Widget.h"

using namespace vaca;

PaintEvent::PaintEvent(Widget* source, Graphics& graphics)
  : Event(source)
  , m_graphics(graphics)
  , m_painted(false)
{
}

PaintEvent::~PaintEvent()
{
}

Graphics& PaintEvent::getGraphics()
{
  m_painted = true;
  return m_graphics;
}

bool PaintEvent::isPainted() const
{
  return m_painted;
}
