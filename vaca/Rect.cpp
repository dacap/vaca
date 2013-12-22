// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/Rect.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"

using namespace Vaca;

/**
   Creates a new empty rectangle with the origin in @c Point(0,0).

   The rectangle will be @c #x=#y=#w=#h=0

   @see isEmpty
*/
Rect::Rect()
{
  x = 0;
  y = 0;
  w = 0;
  h = 0;
}

/**
   Creates a new rectangle with the specified size with the origin in @c Point(0,0).

   The rectangle will be @c #x=#y=0.
*/
Rect::Rect(int w, int h)
{
  this->x = 0;
  this->y = 0;
  this->w = w;
  this->h = h;
}

/**
   Creates a new rectangle with the specified size with the origin in @c Point(0,0).

   The rectangle will be @c #x=#y=0.
*/
Rect::Rect(const Size& size)
{
  x = 0;
  y = 0;
  w = size.w;
  h = size.h;
}

/**
   Creates a copy of @a rect.
*/
Rect::Rect(const Rect& rect)
{
  x = rect.x;
  y = rect.y;
  w = rect.w;
  h = rect.h;
}

/**
   Creates a new rectangle with the origin in @a point
   and the specified @a size.
*/
Rect::Rect(const Point& point, const Size& size)
{
  this->x = point.x;
  this->y = point.y;
  this->w = size.w;
  this->h = size.h;
}

/**
   Creates a new rectangle with the origin in @a point1 and size equal
   to @a point2 - @a point1.

   If a coordinate of @a point1 is greater than @a point2 (Point#x
   and/or Point#y), the coordinates are swapped. So the rectangle
   will be:
   @code
   #x = MIN(point1.x, point2.x)
   #y = MIN(point1.y, point2.y)
   #w = MAX(point1.x, point2.x) - #x
   #h = MAX(point1.x, point2.x) - #y
   @endcode
   See that @a point2 isn't included in the rectangle, it's
   like the point returned by #getPoint2 member function.

   @see #getPoint2
*/
Rect::Rect(const Point& point1, const Point& point2)
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

/**
   Verifies if the width and/or height of the rectangle are less or
   equal than zero.
*/
bool Rect::isEmpty() const
{
  return (w < 1 || h < 1);
}

/**
   Returns the middle point of the rectangle (the centroid).

   @return
     Point(#x+#w/2, #y+#h/2)
*/
Point Rect::getCenter() const
{
  return Point(x+w/2, y+h/2);
}

/**
   Returns the point in the upper-left corner (that is inside the rectangle).

   @return
     Point(#x, #y)
*/
Point Rect::getOrigin() const
{
  return Point(x, y);
}

/**
   Returns point in the lower-right corner (that is outside the rectangle).

   @return
     Point(#x+#w, #y+#h)
*/
Point Rect::getPoint2() const
{
  return Point(x+w, y+h);
}

/**
   Returns the size of the rectangle.

   @return
     Size(#w, #h)
*/
Size Rect::getSize() const
{
  return Size(w, h);
}

/**
   Changes the origin of the rectangle without modifying its size.
*/
Rect& Rect::setOrigin(const Point& pt)
{
  x = pt.x;
  y = pt.y;
  return *this;
}

/**
   Changes the size of the rectangle without modifying its origin.
*/
Rect& Rect::setSize(const Size& sz)
{
  w = sz.w;
  h = sz.h;
  return *this;
}

/**
   Moves the rectangle origin in the specified delta.

   @param dx
     How many pixels displace the #x coordinate (#x+=dx).

   @param dy
     How many pixels displace the #y coordinate (#y+=dy).

   @return
     A reference to @c this.
*/
Rect& Rect::offset(int dx, int dy)
{
  x += dx;
  y += dy;
  return *this;
}

/**
   Moves the rectangle origin in the specified delta.

   @param point
     How many pixels displace the origin (#x+=point.x, #y+=point.y).

   @return
     A reference to @c this.
*/
Rect& Rect::offset(const Point& point)
{
  x += point.x;
  y += point.y;
  return *this;
}

/**
   Increases (or decreases if the delta is negative) the size of the
   rectangle.

   @param dw
     How many pixels to increase the width of the rectangle #w.

   @param dh
     How many pixels to increase the height of the rectangle #h.

   @return
     A reference to @c this.
*/
Rect& Rect::inflate(int dw, int dh)
{
  w += dw;
  h += dh;
  return *this;
}

/**
   Increases (or decreases if the delta is negative) the size of the
   rectangle.

   @param size
     How many pixels to increase the size of the
     rectangle (#w+=size.w, #h+=size.h).

   @return
     A reference to @c this.
*/
Rect& Rect::inflate(const Size& size)
{
  w += size.w;
  h += size.h;
  return *this;
}

/**
   @todo docme
   @return
     A reference to @c this.
*/
Rect& Rect::enlarge(int unit)
{
  x -= unit;
  y -= unit;
  w += unit<<1;
  h += unit<<1;
  return *this;
}

/**
   @todo docme
   @return
     A reference to @c this.
*/
Rect& Rect::shrink(int unit)
{
  x += unit;
  y += unit;
  w -= unit<<1;
  h -= unit<<1;
  return *this;
}

/**
   Returns true if this rectangle encloses the @a pt point.
*/
bool Rect::contains(const Point& pt) const
{
  return
    pt.x >= x && pt.x < x+w &&
    pt.y >= y && pt.y < y+h;
}

/**
   Returns true if this rectangle entirely contains the @a rc rectangle.

   @warning
     If some rectangle is empty, this member function returns false.
*/
bool Rect::contains(const Rect& rc) const
{
  if (isEmpty() || rc.isEmpty())
    return false;

  return
    rc.x >= x && rc.x+rc.w <= x+w &&
    rc.y >= y && rc.y+rc.h <= y+h;
}

/**
   Returns true if the intersection between this rectangle with @a rc
   rectangle is not empty.

   @warning
     If some rectangle is empty, this member function returns false.
*/
bool Rect::intersects(const Rect& rc) const
{
  if (isEmpty() || rc.isEmpty())
    return false;

  return
    rc.x <= x+w && rc.x+rc.w > x &&
    rc.y <= y+h && rc.y+rc.h > y;
}

/**
   Returns the union rectangle between this and @c rc rectangle.

   @warning
     If some rectangle is empty, this member function will return the
     other rectangle.
*/
Rect Rect::createUnion(const Rect& rc) const
{
  if (isEmpty())
    return rc;
  else if (rc.isEmpty())
    return *this;
  else
    return Rect(Point(x < rc.x ? x: rc.x,
		      y < rc.y ? y: rc.y),
		Point(x+w > rc.x+rc.w ? x+w: rc.x+rc.w,
		      y+h > rc.y+rc.h ? y+h: rc.y+rc.h));
}

/**
   Returns the intersection rectangle between this and @c rc rectangles.
*/
Rect Rect::createIntersect(const Rect& rc) const
{
  if (intersects(rc))
    return Rect(Point(x > rc.x ? x: rc.x,
		      y > rc.y ? y: rc.y),
		Point(x+w < rc.x+rc.w ? x+w: rc.x+rc.w,
		      y+h < rc.y+rc.h ? y+h: rc.y+rc.h));
  else
    return Rect();
}

bool Rect::operator==(const Rect& rc) const
{
  return
    x == rc.x && w == rc.w &&
    y == rc.y && h == rc.h;
}

bool Rect::operator!=(const Rect& rc) const
{
  return
    x != rc.x || w != rc.w ||
    y != rc.y || h != rc.h;
}
