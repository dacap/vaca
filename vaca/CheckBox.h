// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CHECKBOX_H
#define VACA_CHECKBOX_H

#include "vaca/ButtonBase.h"

namespace vaca {

/**
   Handles a check box button.

   @image html CheckBox.png
*/
class VACA_DLL CheckBox : public ButtonBase
{
public:

  struct Styles {
    /**
       Default style for CheckBox widget.
    */
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Focusable |
      Style(BS_AUTOCHECKBOX, 0);
  };

  CheckBox(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~CheckBox();

};

} // namespace vaca

#endif // VACA_CHECKBOX_H
