// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Point.h"
#include "vaca/Size.h"

using namespace vaca;

Point::Point()
{
  x = 0;
  y = 0;
}

Point::Point(int x, int y)
{
  this->x = x;
  this->y = y;
}

Point::Point(const Point& point)
{
  x = point.x;
  y = point.y;
}

Point::Point(const Size& size)
{
  x = size.w;
  y = size.h;
}

const Point& Point::operator=(const Point& pt)
{
  x = pt.x;
  y = pt.y;
  return *this;
}

const Point& Point::operator+=(const Point& pt)
{
  x += pt.x;
  y += pt.y;
  return *this;
}

const Point& Point::operator-=(const Point& pt)
{
  x -= pt.x;
  y -= pt.y;
  return *this;
}

const Point& Point::operator+=(int value)
{
  x += value;
  y += value;
  return *this;
}

const Point& Point::operator-=(int value)
{
  x -= value;
  y -= value;
  return *this;
}

const Point& Point::operator*=(int value)
{
  x *= value;
  y *= value;
  return *this;
}

const Point& Point::operator/=(int value)
{
  x /= value;
  y /= value;
  return *this;
}

Point Point::operator+(const Point& pt) const
{
  return Point(x+pt.x, y+pt.y);
}

Point Point::operator-(const Point& pt) const
{
  return Point(x-pt.x, y-pt.y);
}

Point Point::operator+(int value) const
{
  return Point(x+value, y+value);
}

Point Point::operator-(int value) const
{
  return Point(x-value, y-value);
}

Point Point::operator*(int value) const
{
  return Point(x*value, y*value);
}

Point Point::operator/(int value) const
{
  return Point(x/value, y/value);
}

Point Point::operator-() const
{
  return Point(-x, -y);
}

bool Point::operator==(const Point& pt) const
{
  return x == pt.x && y == pt.y;
}

bool Point::operator!=(const Point& pt) const
{
  return x != pt.x || y != pt.y;
}
