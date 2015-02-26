// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_DROPFILESEVENT_H
#define VACA_DROPFILESEVENT_H

#include "vaca/base.h"
#include "vaca/Event.h"

#include <vector>

namespace vaca {

class VACA_DLL DropFilesEvent : public Event
{
  std::vector<String>& m_files;

public:

  DropFilesEvent(Widget* source, std::vector<String>& files);
  virtual ~DropFilesEvent();

  std::vector<String> getFiles();

};

} // namespace vaca

#endif // VACA_DROPFILESEVENT_H
