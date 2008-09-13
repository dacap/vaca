// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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
// * Neither the name of the Vaca nor the names of its contributors
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

#include "Vaca/Brush.h"

using namespace Vaca;

Brush::Brush(const Color& color)
{
  initialize();
  m_color = color;
}

Brush::Brush(const Brush& brush)
{
  initialize();
  assign(brush);
}

Brush::~Brush()
{
  destroy();
}

Brush& Brush::operator=(const Brush& brush)
{
  assign(brush);
  return *this;
}

Color Brush::getColor() const
{
  return m_color;
}

void Brush::setColor(const Color& color)
{
  if (m_color != color) {
    m_color = color;
    m_modified = true;
  }
}

HBRUSH Brush::getHandle()
{
  if (m_modified || m_handle == NULL) {
    m_modified = false;
    destroy();

    m_handle = CreateSolidBrush(m_color.getColorRef());
  }
  return m_handle;
}

void Brush::initialize()
{
  m_handle = NULL;
  m_modified = false;
}

void Brush::assign(const Brush& brush)
{
  destroy();

  m_handle = NULL;
  m_modified = true;
  m_color = brush.m_color;
}

void Brush::destroy()
{
  if (m_handle) {
    DeleteObject(reinterpret_cast<HGDIOBJ>(m_handle));
    m_handle = NULL;
  }
}
