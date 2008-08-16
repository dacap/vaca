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

#ifndef VACA_GRAPHICSPATH_HPP
#define VACA_GRAPHICSPATH_HPP

#include <list>
#include <vector>

#include "Vaca/base.h"

namespace Vaca {

class Point;

class VACA_DLL GraphicsPath
{

  struct Segment
  {
    float x, y;
  };

  struct Figure
  {
    float start_x, start_y;
    bool closed;
    std::list<Segment*> segments;
  };

  std::list<Figure*> m_figures;
  float m_currentX, m_currentY;
  Figure* m_currentFigure;

public:

  GraphicsPath();
  virtual ~GraphicsPath();

  void startFigure();
  void closeFigure();

  void moveTo(const Point& pt);
  void moveTo(float x, float y);
  void lineTo(const Point& pt);
  void lineTo(float x, float y);
  void curveTo(float x1, float y1, float x2, float y2, float x3, float y3);
  void curveTo(const Point& pt1, const Point& pt2, const Point& pt3);
  void curveTo(const std::vector<Point>& points);
  
};

} // namespace Vaca

#endif
