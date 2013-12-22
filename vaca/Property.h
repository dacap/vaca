// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_PROPERTY_H
#define VACA_PROPERTY_H

#include "Vaca/base.h"
#include "Vaca/Referenceable.h"

namespace Vaca {

class VACA_DLL Property : public Referenceable
{
  String m_name;

public:
  Property(const String& name);
  virtual ~Property();

  String getName() const;
};

} // namespace Vaca

#endif // VACA_PROPERTY_H
