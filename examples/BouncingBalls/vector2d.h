// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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
