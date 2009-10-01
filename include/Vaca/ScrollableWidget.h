// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#ifndef VACA_SCROLLABLEWIDGET_H
#define VACA_SCROLLABLEWIDGET_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/Size.h"
#include "Vaca/Point.h"

namespace Vaca {

/**
   A wrapper ready-to-use to create scrollable widgets.
*/   
class VACA_DLL ScrollableWidget : public Widget
{
  /**
     The full size area that can be viewed (using scrolling).
  */
  Size m_fullSize;

  /**
     This point is used to save the old mouse's position (on
     successive onMouseMove events).
  */
  Point m_oldPoint;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };
  
  ScrollableWidget(Widget* parent, Style style = Styles::Default);

  Size getFullSize() const;
  void setFullSize(const Size& sz);

  // Events
protected:
  virtual void onResize(ResizeEvent& ev);
  virtual void onMouseEnter(MouseEvent& ev);
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onMouseWheel(MouseEvent& ev);
  virtual void onSetCursor(SetCursorEvent& ev);
  virtual void onScroll(ScrollEvent& ev);

};

} // namespace Vaca

#endif // VACA_SCROLLABLEWIDGET_H
