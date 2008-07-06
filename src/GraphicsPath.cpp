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

#include "Vaca/GraphicsPath.hpp"

#if 0

GraphicsPath::GraphicsPath()
{
  mCurrentX = 0.0f;
  mCurrentY = 0.0f;
  mCurrentFigure = NULL;

  startFigure();
}

GraphicsPath::~GraphicsPath()
{
  for (std::list<Figure*>::iterator
	 fig = mFigures.begin(); fig != mFigures.end() ++fig) {
    for (std::list<Segment*>::iterator
	   seg = (*fig)->segments.begin(); seg != (*fig)->segments.end() ++seg)
      delete *seg;

    delete *fig;
  }
}

void GraphicsPath::startFigure()
{
  Figure* figure = new Figure;

  figure->start_x = mCurrentX;
  figure->start_y = mCurrentY;
  figure->closed = false;

  mCurrentFigure = figure;
  mFigures.push_back(mCurrentFigure);
}

void GraphicsPath::closeFigure()
{
}

void GraphicsPath::moveTo(const Point& pt)
{
  moveTo(static_cast<float>(pt.x),
	 static_cast<float>(pt.y));
}

void GraphicsPath::moveTo(int x, int y)
{
  moveTo(static_cast<float>(x),
	 static_cast<float>(y));
}

void GraphicsPath::moveTo(float x, float y)
{
  mCurrentX = x;
  mCurrentY = y;
}

void GraphicsPath::lineTo(const Point& pt)
{
  lineTo(static_cast<float>(pt.x),
	 static_cast<float>(pt.y));
}

void GraphicsPath::lineTo(int x, int y)
{
  lineTo(static_cast<float>(x),
	 static_cast<float>(y));
}

void GraphicsPath::lineTo(float x, float y)
{
  mCurrentX = x;
  mCurrentY = y;
}

void GraphicsPath::curveTo(int x1, int y1, int x2, int y2, int x3, int y3)
{
}

void GraphicsPath::curveTo(float x1, float y1, float x2, float y2, float x3, float y3)
{
}

void GraphicsPath::curveTo(const Point& pt1, const Point& pt2, const Point& pt3)
{
}

void GraphicsPath::curveTo(const std::vector<Point> &points)
{
}

#endif
