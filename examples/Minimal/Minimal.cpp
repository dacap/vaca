// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

class MainFrame : public Frame
{
  Label label;
public:
  MainFrame() : Frame(L"Minimal")
              , label(L"This is the minimal program using Vaca\r\n"
		      L"so you can copy && paste this code.", this)
  {
    setLayout(new ClientLayout);
    setSize(getPreferredSize());
  }
};

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
