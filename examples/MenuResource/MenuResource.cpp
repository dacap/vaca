// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the author nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include <Vaca/Vaca.h>
#include "resource.h"

using namespace Vaca;

#include "../Console.h"

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Console m_console;
  MenuItem* m_menuItem2;
  MenuItem* m_menuItem3;

public:

  MainFrame()
    : Frame(L"MenuResource")
    , m_console(this)
  {
    // load the MenuBar from the resource with id=IDM_MENUBAR
    setMenuBar(new MenuBar(IDM_MENUBAR));
    setLayout(new ClientLayout);

    // get Item2 and Item3
    m_menuItem2 = getMenuBar()->getMenuItemById(IDM_ITEM2);
    m_menuItem3 = getMenuBar()->getMenuItemById(IDM_ITEM3);

    // disable Item4
    getMenuBar()->getMenuItemById(IDM_ITEM4)->setEnabled(false);
  }

private:

  virtual void onCommand(CommandEvent& ev)
  {
    if (!ev.isConsumed()) {
      // This is the raw way to handle commands, with a switch-case
      // statement. You can see the "Commands" or "TextEditor" examples
      // for a more sophisticated way using the "CommandsClient" class.
      switch (ev.getCommandId()) {

	case IDM_ITEM1:
	  m_console.println(L"Item1 selected");
	  ev.consume();
	  break;

	case IDM_ITEM2:
	  m_console.println(L"Item2 selected");
	  ev.consume();
	  break;

	case IDM_ITEM3:
	  m_console.println(L"Item3 selected (change Item2 state)");

	  // here we modify the selected-state of the MenuItem directly
	  m_menuItem2->setEnabled(!m_menuItem2->isEnabled());

	  // change the checked-state of IDM_ITEM3
	  m_menuItem3->setChecked(m_menuItem2->isEnabled());
	  ev.consume();
	  break;

	default:
	  m_console.println(format_string(L"Unknown action (ID = %d)",
					  ev.getCommandId()));
	  break;
      }
    }
    Frame::onCommand(ev);
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
