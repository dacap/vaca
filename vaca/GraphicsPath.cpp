// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/GraphicsPath.h"
#include "Vaca/Point.h"
#include "Vaca/Region.h"
#include "Vaca/Pen.h"
#include "Vaca/Brush.h"
#include "Vaca/Graphics.h"
#include "Vaca/win32.h"

using namespace Vaca;

// ======================================================================
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

// ======================================================================
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

  HGDIOBJ oldPen = SelectObject(g.getHandle(), convert_to<HPEN>(pen));

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
