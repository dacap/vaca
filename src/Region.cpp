// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include "Vaca/Region.h"
#include "Vaca/Rect.h"
#include "Vaca/Debug.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"

using namespace Vaca;

Region::Region()
{
  m_HRGN = CreateRectRgn(0, 0, 0, 0);
  m_selfDestruction = true;
  
  assert(m_HRGN != NULL); // TODO exception
}

Region::Region(HRGN hrgn , SelfDestruction selfDestruction)
{
  assert(hrgn != NULL);
  
  m_HRGN = hrgn;
  m_selfDestruction = selfDestruction.isEnabled();
}

Region::Region(const Rect &_rc)
{
  RECT rc = _rc;

  m_HRGN = CreateRectRgnIndirect(&rc);
  m_selfDestruction = true;

  assert(m_HRGN != NULL); // TODO exception
}

Region::Region(const Region& rgn)
{
  m_HRGN = NULL;
  assign(rgn);
}

Region::~Region()
{
  assert(m_HRGN != NULL);
  destroy();
}

/** 
 * Returns true if the Region is empty.
 */
bool Region::isEmpty() const
{
  assert(m_HRGN != NULL);

  RECT rc;
  return GetRgnBox(m_HRGN, &rc) == NULLREGION;
}

/** 
 * Returns true if the Region is just a rectangle.
 *
 * @see getBounds
 */
bool Region::isSimple() const
{
  assert(m_HRGN != NULL);

  RECT rc;
  return GetRgnBox(m_HRGN, &rc) == SIMPLEREGION;
}

/** 
 * Returns true if the Region is complex (a set of rectangles).
 */
bool Region::isComplex() const
{
  assert(m_HRGN != NULL);

  RECT rc;
  return GetRgnBox(m_HRGN, &rc) == COMPLEXREGION;
}

Region& Region::operator=(const Region& rgn)
{
  assign(rgn);
  return *this;
}

void Region::assign(const Region& rgn)
{
  assert(rgn.m_HRGN != NULL);

  // delete old region
  destroy();

  // create a new empty region
  m_HRGN = CreateRectRgn(0, 0, 0, 0);
  m_selfDestruction = true;
  
  assert(m_HRGN != NULL);	// TODO exception

  // copy the "rgn" to this region
  int res = CombineRgn(m_HRGN,
		       rgn.m_HRGN,
		       rgn.m_HRGN, RGN_COPY);
  if (res == ERROR) {
    assert(false);	// TODO exception
  }
}

void Region::assign(HRGN hrgn, SelfDestruction selfDestruction)
{
  assert(hrgn != NULL);

  // delete old region
  destroy();

  // assign the new one
  m_HRGN = hrgn;
  m_selfDestruction = selfDestruction.isEnabled();
}

/** 
 * Gets the bounds of the Region.
 * 
 * @return An empty rectangle if the Region is empty (see
 *         Region::isEmpty).
 */
Rect Region::getBounds() const
{
  assert(m_HRGN != NULL);

  RECT rc;
  int res = GetRgnBox(m_HRGN, &rc);

  if (res == NULLREGION)
    return Rect();		// empty rectangle
  else
    return Rect(&rc);
}

Region& Region::offset(int dx, int dy)
{
  OffsetRgn(getHRGN(), dx, dy);
  return *this;
}

Region& Region::offset(const Point& point)
{
  return offset(point.x, point.y);
}

bool Region::contains(const Point& pt) const
{
  assert(m_HRGN != NULL);
	  
  return PtInRegion(const_cast<HRGN>(m_HRGN), pt.x, pt.y) != FALSE;
}

bool Region::contains(const Rect& rc) const
{
  assert(m_HRGN != NULL);

  RECT rc2 = rc;
  return RectInRegion(const_cast<HRGN>(m_HRGN), &rc2) != FALSE;
}

bool Region::operator==(const Region& rgn) const
{
  BOOL res = EqualRgn(m_HRGN, rgn.m_HRGN) != FALSE;

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
  CombineRgn(res.m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_OR);
  return res;
}

Region Region::operator+(const Region& rgn) const
{
  return operator|(rgn);
}

Region Region::operator&(const Region& rgn) const
{
  Region res;
  CombineRgn(res.m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_AND);
  return res;
}

Region Region::operator-(const Region& rgn) const
{
  Region res;
  CombineRgn(res.m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_DIFF);
  return res;
}

Region Region::operator^(const Region& rgn) const
{
  Region res;
  CombineRgn(res.m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_XOR);
  return res;
}

/**
 * Makes an union between both regions and leaves the result in
 * @b this region.
 */
Region& Region::operator|=(const Region& rgn)
{
  CombineRgn(this->m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_OR);
  return *this;
}

/**
 * Makes an union between both regions and leaves the result in
 * @b this region.
 */
Region& Region::operator+=(const Region& rgn)
{
  return operator|=(rgn);
}

/**
 * Makes the intersection between both regions and leaves
 * the result in @b this region.
 */
Region& Region::operator&=(const Region& rgn)
{
  CombineRgn(this->m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_AND);
  return *this;
}

/**
 * Substracts the speficied region @a rgn from @b this region.
 */
Region& Region::operator-=(const Region& rgn)
{
  CombineRgn(this->m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_DIFF);
  return *this;
}

/**
 * Makes a XOR operation between both regions and leaves the result in
 * @b this region.
 */
Region& Region::operator^=(const Region& rgn)
{
  CombineRgn(this->m_HRGN,
	     this->m_HRGN,
	     rgn.m_HRGN, RGN_XOR);
  return *this;
}

/**
 * Creates a region from a rectangle.
 */
Region Region::fromRect(const Rect &_rc)
{
  RECT rc = _rc;
  return Region(CreateRectRgnIndirect(&rc), SelfDestruction(true));
}

/**
 * Creates a region from an ellipse.
 */
Region Region::fromEllipse(const Rect &_rc)
{
  RECT rc = _rc;
  return Region(CreateEllipticRgnIndirect(&rc), SelfDestruction(true));
}

/**
 * Creates a region from a rounded rectangle.
 *
 * It uses the CreateRoundRectRgn function of Win32.
 */
Region Region::fromRoundRect(const Rect& _rc, const Size& ellipseSize)
{
  RECT rc = _rc;
  return Region(CreateRoundRectRgn(rc.left, rc.top,
				   rc.right, rc.bottom,
				   ellipseSize.w, ellipseSize.h),
		SelfDestruction(true));
}

/**
 * Returns the Win32 region handler.
 */
HRGN Region::getHRGN()
{
  return m_HRGN;
}

/**
 * Destroys the region, it's called by the destructor.
 *
 * In Win32 it uses the DeleteObject function.
 */
void Region::destroy()
{
  if (m_HRGN != NULL && m_selfDestruction) {
    DeleteObject(m_HRGN);

    m_HRGN = NULL;
  }
}

