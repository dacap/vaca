// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/CommandEvent.h"

using namespace Vaca;

CommandEvent::CommandEvent(Component* source, CommandId commandId)
  : ConsumableEvent(source)
  , m_commandId(commandId)
{
}

CommandEvent::~CommandEvent()
{
}

CommandId CommandEvent::getCommandId() const
{
  return m_commandId;
}
