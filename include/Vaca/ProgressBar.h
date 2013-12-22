// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_PROGRESSBAR_H
#define VACA_PROGRESSBAR_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

/**
   Shows progress for a task that is running in background.
*/
class VACA_DLL ProgressBar : public Widget
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style Smooth;
    static const Style Vertical; // TODO change with set/getOrientation
    static const Style Marquee;
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

} // namespace Vaca

#endif // VACA_PROGRESSBAR_H
