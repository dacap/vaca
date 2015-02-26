// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Keys.h"

using namespace vaca;

Keys::Type Keys::fromMessageParams(WPARAM wParam, LPARAM lParam)
{
  return static_cast<Keys::Type>(wParam)
    | (GetKeyState(VK_SHIFT  ) & 0x8000 ? Keys::Shift  : 0)
    | (GetKeyState(VK_CONTROL) & 0x8000 ? Keys::Control: 0)
    | (GetKeyState(VK_MENU   ) & 0x8000 ? Keys::Alt    : 0);
}
