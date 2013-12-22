// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

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
