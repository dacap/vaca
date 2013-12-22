// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/Size.h"
#include "Vaca/Point.h"

using namespace Vaca;

Size::Size()
{
  w = 0;
  h = 0;
}

Size::Size(int w, int h)
{
  this->w = w;
  this->h = h;
}

Size::Size(const Size& size)
{
  w = size.w;
  h = size.h;
}

Size::Size(const Point& point)
{
  w = point.x;
  h = point.y;
}

Size Size::createUnion(const Size& sz) const
{
  return Size(max_value(w, sz.w),
	      max_value(h, sz.h));
}

Size Size::createIntersect(const Size& sz) const
{
  return Size(min_value(w, sz.w),
	      min_value(h, sz.h));
}

const Size& Size::operator=(const Size& sz)
{
  w = sz.w;
  h = sz.h;
  return *this;
}

const Size& Size::operator+=(const Size& sz)
{
  w += sz.w;
  h += sz.h;
  return *this;
}

const Size& Size::operator-=(const Size& sz)
{
  w -= sz.w;
  h -= sz.h;
  return *this;
}

const Size& Size::operator+=(int value)
{
  w += value;
  h += value;
  return *this;
}

const Size& Size::operator-=(int value)
{
  w -= value;
  h -= value;
  return *this;
}

const Size& Size::operator*=(int value)
{
  w *= value;
  h *= value;
  return *this;
}

const Size& Size::operator/=(int value)
{
  w /= value;
  h /= value;
  return *this;
}

Size Size::operator+(const Size& sz) const
{
  return Size(w+sz.w, h+sz.h);
}

Size Size::operator-(const Size& sz) const
{
  return Size(w-sz.w, h-sz.h);
}

Size Size::operator+(int value) const
{
  return Size(w+value, h+value);
}

Size Size::operator-(int value) const
{
  return Size(w-value, h-value);
}

Size Size::operator*(int value) const
{
  return Size(w*value, h*value);
}

Size Size::operator/(int value) const
{
  return Size(w/value, h/value);
}

Size Size::operator-() const
{
  return Size(-w, -h);
}

bool Size::operator==(const Size& sz) const
{
  return w == sz.w && h == sz.h;
}

bool Size::operator!=(const Size& sz) const
{
  return w != sz.w || h != sz.h;
}
