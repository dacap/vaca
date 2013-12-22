// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_COMMONDIALOG_H
#define VACA_COMMONDIALOG_H

#include "Vaca/base.h"
#include "Vaca/Component.h"

#include <commdlg.h>

namespace Vaca {

/**
   Base class for common dialogs.
*/
class VACA_DLL CommonDialog : public Component
{
  Widget* m_parent;

public:

  CommonDialog(Widget* parent);
  virtual ~CommonDialog();

  Widget* getParent();
  HWND getParentHandle();

  virtual bool doModal() = 0;

};

} // namespace Vaca

#endif // VACA_COMMONDIALOG_H
