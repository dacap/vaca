// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_REFERENCEABLE_H
#define VACA_REFERENCEABLE_H

#include "vaca/base.h"
#include "vaca/NonCopyable.h"

namespace vaca {

/**
   Class that counts references and can be wrapped by a SharedPtr.
*/
class VACA_DLL Referenceable : private NonCopyable
{
  template<class> friend class SharedPtr;
  unsigned m_refCount;

public:

  Referenceable();
  virtual ~Referenceable();

  void ref();
  unsigned unref();

  unsigned getRefCount();

#ifndef NDEBUG
  static void showLeaks();
#endif

private:
  void destroy();
};

} // namespace vaca

#endif // VACA_REFERENCEABLE_H
