// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_CONSTRAINT_H
#define VACA_CONSTRAINT_H

#include "vaca/base.h"
#include "vaca/Referenceable.h"

namespace vaca {

/**
   Abstract class to represent a constraint. Constraints are useful to
   specify conditions to different arrangements using the same Layout
   manager.

   Each widget can have a constraint. It's only useful when the parent
   has a layout manager that known about the constraint of the widget
   (<b>the child</b>).

   A constraint is an object to indicate to the layout manager (of the
   parent widget) special properties to be used when the layout manager
   must to arrange the widget.
*/
class VACA_DLL Constraint : public Referenceable
{
public:

  Constraint();
  virtual ~Constraint();

};

} // namespace vaca

#endif // VACA_CONSTRAINT_H
