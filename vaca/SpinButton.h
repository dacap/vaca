// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SPINBUTTON_H
#define VACA_SPINBUTTON_H

#include "vaca/base.h"
#include "vaca/Widget.h"
#include "vaca/CancelableEvent.h"

namespace vaca {

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

  struct Styles {
    static constexpr Style Default =
      Widget::Styles::Visible |
      Style(UDS_SETBUDDYINT |
            UDS_ALIGNRIGHT |
            UDS_AUTOBUDDY |
            UDS_ARROWKEYS |
            UDS_NOTHOUSANDS, 0);

    // TODO change with set/getOrientation
    static constexpr Style Horizontal = Style(UDS_HORZ, 0);
    static constexpr Style HotTrack = Style(UDS_HOTTRACK, 0);
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
  Signal<void(SpinButtonEvent&)> Change;
//   Signal<void(SpinButtonEvent&)> BeforeChange;
//   Signal<void(SpinButtonEvent&)> AfterChange;

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

} // namespace vaca

#endif // VACA_SPINBUTTON_H
