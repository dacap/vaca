// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/MenuItemEvent.h"
#include "Vaca/Menu.h"

using namespace Vaca;

MenuItemEvent::MenuItemEvent(MenuItem* source)
  : Event(source)
{
}

MenuItemEvent::~MenuItemEvent()
{
}

MenuItem* MenuItemEvent::getMenuItem()
{
  return dynamic_cast<MenuItem*>(getSource());
}
