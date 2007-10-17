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

#ifndef VACA_BOXLAYOUT_H
#define VACA_BOXLAYOUT_H

#include "Vaca/base.h"
#include "Vaca/Layout.h"

namespace Vaca {

//////////////////////////////////////////////////////////////////////
// BoxLayout

class VACA_DLL BoxLayout : public Layout
{
  Orientation m_orientation;
  bool m_homogeneous;
  int m_border;
  int m_childSpacing;

public:

  BoxLayout(Orientation orientation,
	    bool homogeneous,
	    int borderSize = 4,
	    int childSpacing = 4);

  bool isHorizontal();
  bool isVertical();

  bool isHomogeneous();

  int getBorder();
  void setBorder(int border);

  int getChildSpacing();
  void setChildSpacing(int childSpacing);

  virtual Size getPreferredSize(Widget *parent, Widget::Container &widgets, const Size &fitIn);

protected:

  virtual void layout(Widget *parent, Widget::Container &widgets, const Rect &rc);

};

} // namespace Vaca

#endif

