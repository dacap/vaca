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

#ifndef VACA_SPINBUTTON_H
#define VACA_SPINBUTTON_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

#include "Vaca/CancellableEvent.h"

namespace Vaca {

#define SpinButtonStyle		(ChildStyle + Style(UDS_SETBUDDYINT |	\
						    UDS_ALIGNRIGHT |	\
						    UDS_AUTOBUDDY |	\
						    UDS_ARROWKEYS |	\
						    UDS_NOTHOUSANDS, 0))

#define HorizontalSpinButtonStyle	(Style(UDS_HORZ, 0))

#define HotTrackSpinButtonStyle		(Style(UDS_HOTTRACK, 0))

// TODO move this to "SpinButtonEvent.h"
class SpinButtonEvent : public CancellableEvent
{
  int mPosition;
  int mDelta;
  Side mSide;

public:

  SpinButtonEvent(Widget *widget, int position, int delta, Side side)
    : CancellableEvent(widget)
    , mPosition(position)
    , mDelta(delta)
    , mSide(side)
  {
  }

  int getPosition()
  {
    return mPosition;
  }

  int getDelta()
  {
    return mDelta;
  }

  Side getPressedSide()
  {
    return mSide;
  }

};

/**
 * SpinButton (Up-Down window control wrapper)
 */
class VACA_DLL SpinButton : public Widget
{
public:

  SpinButton(Widget *parent, Style style = SpinButtonStyle);

  virtual Size preferredSize();

  bool isHorizontal();
  bool isVertical();

  int getMinimum();
  int getMaximum();
  void getRange(int &minValue, int &maxValue);
  void setRange(int minValue, int maxValue);

  int getPosition();
  void setPosition(int posValue);

  Widget *getBuddy();
  void setBuddy(Widget *buddy);

  // signals
  boost::signal<void (SpinButtonEvent &)> Change;
//   boost::signal<void (SpinButtonEvent &)> BeforeChange;
//   boost::signal<void (SpinButtonEvent &)> AfterChange;
  
protected:
  // new events
  virtual void onChange(SpinButtonEvent &ev);
//   virtual void onBeforeChange(SpinButtonEvent &ev);
//   virtual void onAfterChange(SpinButtonEvent &ev);
  // events
//   virtual void onVScroll(int code, int pos/*, ScrollBar *scrollbar*/);
//   virtual void onHScroll(int code, int pos/*, ScrollBar *scrollbar*/);
  // reflection
  virtual bool onNotify(LPNMHDR lpnmhdr, LRESULT &lResult);

};

} // namespace Vaca

#endif
