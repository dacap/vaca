// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_PROGRESSBAR_H
#define VACA_PROGRESSBAR_H

#include "vaca/base.h"
#include "vaca/Widget.h"

#ifndef PBS_MARQUEE
  #define PBS_MARQUEE 8
#endif

namespace vaca {

/**
   Shows progress for a task that is running in background.
*/
class VACA_DLL ProgressBar : public Widget
{
public:

  struct VACA_DLL Styles {
    static constexpr Style Default = Widget::Styles::Visible;
    static constexpr Style Smooth = Style(PBS_SMOOTH, 0);
     // TODO change with set/getOrientation
    static constexpr Style Vertical = Style(PBS_VERTICAL, 0);
    static constexpr Style Marquee = Style(PBS_MARQUEE, 0);
  };

  ProgressBar(Widget* parent, Style style = Styles::Default);
  ProgressBar(int minValue, int maxValue, Widget* parent, Style style = Styles::Default);
  virtual ~ProgressBar();

  virtual void setBgColor(const Color& color);

  int getMinimum();
  int getMaximum();
  void getRange(int& minValue, int& maxValue);
  void setRange(int minValue, int maxValue);

  int getValue();
  void setValue(int value);
  void addValue(int delta);

  void setMarquee(int msecsInterval);

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);

};

} // namespace vaca

#endif // VACA_PROGRESSBAR_H
