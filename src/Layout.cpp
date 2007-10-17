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
#include "Vaca/Layout.h"
#include "Vaca/Debug.h"

using namespace Vaca;

Layout::Layout()
{
  mHdwp = NULL;
}

Layout::~Layout()
{
  VACA_ASSERT(mHdwp == NULL);
}

void Layout::beginMovement(const Widget::Container &widgets)
{
  mHdwp = BeginDeferWindowPos(widgets.size());

  mRelayoutWidgets.clear();
}

void Layout::moveWidget(Widget *widget, const Rect &rc)
{
  if (mHdwp != NULL) {
    mHdwp = DeferWindowPos(mHdwp, widget->getHwnd(), NULL,
			   rc.x, rc.y, rc.w, rc.h, 
			   SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
  }

//   if (widget->getBounds() == rc)
  mRelayoutWidgets.push_back(widget);
}

void Layout::endMovement()
{
  if (mHdwp != NULL) {
    EndDeferWindowPos(mHdwp);
    mHdwp = NULL;
  }

  for (Widget::Container::iterator it=mRelayoutWidgets.begin();
       it!=mRelayoutWidgets.end(); ++it) {
    (*it)->layout();
  }

  mRelayoutWidgets.clear();
}

// Size Layout::minimumSize(Widget *parent, Widget::Container &widgets)
// {
//   return Size(0, 0);
// }

Size Layout::preferredSize(Widget *parent, Widget::Container &widgets, const Size &fitIn)
{
  return Size(0, 0);
}

// Size Layout::maximumSize(Widget *parent, Widget::Container &widgets)
// {
//   // TODO use std::numeric_limits<int>::max()
//   return Size(INT_MAX, INT_MAX);
// }
