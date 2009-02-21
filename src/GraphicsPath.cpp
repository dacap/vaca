// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#include "Vaca/GraphicsPath.h"
#include "Vaca/Point.h"
#include "Vaca/Region.h"
#include "Vaca/Pen.h"
#include "Vaca/Brush.h"
#include "Vaca/Graphics.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
//


GraphicsPath::Node::Node(int type, const Point& point)
  : m_flags(type & TypeMask)
  , m_point(point)
{
}

int GraphicsPath::Node::getType() const
{
  return m_flags & TypeMask;
}

bool GraphicsPath::Node::isCloseFigure() const
{
  return m_flags & CloseFigure ? true: false;
}

void GraphicsPath::Node::setCloseFigure(bool state)
{
  if (state)
    m_flags |= CloseFigure;
  else
    m_flags &= ~CloseFigure;
}

Point& GraphicsPath::Node::getPoint()
{
  return m_point;
}

const Point& GraphicsPath::Node::getPoint() const
{
  return m_point;
}

//////////////////////////////////////////////////////////////////////
// GraphicsPath

GraphicsPath::GraphicsPath()
{
}

GraphicsPath::~GraphicsPath()
{
}

GraphicsPath::iterator GraphicsPath::begin()
{
  return m_nodes.begin();
}

GraphicsPath::iterator GraphicsPath::end()
{
  return m_nodes.end();
}

GraphicsPath::const_iterator GraphicsPath::begin() const
{
  return m_nodes.begin();
}

GraphicsPath::const_iterator GraphicsPath::end() const
{
  return m_nodes.end();
}

void GraphicsPath::clear()
{
  m_nodes.clear();
}

bool GraphicsPath::empty() const
{
  return m_nodes.empty();
}

unsigned GraphicsPath::size() const
{
  return m_nodes.size();
}

GraphicsPath& GraphicsPath::offset(int dx, int dy)
{
  for (iterator it=begin(); it!=end(); ++it) {
    it->getPoint().x += dx;
    it->getPoint().y += dy;
  }
  return *this;
}

GraphicsPath& GraphicsPath::offset(const Point& point)
{
  return offset(point.x, point.y);
}

GraphicsPath& GraphicsPath::moveTo(const Point& pt)
{
  if (!m_nodes.empty() && m_nodes.back().getType() == GraphicsPath::MoveTo)
    m_nodes.back().m_point = pt;
  else
    addNode(GraphicsPath::MoveTo, pt);
  return *this;
}

GraphicsPath& GraphicsPath::moveTo(int x, int y)
{
  moveTo(Point(x, y));
  return *this;
}

GraphicsPath& GraphicsPath::lineTo(const Point& pt)
{
  addNode(GraphicsPath::LineTo, pt);
  return *this;
}

GraphicsPath& GraphicsPath::lineTo(int x, int y)
{
  lineTo(Point(x, y));
  return *this;
}

GraphicsPath& GraphicsPath::curveTo(const Point& pt1, const Point& pt2, const Point& pt3)
{
  addNode(GraphicsPath::BezierControl1, pt1);
  addNode(GraphicsPath::BezierControl2, pt2);
  addNode(GraphicsPath::BezierTo, pt3);
  return *this;
}

GraphicsPath& GraphicsPath::curveTo(int x1, int y1, int x2, int y2, int x3, int y3)
{
  curveTo(Point(x1, y1), Point(x2, y2), Point(x3, y3));
  return *this;
}

GraphicsPath& GraphicsPath::closeFigure()
{
  if (!m_nodes.empty())
    m_nodes.back().m_flags |= GraphicsPath::CloseFigure;
  return *this;
}

GraphicsPath& GraphicsPath::flatten()
{
  ScreenGraphics g;
  g.tracePath(*this, Point(0, 0));
  ::FlattenPath(g.getHandle());
  g.getPath(*this);
  return *this;
}

GraphicsPath& GraphicsPath::widen(const Pen& pen)
{
  ScreenGraphics g;

  HGDIOBJ oldPen = SelectObject(g.getHandle(), pen.getHandle());

  g.tracePath(*this, Point(0, 0));
  ::WidenPath(g.getHandle());
  g.getPath(*this);

  SelectObject(g.getHandle(), oldPen);
  return *this;
}

Region GraphicsPath::toRegion() const
{
  ScreenGraphics g;
  g.tracePath(*this, Point(0, 0));
  return g.getRegionFromPath();
}

void GraphicsPath::addNode(int type, const Point& pt)
{
  m_nodes.push_back(Node(type, pt));
}
