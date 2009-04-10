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

#error deprecated

#include <Vaca/Vaca.h>
#include "../resource.h"

using namespace Vaca;

#include "../Console.h"

//////////////////////////////////////////////////////////////////////

class MyCommand : public Command // class used by method 2
{
  Console& m_console;

public:

  MyCommand(int id, Console &console)
    : Command(id) 
    , m_console(console)
  {
  }

  virtual void execute()
  {
    m_console.println("MyCommand::execute("+
		      String::fromInt(getId())+")");
  }

};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Console m_console;

public:

  MainFrame()
    : Frame("Alternatives to hook a command")
    , m_console(this)
  {
    setLayout(new ClientLayout);
    setMenuBar(new MenuBar);

    // method 1: Hook the command in Command::Execute signal
    Command *command;
    command = new Command(1000);
    command->Click.connect(Bind(&MainFrame::onCommandSlot, this, command));
    addCommand(command);

    // method 2: Hook the command in MyCommand::onAction event
    addCommand(new MyCommand(1001, m_console));

    // method 3: Hook the command in MenuItem::Action signal
    MenuItem* menuItem;
    menuItem = new MenuItem("&Cmd1(Ctrl+1)", 1000,
			    Keys::Control | Keys::D1);

    menuItem->Action.connect(Bind(&MainFrame::onMenuItemSlot, this));
    getMenuBar()->add(menuItem);

    menuItem = new MenuItem("&Cmd2(Ctrl+2)", 1001,
			    Keys::Control | Keys::D2);

    menuItem->Action.connect(Bind(&MainFrame::onMenuItemSlot, this));
    getMenuBar()->add(menuItem);
  }
  

protected:

  // method 4: Hook the command in onIdAction
  virtual bool onIdAction(int id)
  {
    // print a message about the execution of this method
    m_console.println("MainFrame::onIdAction("+String::fromInt(id)+")");

    // Frame::onIdAction() does the MenuItems and Commands stuff
    Frame::onIdAction(id);

    // print a separator
    m_console.println("");
    return true;
  }

  // routine used by method 3
  void onMenuItemSlot(MenuItemEvent &ev)
  {
    m_console.println("MainFrame::onMenuItemSlot("+
		     String::fromInt(ev.getMenuItem()->getId())+")");
  }

  // routine used by method 1
  void onCommandSlot(Command* cmd)
  {
    m_console.println("MainFrame::onCommandSlot("+
		     String::fromInt(cmd->getId())+")");
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
