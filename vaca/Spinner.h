// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SPINNER_H
#define VACA_SPINNER_H

#include "vaca/base.h"
#include "vaca/Widget.h"
#include "vaca/WidgetClass.h"
#include "vaca/Register.h"
#include "vaca/TextEdit.h"
#include "vaca/SpinButton.h"

namespace vaca {

/**
   Represents the Win32 class used by Spinner.
*/
class SpinnerClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.Spinner"); }
};

/**
   A Spinner is a couple of Widgets: an Edit and a SpinButton at the
   right side.

   The default range is from 0 to 100. The default position is 0.
*/
class VACA_DLL Spinner : public Register<SpinnerClass>, public Widget
{
  TextEdit m_edit;
  SpinButton m_spin;

public:

  struct VACA_DLL Styles {
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Container;
  };

  Spinner(Widget* parent,
	  Style spinStyle = SpinButton::Styles::Default,
	  Style style = Spinner::Styles::Default);
  Spinner(int minValue, int maxValue, int value,
	  Widget* parent,
	  Style spinStyle = SpinButton::Styles::Default,
	  Style style = Spinner::Styles::Default);
  virtual ~Spinner();

  TextEdit& getTextEdit();
  SpinButton& getSpinButton();

  int getMinimum();
  int getMaximum();
  void getRange(int& minValue, int& maxValue);
  void setRange(int minValue, int maxValue);

  int getValue();
  void setValue(int value);

  int getBase();
  void setBase(int base);

protected:

  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onLayout(LayoutEvent& ev);

};

} // namespace vaca

#endif // VACA_SPINNER_H
