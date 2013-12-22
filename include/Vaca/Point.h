// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_POINT_H
#define VACA_POINT_H

#include "Vaca/base.h"

namespace Vaca {

/**
   A 2D coordinate in the screen or client area of a widget.
*/
class VACA_DLL Point
{
public:

  int x, y;

  Point();
  Point(int x, int y);
  Point(const Point& point);
  explicit Point(const Size& size);

  const Point& operator=(const Point& pt);
  const Point& operator+=(const Point& pt);
  const Point& operator-=(const Point& pt);
  const Point& operator+=(int value);
  const Point& operator-=(int value);
  const Point& operator*=(int value);
  const Point& operator/=(int value);
  Point operator+(const Point& pt) const;
  Point operator-(const Point& pt) const;
  Point operator+(int value) const;
  Point operator-(int value) const;
  Point operator*(int value) const;
  Point operator/(int value) const;
  Point operator-() const;

  bool operator==(const Point& pt) const;
  bool operator!=(const Point& pt) const;

};

} // namespace Vaca

#endif // VACA_POINT_H
