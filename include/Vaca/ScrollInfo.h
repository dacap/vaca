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

#ifndef VACA_SCROLLINFO_H
#define VACA_SCROLLINFO_H

#include "Vaca/base.h"

namespace Vaca {
  
/**
 * Auxiliary structure to get and set scroll information.
 * 
 * Scroll information includes: the valid range where the scroll
 * position can be, and the page size.
 *
 * @see Widget#getScrollInfo, Widget#getScrollPos
 */
class VACA_DLL ScrollInfo
{
  int m_minPos;
  int m_maxPos;
  int m_pageSize;

public:
  ScrollInfo();
  ScrollInfo(int minPos, int maxPos, int pageSize);
  virtual ~ScrollInfo();

  int getMinPos() const;
  int getMaxPos() const;
  int getPageSize() const;

  void setMinPos(int minPos);
  void setMaxPos(int maxPos);
  void setPageSize(int pageSize);

};

} // namespace Vaca

#endif // VACA_SCROLLINFO_H

