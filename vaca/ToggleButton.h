// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TOGGLEBUTTON_H
#define VACA_TOGGLEBUTTON_H

#include "Vaca/ButtonBase.h"

namespace Vaca {

/**
   Handles a pusheable button (with BS_AUTOCHECKBOX | BS_PUSHLIKE styles).
*/
class VACA_DLL ToggleButton : public ButtonBase
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  ToggleButton(const String& text, Widget* parent, Style style = Styles::Default);
  virtual ~ToggleButton();

};

} // namespace Vaca

#endif // VACA_TOGGLEBUTTON_H
