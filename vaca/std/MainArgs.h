// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/String.h"

void vaca::details::MainArgs::setArgs(int argc, char* argv[])
{
  std::vector<String> args;
  args.reserve(argc);
  for (int i=0; i<argc; ++i)
    args.push_back(convert_to<String>(argv[i]));

  Application::setArgs(args);
}
