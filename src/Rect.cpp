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
#include "Vaca/Rect.hpp"
#include "Vaca/Point.hpp"
#include "Vaca/Size.hpp"

using namespace Vaca;

/**
 * Creates a new empty rectangle (@c w=0 and @c h=0)
 */
Rect::Rect()
{
  x = 0;
  y = 0;
  w = 0;
  h = 0;
}

Rect::Rect(int w, int h)
{
  this->x = 0;
  this->y = 0;
  this->w = w;
  this->h = h;
}

Rect::Rect(const Size &size)
{
  x = 0;
  y = 0;
  w = size.w;
  h = size.h;
}

Rect::Rect(const Rect &rect)
{
  x = rect.x;
  y = rect.y;
  w = rect.w;
  h = rect.h;
}

Rect::Rect(const Point &point, const Size &size)
{
  this->x = point.x;
  this->y = point.y;
  this->w = size.w;
  this->h = size.h;
}

Rect::Rect(const Point &point1, const Point &point2)
{
  Point leftTop = point1;
  Point rightBottom = point2;
  register int t;

  if (leftTop.x > rightBottom.x) {
    t = leftTop.x;
    leftTop.x = rightBottom.x;
    rightBottom.x = t;
  }

  if (leftTop.y > rightBottom.y) {
    t = leftTop.y;
    leftTop.y = rightBottom.y;
    rightBottom.y = t;
  }
  
  this->x = leftTop.x;
  this->y = leftTop.y;
  this->w = rightBottom.x - leftTop.x;
  this->h = rightBottom.y - leftTop.y;
}

Rect::Rect(int x, int y, int w, int h)
{
  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;
}

bool Rect::isEmpty() const
{
  return (w < 1 || h < 1);
}

Point Rect::getCenter() const
{
  return Point(x+w/2, y+h/2);
}

Point Rect::getOrigin() const
{
  return Point(x, y);
}

Size Rect::getSize() const
{
  return Size(w, h);
}

Rect &Rect::setOrigin(const Point &pt)
{
  x = pt.x;
  y = pt.y;
  return *this;
}

Rect &Rect::setSize(const Size &sz)
{
  w = sz.w;
  h = sz.h;
  return *this;
}

Rect &Rect::offset(int dx, int dy)
{
  x += dx;
  y += dy;
  return *this;
}

Rect &Rect::offset(const Point &point)
{
  x += point.x;
  y += point.y;
  return *this;
}

Rect &Rect::inflate(int dw, int dh)
{
  w += dw;
  h += dh;
  return *this;
}

Rect &Rect::inflate(const Size &size)
{
  w += size.w;
  h += size.h;
  return *this;
}

Rect &Rect::enlarge(int unit)
{
  x -= unit;
  y -= unit;
  w += unit<<1;
  h += unit<<1;
  return *this;
}

Rect &Rect::shrink(int unit)
{
  x += unit;
  y += unit;
  w -= unit<<1;
  h -= unit<<1;
  return *this;
}

bool Rect::contains(const Point &pt) const
{
  // TODO empty stuff
  return
    pt.x >= x && pt.x < x+w && 
    pt.y >= y && pt.y < y+h;
}

/**
 * This rectangle entirely contains the @c rc rectangle.
 */
bool Rect::contains(const Rect &rc) const
{
  // TODO empty stuff
  // TODO test it
  return
    rc.x >= x && rc.x+rc.w <= x+w && 
    rc.y >= y && rc.y+rc.h <= y+h;
}

/**
 * Returns true if the intersection between this rectangle with @c rc
 * rectangle is not empty.
 */
bool Rect::intersects(const Rect &rc) const
{
  if (isEmpty() || rc.isEmpty())
    return false;

  // TODO test it
  return
    rc.x <= x+w && rc.x+rc.w > x && 
    rc.y <= y+h && rc.y+rc.h > y;
}

/**
 * Returns the union rectangle between this and @c rc rectangles.
 */
Rect Rect::createUnion(const Rect &rc) const
{
  if (isEmpty())
    return rc;
  else if (rc.isEmpty())
    return *this;
  else
    return Rect(Point(x < rc.x ? x: rc.x,
		      y < rc.y ? y: rc.y),
		Point(x+w > rc.x+rc.w ? x+w-1: rc.x+rc.w-1,
		      y+h > rc.y+rc.h ? y+h-1: rc.y+rc.h-1));
}

/**
 * Returns the intersection rectangle between this and @c rc rectangles.
 */
Rect Rect::createIntersect(const Rect &rc) const
{
  if (intersects(rc))
    return Rect(Point(x > rc.x ? x: rc.x,
		      y > rc.y ? y: rc.y),
		Point(x+w < rc.x+rc.w ? x+w-1: rc.x+rc.w-1,
		      y+h < rc.y+rc.h ? y+h-1: rc.y+rc.h-1));
  else
    return Rect();
}

bool Rect::operator==(const Rect &rc) const
{
  // TODO empty stuff
  return
    x == rc.x && w == rc.w &&
    y == rc.y && h == rc.h;
}

bool Rect::operator!=(const Rect &rc) const
{
  // TODO empty stuff
  return
    x != rc.x || w != rc.w ||
    y != rc.y || h != rc.h;
}

Rect::Rect(LPCRECT rc)
{
  x = rc->left;
  y = rc->top;
  w = rc->right-rc->left;
  h = rc->bottom-rc->top;
}

Rect::operator RECT() const
{
  RECT rc;
  rc.left = x;
  rc.top = y;
  rc.right = x+w;
  rc.bottom = y+h;
  return rc;
}
