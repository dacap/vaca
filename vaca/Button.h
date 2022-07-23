// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_BUTTON_H
#define VACA_BUTTON_H

#include "vaca/ButtonBase.h"

namespace vaca {

/**
   Handles a pusheable button. These are the more common buttons, like
   "OK" or "Cancel".

   @image html Button.png
*/
class VACA_DLL Button : public ButtonBase
{
public:

  struct VACA_DLL Styles {

    /**
       Default style for Button widget.
    */
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Focusable |
      Style(BS_PUSHBUTTON, 0);
  };

  Button(const String& text, Widget* parent, Style style = Styles::Default);
  Button(const String& text, CommandId id, Widget* parent, Style style = Styles::Default);
  explicit Button(HWND handle);
  virtual ~Button();

  bool isDefault();
  void setDefault(bool state);

};

} // namespace vaca

#endif // VACA_BUTTON_H
