// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VECTOR2D
#define VECTOR2D

#include <cmath>

#define v2dfloat float
// #define v2dfloat double

class vector2d
{
public:

  v2dfloat x, y;

  vector2d()
  { x = y = 0.0; }

  vector2d(v2dfloat u, v2dfloat v)
  { x = u; y = v; }

  vector2d(const vector2d& v)
  { x = v.x; y = v.y; }

  vector2d operator-() const { return vector2d(-x, -y); }

  vector2d operator+(const vector2d& v) const { return vector2d(x+v.x, y+v.y); }
  vector2d operator-(const vector2d& v) const { return vector2d(x-v.x, y-v.y); }
  v2dfloat operator*(const vector2d& v) const { return dot_product(v); }
  vector2d operator*(      v2dfloat  f) const { return vector2d(x*f, y*f); }
  vector2d operator/(      v2dfloat  f) const { return vector2d(x/f, y/f); }

  vector2d& operator= (const vector2d& v) { x=v.x; y=v.y; return *this; }
  vector2d& operator+=(const vector2d& v) { x+=v.x; y+=v.y; return *this; }
  vector2d& operator-=(const vector2d& v) { x-=v.x; y-=v.y; return *this; }
  vector2d& operator*=(      v2dfloat  f) { x*=f; y*=f; return *this; }
  vector2d& operator/=(      v2dfloat  f) { x/=f; y/=f; return *this; }

  v2dfloat magnitude() const
  { return std::sqrt(x*x + y*y); }

  v2dfloat dot_product(const vector2d& v) const
  { return x*v.x + y*v.y; }

  vector2d project_on(const vector2d& v) const
  { return v * (this->dot_product(v) / std::pow(v.magnitude(), 2)); }

  v2dfloat angle() const
  { return std::atan2(y, x); }

  vector2d normalize() const { return vector2d(*this) / magnitude(); }

  vector2d get_normal() const { return vector2d(y, -x); }

};

vector2d operator*(v2dfloat f, const vector2d& v) { return vector2d(v.x*f, v.y*f); }

#endif
