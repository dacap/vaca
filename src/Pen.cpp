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

#include "Vaca/Pen.h"

using namespace Vaca;

/**
 * Creates a pen.
 *
 * @param color Color of the pen.
 * @param width Width of the pen. If it's 1, the pen will be cosmetic,
 *              if width > 0 the pen will be geometric.
 */
Pen::Pen(const Color& color, int width)
{
  initialize();
  m_color = color;
  m_width = width;
  m_style = PenStyle::Solid;
}

Pen::Pen(const Pen& pen)
{
  initialize();
  assign(pen);
}

/**
 * Destroys the pen. Uses Win32's DeleteObject.
 */
Pen::~Pen()
{
  destroy();
}

Pen& Pen::operator=(const Pen& pen)
{
  assign(pen);
  return *this;
}

/**
 * Gets pen's color.
 * 
 * @return Current pen's color.
 */
Color Pen::getColor() const
{
  return m_color;
}

/**
 * Sets pen's color.
 *
 * @param color New color for the pen.
 */
void Pen::setColor(const Color& color)
{
  if (m_color != color) {
    m_color = color;
    m_modified = true;
  }
}

/**
 * Gets pen's width (in pixels).
 * 
 * @return Current pen's width.
 */
int Pen::getWidth() const
{
  return m_width;
}

/**
 * Sets pen's width (in pixels).
 *
 * @param width New width for the pen.
 */
void Pen::setWidth(int width)
{
  if (m_width != width) {
    m_width = width;
    m_modified = true;
  }
}

/**
 * Gets pen's style.
 *
 * @return Current pen's style.
 */
PenStyle Pen::getStyle() const
{
  return m_style;
}

/**
 * Sets pen's style.
 * 
 * @param style New style for the pen.
 */
void Pen::setStyle(PenStyle style)
{
  if (m_style != style) {
    m_style = style;
    m_modified = true;
  }
}

PenEndCap Pen::getEndCap() const
{
  return m_endCap;
}

void Pen::setEndCap(PenEndCap endCap)
{
  if (m_endCap != endCap) {
    m_endCap = endCap;
    m_modified = true;
  }
}

PenJoin Pen::getJoin() const
{
  return m_join;
}

void Pen::setJoin(PenJoin join)
{
  if (m_join != join) {
    m_join = join;
    m_modified = true;
  }
}

/**
 * Returns the handler used by Win32 use the pen in graphics
 * primitives.
 */
HPEN Pen::getHandle()
{
  if (m_modified || m_handle == NULL) {
    m_modified = false;
    destroy();

    int style = 0;

    // pen style
    switch (m_style) {
      case PenStyle::Solid:       style |= PS_SOLID;       break;
      case PenStyle::Dash:        style |= PS_DASH;        break;
      case PenStyle::Dot:         style |= PS_DOT;         break;
      case PenStyle::DashDot:     style |= PS_DASHDOT;     break;
      case PenStyle::DashDotDot:  style |= PS_DASHDOTDOT;  break;
      case PenStyle::Null:        style |= PS_NULL;        break;
      case PenStyle::InsideFrame: style |= PS_INSIDEFRAME; break;
    }

    // end cap style
    switch (m_endCap) {
      case PenEndCap::Round:  style |= PS_ENDCAP_ROUND;  break;
      case PenEndCap::Square: style |= PS_ENDCAP_SQUARE; break;
      case PenEndCap::Flat:   style |= PS_ENDCAP_FLAT;   break;
    }

    // join style
    switch (m_join) {
      case PenJoin::Round: style |= PS_JOIN_ROUND; break;
      case PenJoin::Bevel: style |= PS_JOIN_BEVEL; break;
      case PenJoin::Miter: style |= PS_JOIN_MITER; break;
    }

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    lb.lbColor = m_color.getColorRef();
    lb.lbHatch = 0;

    if (m_width == 1)
      m_handle = ExtCreatePen(PS_COSMETIC | style, m_width, &lb, 0, NULL);
    else
      m_handle = ExtCreatePen(PS_GEOMETRIC | style, m_width, &lb, 0, NULL);
  }
  return m_handle;
}

void Pen::initialize()
{
  m_handle = NULL;
  m_modified = false;
}

void Pen::assign(const Pen& pen)
{
  destroy();

  m_handle = NULL;
  m_modified = true;
  m_color = pen.m_color;
  m_width = pen.m_width;
  m_style = pen.m_style;
  m_endCap = pen.m_endCap;
  m_join = pen.m_join;
}

void Pen::destroy()
{
  if (m_handle) {
    DeleteObject(reinterpret_cast<HGDIOBJ>(m_handle));
    m_handle = NULL;
  }
}
