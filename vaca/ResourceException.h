// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_RESOURCEEXCEPTION_H
#define VACA_RESOURCEEXCEPTION_H

#include "vaca/Exception.h"

namespace vaca {

/**
   A resource (from exe or from an external file) can't be loaded.
*/
class ResourceException : public Exception
{
public:

  ResourceException() : Exception() { }
  ResourceException(const String& message) : Exception(message) { }
  virtual ~ResourceException() throw() { }

};

} // namespace vaca

#endif // VACA_RESOURCEEXCEPTION_H
