// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_FINDFILES_H
#define VACA_FINDFILES_H

#include "vaca/base.h"
#include "vaca/NonCopyable.h"

namespace vaca {

class VACA_DLL FindFiles : public NonCopyable
{
  String m_pattern;
  HANDLE m_handle;
  WIN32_FIND_DATA m_data;

public:

  FindFiles(const String& pattern);
  ~FindFiles();

  bool next();

  String getFileName() const;
  String getFullFileName() const;
  bool isFile() const;
  bool isDirectory() const;

};

} // namespace vaca

#endif // VACA_FINDFILES_H
