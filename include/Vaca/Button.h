// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_BUTTON_H
#define VACA_BUTTON_H

#include "Vaca/ButtonBase.h"

namespace Vaca {

/**
   Handles a pusheable button. These are the more common buttons, like
   "OK" or "Cancel".

   @image html Button.png
*/
class VACA_DLL Button : public ButtonBase
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  Button(const String& text, Widget* parent, Style style = Styles::Default);
  Button(const String& text, CommandId id, Widget* parent, Style style = Styles::Default);
  explicit Button(HWND handle);
  virtual ~Button();

  bool isDefault();
  void setDefault(bool state);

};

} // namespace Vaca

#endif // VACA_BUTTON_H
