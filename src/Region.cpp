// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#include "Vaca/Region.h"
#include "Vaca/Rect.h"
#include "Vaca/Debug.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"
#include "Vaca/win32.h"

using namespace Vaca;

Region::Region()
  : SharedPtr<GdiObject<HRGN> >(new GdiObject<HRGN>(CreateRectRgn(0, 0, 0, 0)))
{
  assert(getHandle()); // TODO exception
}

Region::Region(const Region& rgn)
  : SharedPtr<GdiObject<HRGN> >(rgn)
{
}

Region::Region(HRGN hrgn)
  : SharedPtr<GdiObject<HRGN> >(new GdiObject<HRGN>)
{
  assert(hrgn);
  get()->setHandle(hrgn);
}

Region::Region(const Rect& rc)
  : SharedPtr<GdiObject<HRGN> >(new GdiObject<HRGN>)
{
  RECT _rc = convert_to<RECT>(rc);
  get()->setHandle(CreateRectRgnIndirect(&_rc));
  assert(getHandle()); // TODO exception
}

Region::~Region()
{
}

/**
   Returns true if the Region is empty.
*/
bool Region::isEmpty() const
{
  assert(getHandle());

  RECT rc;
  return GetRgnBox(getHandle(), &rc) == NULLREGION;
}

/**
   Returns true if the Region is just a rectangle.

   @see getBounds
*/
bool Region::isSimple() const
{
  assert(getHandle());

  RECT rc;
  return GetRgnBox(getHandle(), &rc) == SIMPLEREGION;
}

/**
   Returns true if the Region is complex (a set of rectangles).
*/
bool Region::isComplex() const
{
  assert(getHandle());

  RECT rc;
  return GetRgnBox(getHandle(), &rc) == COMPLEXREGION;
}

Region& Region::operator=(const Region& rgn)
{
  SharedPtr<GdiObject<HRGN> >::operator=(rgn);
  return *this;
}

Region Region::clone() const
{
  assert(getHandle());

  // copy the this region to "rgn"
  Region copy;
  int res = CombineRgn(copy->getHandle(), getHandle(), NULL, RGN_COPY);
  if (res == ERROR) {
    assert(false);	// TODO exception
  }

  return copy;
}

/**
   Gets the bounds of the Region.

   @return An empty rectangle if the Region is empty (see
	   Region::isEmpty).
*/
Rect Region::getBounds() const
{
  assert(getHandle());

  RECT rc;
  int res = GetRgnBox(getHandle(), &rc);

  if (res == NULLREGION)
    return Rect();		// empty rectangle
  else
    return convert_to<Rect>(rc);
}

Region& Region::offset(int dx, int dy)
{
  OffsetRgn(getHandle(), dx, dy);
  return *this;
}

Region& Region::offset(const Point& point)
{
  return offset(point.x, point.y);
}

bool Region::contains(const Point& pt) const
{
  assert(getHandle());

  return PtInRegion(getHandle(), pt.x, pt.y) != FALSE;
}

bool Region::contains(const Rect& rc) const
{
  assert(getHandle());

  RECT rc2 = convert_to<RECT>(rc);
  return RectInRegion(getHandle(), &rc2) != FALSE;
}

bool Region::operator==(const Region& rgn) const
{
  BOOL res = EqualRgn(getHandle(), rgn.getHandle()) != FALSE;

  if (res == ERROR)
    return false;
  else
    return res != FALSE;
}

bool Region::operator!=(const Region& rgn) const
{
  return !operator==(rgn);
}

Region Region::operator|(const Region& rgn) const
{
  Region res;
  CombineRgn(res.getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_OR);
  return res;
}

Region Region::operator+(const Region& rgn) const
{
  return operator|(rgn);
}

Region Region::operator&(const Region& rgn) const
{
  Region res;
  CombineRgn(res.getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_AND);
  return res;
}

Region Region::operator-(const Region& rgn) const
{
  Region res;
  CombineRgn(res.getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_DIFF);
  return res;
}

Region Region::operator^(const Region& rgn) const
{
  Region res;
  CombineRgn(res.getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_XOR);
  return res;
}

/**
   Makes an union between both regions and leaves the result in
   @b this region.
*/
Region& Region::operator|=(const Region& rgn)
{
  CombineRgn(this->getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_OR);
  return *this;
}

/**
   Makes an union between both regions and leaves the result in
   @b this region.
*/
Region& Region::operator+=(const Region& rgn)
{
  return operator|=(rgn);
}

/**
   Makes the intersection between both regions and leaves
   the result in @b this region.
*/
Region& Region::operator&=(const Region& rgn)
{
  CombineRgn(this->getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_AND);
  return *this;
}

/**
   Substracts the speficied region @a rgn from @b this region.
*/
Region& Region::operator-=(const Region& rgn)
{
  CombineRgn(this->getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_DIFF);
  return *this;
}

/**
   Makes a XOR operation between both regions and leaves the result in
   @b this region.
*/
Region& Region::operator^=(const Region& rgn)
{
  CombineRgn(this->getHandle(),
	     this->getHandle(),
	     rgn.getHandle(), RGN_XOR);
  return *this;
}

/**
   Creates a region from a rectangle.
*/
Region Region::fromRect(const Rect &_rc)
{
  RECT rc = convert_to<RECT>(_rc);
  return Region(CreateRectRgnIndirect(&rc));
}

/**
   Creates a region from an ellipse.
*/
Region Region::fromEllipse(const Rect &_rc)
{
  RECT rc = convert_to<RECT>(_rc);
  return Region(CreateEllipticRgnIndirect(&rc));
}

/**
   Creates a region from a rounded rectangle.

   It uses the CreateRoundRectRgn function of Win32.
*/
Region Region::fromRoundRect(const Rect& _rc, const Size& ellipseSize)
{
  RECT rc = convert_to<RECT>(_rc);
  return Region(CreateRoundRectRgn(rc.left, rc.top,
				   rc.right, rc.bottom,
				   ellipseSize.w, ellipseSize.h));
}

/**
   Returns the Win32 region handler.
*/
HRGN Region::getHandle() const
{
  return get()->getHandle();
}
