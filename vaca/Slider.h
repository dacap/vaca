// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SLIDER_H
#define VACA_SLIDER_H

#include "vaca/base.h"
#include "vaca/Widget.h"

namespace vaca {

class VACA_DLL Slider : public Widget
{
public:

  static const int MinLimit;
  static const int MaxLimit;

  struct VACA_DLL Styles {
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Focusable |
      Style(TBS_HORZ | TBS_BOTH |
            TBS_NOTICKS | TBS_AUTOTICKS, 0);
  };

  Slider(Widget* parent, Style style = Styles::Default);
  Slider(int minValue, int maxValue, int value, Widget* parent, Style style = Styles::Default);
  virtual ~Slider();

  Orientation getOrientation();
  void setOrientation(Orientation orientation);

  Sides getSides();
  void setSides(Sides sides);

  void setTickVisible(bool state);
  void setTickFreq(int freq);

  int getMinValue();
  int getMaxValue();
  void getRange(int& minValue, int& maxValue);
  void setRange(int minValue, int maxValue);

  int getValue();
  void setValue(int posValue);

  int getLineSize();
  void setLineSize(int lineSize);

  int getPageSize();
  void setPageSize(int pageSize);

  // Signals
  Signal<void(Event&)> Change; ///< @see onChange

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onScroll(ScrollEvent& ev);

  // New events
  virtual void onChange(Event& ev);

};

} // namespace vaca

#endif // VACA_SLIDER_H
