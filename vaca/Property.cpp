// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Property.h"
#include "vaca/Debug.h"

using namespace vaca;

/**
   Creates a new named property.
*/
Property::Property(const String& name)
  : m_name(name)
{
}

/**
   Destroys the property.
*/
Property::~Property()
{
}

String Property::getName() const
{
  return m_name;
}
