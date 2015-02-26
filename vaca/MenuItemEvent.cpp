// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/MenuItemEvent.h"
#include "vaca/Menu.h"

using namespace vaca;

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
