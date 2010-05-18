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

#include "Vaca/Layout.h"
#include "Vaca/Debug.h"
#include "Vaca/Widget.h"

using namespace Vaca;

Layout::Layout()
{
  m_HDWP = NULL;
}

Layout::~Layout()
{
  assert(m_HDWP == NULL);
}

void Layout::beginMovement(const WidgetList& widgets)
{
  m_HDWP = BeginDeferWindowPos(widgets.size());

  m_relayoutWidgets.clear();
}

void Layout::moveWidget(Widget* widget, const Rect& rc)
{
  if (m_HDWP != NULL) {
    m_HDWP = DeferWindowPos(m_HDWP, widget->getHandle(), NULL,
			    rc.x, rc.y, rc.w, rc.h, 
			    SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
  }

//   if (widget->getBounds() == rc)
  m_relayoutWidgets.push_back(widget);
}

void Layout::endMovement()
{
  if (m_HDWP != NULL) {
    EndDeferWindowPos(m_HDWP);
    m_HDWP = NULL;
  }

  for (WidgetList::iterator it=m_relayoutWidgets.begin();
       it!=m_relayoutWidgets.end(); ++it) {
    (*it)->layout();
  }

  m_relayoutWidgets.clear();
}

Size Layout::getPreferredSize(Widget* parent, WidgetList& widgets, const Size& fitIn)
{
  return Size(0, 0);
}
