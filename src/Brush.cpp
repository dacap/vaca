// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include "stdvaca.h"
#include "Vaca/Brush.hpp"

using namespace Vaca;

Brush::Brush(Color color)
{
  initialize();
  m_color = color;
}

Brush::~Brush()
{
  destroy();
}

Color Brush::getColor() const
{
  return m_color;
}

void Brush::setColor(Color color)
{
  if (m_color != color) {
    m_color = color;
    m_modified = true;
  }
}

HBRUSH Brush::getHBRUSH()
{
  if (m_modified || m_HBRUSH == NULL) {
    m_modified = false;
    destroy();

    m_HBRUSH = CreateSolidBrush(m_color.getColorRef());
  }
  return m_HBRUSH;
}

void Brush::initialize()
{
  m_HBRUSH = NULL;
  m_modified = false;
}

void Brush::destroy()
{
  if (m_HBRUSH != NULL) {
    DeleteObject(reinterpret_cast<HGDIOBJ>(m_HBRUSH));
    m_HBRUSH = NULL;
  }
}
