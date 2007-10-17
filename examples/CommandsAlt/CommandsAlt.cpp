// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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
// * Neither the name of the Vaca nor the names of its contributors
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

#include "Vaca/Vaca.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class Console : public MultilineEdit
{
  Font mFont;
  
public:

  Console(Widget *parent)
    : MultilineEdit("", parent, MultilineEditStyle + ScrollStyle)
    , mFont("Courier New", 10)
  {
    setFont(mFont);
    setBgColor(Color::Black);
    setFgColor(Color(0, 220, 0));
  }

  void println(const String &str)
  {
    setText(getText() + str + "\r\n");
    scrollLines(getLineCount());
  }
  
};

//////////////////////////////////////////////////////////////////////

class MyCommand : public Command // class used by method 2
{
  Console &mConsole;

public:

  MyCommand(int id, Console &console)
    : Command(id) 
    , mConsole(console)
  {
  }

  virtual void onAction()
  {
    mConsole.println("MyCommand::onAction("+
		     String::fromInt(getId())+")");
  }

};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  MenuBar mMenuBar;
  Console mConsole;

public:

  MainFrame()
    : Frame("Alternatives to hook a command")
    , mConsole(this)
  {
    setLayout(new ClientLayout);
    setMenuBar(&mMenuBar);

    // method 1: Hook the command in Command::Action signal
    Command *command;
    command = new Command(1000);
    command->Action.connect(Bind(&MainFrame::onCommandSlot, this, command));
    addCommand(command);

    // method 2: Hook the command in MyCommand::onAction event
    addCommand(new MyCommand(1001, mConsole));

    // method 3: Hook the command in MenuItem::Action signal
    MenuItem *menuItem;
    menuItem = new MenuItem("&Cmd1(Ctrl+1)",
			    Keys::Control | Keys::D1,
			    1000);

    menuItem->Action.connect(Bind(&MainFrame::onMenuItemSlot, this));
    mMenuBar.add(menuItem);

    menuItem = new MenuItem("&Cmd2(Ctrl+2)",
			    Keys::Control | Keys::D2,
			    1001);

    menuItem->Action.connect(Bind(&MainFrame::onMenuItemSlot, this));
    mMenuBar.add(menuItem);
  }
  

protected:

  // method 3: Hook the command in onIdAction
  virtual bool onIdAction(int id)
  {
    // print a message about the execution of this method
    mConsole.println("MainFrame::onIdAction("+String::fromInt(id)+")");

    // Frame::onIdAction() does the MenuItems and Commands stuff
    Frame::onIdAction(id);

    // print a separator
    mConsole.println("");
    return true;
  }

  // method used by method 3
  void onMenuItemSlot(MenuItemEvent &ev)
  {
    mConsole.println("MainFrame::onMenuItemSlot("+
		     String::fromInt(ev.getMenuItem()->getId())+")");
  }

  // method used by method 1
  void onCommandSlot(Command *cmd)
  {
    mConsole.println("MainFrame::onCommandSlot("+
		     String::fromInt(cmd->getId())+")");
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
