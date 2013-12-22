// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SPINNER_H
#define VACA_SPINNER_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Register.h"
#include "Vaca/TextEdit.h"
#include "Vaca/SpinButton.h"

namespace Vaca {

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
    static const Style Default;
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

} // namespace Vaca

#endif // VACA_SPINNER_H
