// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#ifndef VACA_REGION_H
#define VACA_REGION_H

#include "Vaca/base.h"
#include "Vaca/GdiObject.h"
#include "Vaca/SmartPtr.h"

namespace Vaca {

class Point;
class Rect;
class Size;

/**
 * A region, it can be simple as a rectangle, complex as any shape,
 * but also can be empty.
 */
class VACA_DLL Region : private SmartPtr<GdiObject<HRGN> >
{
public:

  Region();
  Region(const Region& rgn);
  explicit Region(HRGN hrgn);
  explicit Region(const Rect& rc);
  virtual ~Region();

  bool isEmpty() const;
  bool isSimple() const;
  bool isComplex() const;

  Region& operator=(const Region& rgn);
  Region clone() const;

  Rect getBounds() const;

  Region& offset(int dx, int dy);
  Region& offset(const Point& point);

  bool contains(const Point& pt) const;
  bool contains(const Rect& rc) const;

  bool operator==(const Region& rgn) const;
  bool operator!=(const Region& rgn) const;

  Region operator|(const Region& rgn) const;
  Region operator+(const Region& rgn) const;
  Region operator&(const Region& rgn) const;
  Region operator-(const Region& rgn) const;
  Region operator^(const Region& rgn) const;

  Region& operator|=(const Region& rgn);
  Region& operator+=(const Region& rgn);
  Region& operator&=(const Region& rgn);
  Region& operator-=(const Region& rgn);
  Region& operator^=(const Region& rgn);

  static Region fromRect(const Rect& rc);
  static Region fromEllipse(const Rect& rc);
  static Region fromRoundRect(const Rect& rc, const Size& ellipseSize);

  HRGN getHandle() const;
  
};

} // namespace Vaca

#endif // VACA_REGION_H

