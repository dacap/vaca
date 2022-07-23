// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TOGGLEBUTTON_H
#define VACA_TOGGLEBUTTON_H

#include "vaca/ButtonBase.h"

namespace vaca {

/**
   Handles a pusheable button (with BS_AUTOCHECKBOX | BS_PUSHLIKE styles).
*/
class VACA_DLL ToggleButton : public ButtonBase
{
public:

  struct VACA_DLL Styles {
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Focusable |
      Style(BS_AUTOCHECKBOX |
            BS_PUSHLIKE, 0);
  };

  ToggleButton(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~ToggleButton();

};

} // namespace vaca

#endif // VACA_TOGGLEBUTTON_H
