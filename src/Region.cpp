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

#include "stdvaca.h"
#include "Vaca/Region.h"
#include "Vaca/Rect.h"
#include "Vaca/Debug.h"
#include "Vaca/Point.h"

using namespace Vaca;

Region::Region()
{
  mHRGN = CreateRectRgn(0, 0, 0, 0);
  mAutoDelete = true;
  
  assert(mHRGN != NULL); // TODO exception
}

Region::Region(HRGN hrgn, bool autoDelete)
{
  assert(hrgn != NULL);
  
  mHRGN = hrgn;
  mAutoDelete = autoDelete;
}

Region::Region(const Rect &_rc)
{
  RECT rc = _rc;

  mHRGN = CreateRectRgnIndirect(&rc);
  mAutoDelete = true;

  assert(mHRGN != NULL); // TODO exception
}

Region::Region(const Region &rgn)
{
  mHRGN = NULL;
  assign(rgn);
}

Region::~Region()
{
  assert(mHRGN != NULL);
  
  if (mAutoDelete)
    DeleteObject(mHRGN);
}

Region &Region::operator=(const Region &rgn)
{
  assign(rgn);
  return *this;
}

void Region::assign(const Region &rgn)
{
  assert(rgn.mHRGN != NULL);

  // delete old region
  if (mHRGN != NULL && mAutoDelete)
    DeleteObject(mHRGN);

  // create a new empty region
  mHRGN = CreateRectRgn(0, 0, 0, 0);
  mAutoDelete = true;
  
  assert(mHRGN != NULL);	// TODO exception

  // copy the "rgn" to this region
  int res = CombineRgn(mHRGN,
		       rgn.mHRGN,
		       rgn.mHRGN, RGN_COPY);
  if (res == ERROR) {
    assert(false);	// TODO exception
  }
}

Rect Region::getBounds() const
{
  assert(mHRGN != NULL);

  RECT rc;
  int res = GetRgnBox(mHRGN, &rc);

  if (res == NULLREGION)
    return Rect();		// empty rectangle
  else
    return Rect(&rc);
}

bool Region::contains(const Point &pt) const
{
  assert(mHRGN != NULL);
	  
  return PtInRegion(const_cast<HRGN>(mHRGN), pt.x, pt.y) != FALSE;
}

bool Region::contains(const Rect &rc) const
{
  assert(mHRGN != NULL);

  RECT rc2 = rc;
  return RectInRegion(const_cast<HRGN>(mHRGN), &rc2) != FALSE;
}

bool Region::operator==(const Region &rgn) const
{
  BOOL res = EqualRgn(mHRGN, rgn.mHRGN) != FALSE;

  if (res == ERROR)
    return false;
  else
    return res != FALSE;
}

bool Region::operator!=(const Region &rgn) const
{
  return !operator==(rgn);
}

Region Region::operator|(const Region &rgn) const
{
  Region res;
  CombineRgn(res.mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_OR);
  return res;
}

Region Region::operator+(const Region &rgn) const
{
  return operator|(rgn);
}

Region Region::operator&(const Region &rgn) const
{
  Region res;
  CombineRgn(res.mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_AND);
  return res;
}

Region Region::operator-(const Region &rgn) const
{
  Region res;
  CombineRgn(res.mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_DIFF);
  return res;
}

Region Region::operator^(const Region &rgn) const
{
  Region res;
  CombineRgn(res.mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_XOR);
  return res;
}

Region &Region::operator|=(const Region &rgn)
{
  CombineRgn(this->mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_OR);
  return *this;
}

Region &Region::operator+=(const Region &rgn)
{
  return operator|=(rgn);
}

Region &Region::operator&=(const Region &rgn)
{
  CombineRgn(this->mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_AND);
  return *this;
}

Region &Region::operator-=(const Region &rgn)
{
  CombineRgn(this->mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_DIFF);
  return *this;
}

Region &Region::operator^=(const Region &rgn)
{
  CombineRgn(this->mHRGN,
	     this->mHRGN,
	     rgn.mHRGN, RGN_XOR);
  return *this;
}

Region Region::fromRect(const Rect &_rc)
{
  RECT rc = _rc;
  return Region(CreateRectRgnIndirect(&rc), true);
}

Region Region::fromEllipse(const Rect &_rc)
{
  RECT rc = _rc;
  return Region(CreateEllipticRgnIndirect(&rc), true);
}

Region Region::fromRoundRect(const Rect &_rc, int ellipseWidth, int ellipseHeight)
{
  RECT rc = _rc;
  return Region(CreateRoundRectRgn(rc.left, rc.top,
				   rc.right, rc.bottom,
				   ellipseWidth, ellipseHeight),
		true);
}

HRGN Region::getHRGN()
{
  return mHRGN;
}
