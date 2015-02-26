// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/MainArgs.h"
#include "vaca/Application.h"

using namespace vaca;

#if defined(VACA_WINDOWS)
  #include "win32/MainArgs.h"
#else
  #include "std/MainArgs.h"
#endif
