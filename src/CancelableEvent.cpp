// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/CancelableEvent.h"

using namespace Vaca;

CancelableEvent::CancelableEvent(Component* source)
  : Event(source)
  , m_canceled(false)
{
}

CancelableEvent::~CancelableEvent()
{
}

/**
   Cancels the event.
*/
void CancelableEvent::cancel()
{
  m_canceled = true;
}

/**
   The event was canceled.
*/
bool CancelableEvent::isCanceled() const
{
  return m_canceled;
}
