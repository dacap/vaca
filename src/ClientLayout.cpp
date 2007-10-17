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
#include "Vaca/ClientLayout.h"

using namespace Vaca;

// Size ClientLayout::minimumSize(Widget *parent, Widget::Container &widgets)
// {
//   Size sz(0, 0);

//   for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
//     Widget *widget = *it;
//     if (!widget->isLayoutFree()) {
//       Size pref = widget->minimumSize();
//       if (sz.w < pref.w) sz.w = pref.w;
//       if (sz.h < pref.h) sz.h = pref.h;
//     }
//   }

//   return sz;
// }

Size ClientLayout::preferredSize(Widget *parent, Widget::Container &widgets, const Size &fitIn)
{
  Size sz(0, 0);

  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget *widget = *it;
    if (!widget->isLayoutFree()) {
      Size pref = widget->preferredSize(fitIn);
      if (sz.w < pref.w) sz.w = pref.w;
      if (sz.h < pref.h) sz.h = pref.h;
    }
  }

  return sz;
}

// Size ClientLayout::maximumSize(Widget *parent, Widget::Container &widgets)
// {
//   Size sz(0, 0);

//   for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
//     Widget *widget = *it;
//     if (!widget->isLayoutFree()) {
//       Size pref = widget->maximumSize();
//       if (sz.w < pref.w) sz.w = pref.w;
//       if (sz.h < pref.h) sz.h = pref.h;
//     }
//   }

//   return sz;
// }

void ClientLayout::layout(Widget *parent, Widget::Container &widgets, const Rect &rc)
{
  beginMovement(widgets);

  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget *widget = *it;
    if (!widget->isLayoutFree())
      moveWidget(widget, rc);
  }

  endMovement();
}
