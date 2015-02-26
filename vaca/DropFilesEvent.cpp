// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/DropFilesEvent.h"
#include "vaca/Widget.h"

using namespace vaca;

/**
   Creates the event.

   @param widget Source of the event.
   @param files The list of dropped files.
*/
DropFilesEvent::DropFilesEvent(Widget* widget, std::vector<String> &files)
  : Event(widget)
  , m_files(files)
{
}

/**
   Destroys the event.
*/
DropFilesEvent::~DropFilesEvent()
{
}

/**
   @return The list of dropped files.
*/
std::vector<String> DropFilesEvent::getFiles()
{
  return m_files;
}
