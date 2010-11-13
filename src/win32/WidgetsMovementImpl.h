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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Vaca::WidgetsMovement::WidgetsMovementImpl
{
  HDWP m_hdwp;
  WidgetList m_relayoutWidgets;

public:

  WidgetsMovementImpl(const WidgetList& widgets)
  {
    m_hdwp = BeginDeferWindowPos(widgets.size());
  }

  ~WidgetsMovementImpl()
  {
    EndDeferWindowPos(m_hdwp);
    m_hdwp = NULL;

    for (WidgetList::iterator it=m_relayoutWidgets.begin();
	 it!=m_relayoutWidgets.end(); ++it) {
      (*it)->layout();
    }
  }

  void moveWidget(Widget* widget, const Rect& rc)
  {
    m_hdwp = DeferWindowPos(m_hdwp, widget->getHandle(), NULL,
			    rc.x, rc.y, rc.w, rc.h, 
			    SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

    m_relayoutWidgets.push_back(widget);
  }

};
