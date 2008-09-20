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

#ifndef VACA_GRAPHICSPATH_H
#define VACA_GRAPHICSPATH_H

#include <vector>

#include "Vaca/base.h"
#include "Vaca/Point.h"

namespace Vaca {

class Pen;
class Region;

/**
 * Set of nodes to draw polygons and shapes in Graphics.
 */
class VACA_DLL GraphicsPath
{
public:
  // values that are acceptable for Node#m_flags
  enum {
    MoveTo = 1,
    LineTo = 2,
    BezierControl1 = 3,		// first bezier's control point
    BezierControl2 = 4,		// second bezier's control point
    BezierTo = 5,		// bezier's end point
    TypeMask = 7,
    // flags
    CloseFigure = 8,
  };

  /**
   * A node in a GraphicsPath.
   *
   * A node is an action to draw the GraphicsPath, it can be a node of
   * movement (MoveTo), a node to draw a line (LineTo), or a curve
   * (BezierTo).
   *
   * The nodes of movement don't draw lines, they just change the
   * current position which we have to start to draw. For example,
   * a GraphicsPath that represent just one line, has really two nodes:
   * @li a node of movement (MoveTo) to go to the line's start position, and
   * @li a node to draw a line (LineTo) that represent the end position of the line.
   */
  class VACA_DLL Node
  {
    friend class GraphicsPath;
    int m_flags;
    Point m_point;
  public:
    Node(int type, const Point& point);

    int getType() const;
    bool isCloseFigure() const;
    void setCloseFigure(bool closeFigure);
    Point& getPoint();
    const Point& getPoint() const;
  };

private:
  std::vector<Node> m_nodes;

public:
  typedef std::vector<Node>::iterator iterator;
  typedef std::vector<Node>::const_iterator const_iterator;
  
  GraphicsPath();
  virtual ~GraphicsPath();

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  void clear();
  bool empty() const;
  unsigned size() const;

  GraphicsPath& offset(int dx, int dy);
  GraphicsPath& offset(const Point& point);

  GraphicsPath& moveTo(const Point& pt);
  GraphicsPath& moveTo(int x, int y);

  GraphicsPath& lineTo(const Point& pt);
  GraphicsPath& lineTo(int x, int y);

  GraphicsPath& curveTo(const Point& pt1, const Point& pt2, const Point& pt3);
  GraphicsPath& curveTo(int x1, int y1, int x2, int y2, int x3, int y3);

  GraphicsPath& closeFigure();

  GraphicsPath& flatten();
  GraphicsPath& widen(const Pen& pen);

  Region toRegion() const;

private:
  void addNode(int type, const Point& pt);

};

} // namespace Vaca

#endif // VACA_GRAPHICSPATH_H
