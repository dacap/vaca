// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_REGION_H
#define VACA_REGION_H

#include "Vaca/base.h"
#include "Vaca/GdiObject.h"
#include "Vaca/SharedPtr.h"

namespace Vaca {

/**
   A region, it can be simple as a rectangle, complex as any shape,
   but also can be empty.
*/
class VACA_DLL Region : private SharedPtr<GdiObject<HRGN> >
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
