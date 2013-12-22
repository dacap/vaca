// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_COMPONENT_H
#define VACA_COMPONENT_H

#include "Vaca/base.h"
#include "Vaca/Referenceable.h"
#include <map>

namespace Vaca {

/**
   A component is a visual object, such as widgets or menus.

   Components are non-copyable but are referenceable (e.g. you can
   use them inside a SharedPtr).

   @see NonCopyable, Referenceable, SharedPtr
*/
class VACA_DLL Component : public Referenceable
{
public:
  typedef std::map<String, PropertyPtr> Properties;

  Component();
  virtual ~Component();

  PropertyPtr getProperty(const String& name);
  void setProperty(PropertyPtr property);

  bool hasProperty(const String& name);
  void removeProperty(const String& name);

  const Properties& getProperties() const;

private:
  Properties m_properties;
};

} // namespace Vaca

#endif // VACA_COMPONENT_H
