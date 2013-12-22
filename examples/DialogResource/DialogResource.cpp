// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <Vaca/Vaca.h>
#include "resource.h"

using namespace Vaca;

void show_about(Widget* parent)
{
  Dialog dlg(ResourceId(IDD_ABOUT), parent);

  dlg.center();
  dlg.doModal();
}

int VACA_MAIN()
{
  Application app;
  Dialog dlg(ResourceId(IDD_MAIN));
  // Button but(dlg.getItemHandle(ResourceId(IDC_MAIN_ABOUT_BUTTON)));
  Button button(::GetDlgItem(dlg.getHandle(), IDC_MAIN_ABOUT_BUTTON));
  button.Click.connect(Bind<void>(&show_about, &dlg));

  dlg.center();
  dlg.doModal();
  return 0;
}
