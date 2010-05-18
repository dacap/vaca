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

#ifndef VACA_SPINBUTTON_H
#define VACA_SPINBUTTON_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/CancelableEvent.h"

namespace Vaca {

// TODO move this to "SpinButtonEvent.h"
class SpinButtonEvent : public CancelableEvent
{
  int m_value;
  int m_delta;
  Side m_side;

public:

  SpinButtonEvent(Widget* widget, int value, int delta, Side side)
    : CancelableEvent(widget)
    , m_value(value)
    , m_delta(delta)
    , m_side(side)
  {
  }

  int getValue() { return m_value; }
  int getDelta() { return m_delta; }
  Side getSide() { return m_side; }

};

/**
   A couple of buttons to up or down a value.

   @win32
     This is a wrapper for the @msdn{Up-Down} control.
   @endwin32
*/
class VACA_DLL SpinButton : public Widget
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style Horizontal; // TODO change with set/getOrientation
    static const Style HotTrack;
  };

  SpinButton(Widget* parent, Style style = Styles::Default);
  SpinButton(int minValue, int maxValue, int value, Widget* parent, Style style = Styles::Default);
  virtual ~SpinButton();

  bool isHorizontal();
  bool isVertical();

  int getMinimum();
  int getMaximum();
  void getRange(int& minValue, int& maxValue);
  void setRange(int minValue, int maxValue);

  int getValue();
  void setValue(int value);

  int getBase();
  void setBase(int base);

  Widget* getBuddy();
  void setBuddy(Widget* buddy);

  // Signals
  Signal1<void, SpinButtonEvent&> Change;
//   Signal1<void, SpinButtonEvent&> BeforeChange;
//   Signal1<void, SpinButtonEvent&> AfterChange;

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);

  // New events
  virtual void onChange(SpinButtonEvent& ev);
//   virtual void onBeforeChange(SpinButtonEvent& ev);
//   virtual void onAfterChange(SpinButtonEvent& ev);
  // Events
//   virtual void onScroll(ScrollEvent& ev);
  // Reflected notifications
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

};

} // namespace Vaca

#endif // VACA_SPINBUTTON_H
