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

#include "Vaca/GraphicsPath.h"
#include "Vaca/Point.h"

using namespace Vaca;

GraphicsPath::GraphicsPath()
{
}

GraphicsPath::~GraphicsPath()
{
}

void GraphicsPath::closeFigure()
{
  if (!m_types.empty())
    m_types.back() |= PT_CLOSEFIGURE;
}

void GraphicsPath::moveTo(const Point& pt)
{
  moveTo(pt.x, pt.y);
}

void GraphicsPath::moveTo(int x, int y)
{
  if (!m_types.empty() && m_types.back() == PT_MOVETO) {
    m_points.back().x = x;
    m_points.back().y = y;
  }
  else
    addPoint(PT_MOVETO, x, y);
}

void GraphicsPath::lineTo(const Point& pt)
{
  lineTo(pt.x, pt.y);
}

void GraphicsPath::lineTo(int x, int y)
{
  addPoint(PT_LINETO, x, y);
}

void GraphicsPath::curveTo(const Point& pt1, const Point& pt2, const Point& pt3)
{
  curveTo(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y);
}

void GraphicsPath::curveTo(int x1, int y1, int x2, int y2, int x3, int y3)
{
  addPoint(PT_BEZIERTO, x1, y1);
  addPoint(PT_BEZIERTO, x2, y2);
  addPoint(PT_BEZIERTO, x3, y3);
}

void GraphicsPath::addPoint(BYTE type, int x, int y)
{
  POINT point = { x, y };

  m_types.push_back(type);
  m_points.push_back(point);
}
