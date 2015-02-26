// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Button.h"
#include "vaca/Debug.h"

using namespace vaca;

Button::Button(const String& text, Widget* parent, Style style)
  : ButtonBase(parent, style)
{
  setText(text);
}

Button::Button(const String& text, CommandId id, Widget* parent, Style style)
  : ButtonBase(parent, style)
{
  setText(text);
  setId(id);
}

Button::Button(HWND handle)
  : ButtonBase(handle)
{
}

Button::~Button()
{
}

bool Button::isDefault()
{
  return (getStyle().regular & BS_DEFPUSHBUTTON) != 0;
}

void Button::setDefault(bool state)
{
  if (state) {
    addStyle(Style(BS_DEFPUSHBUTTON, 0));
    // setId(IDOK);
  }
  else {
    removeStyle(Style(BS_DEFPUSHBUTTON, 0));
    // if (getId() == IDOK)
    //   setId(0);
  }
}
