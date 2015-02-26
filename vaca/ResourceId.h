// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_RESOURCEID_H
#define VACA_RESOURCEID_H

#include "vaca/base.h"

namespace vaca {

/**
   Class to wrap an ID of an object in the resource file (.rc).

   @win32
     Look for @msdn{About Resource Files}.
   @endwin32
*/
class VACA_DLL ResourceId
{
  int m_id;

public:

  explicit ResourceId(int id);
  ResourceId(const ResourceId& rc);
  virtual ~ResourceId();

  int getId() const;

  String toString();
  LPTSTR toLPTSTR();

};

} // namespace vaca

#endif // VACA_RESOURCEID_H
