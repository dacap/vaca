// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include <Vaca/Vaca.h>

using namespace Vaca;

enum {
  ID_FILENEW = 1000,
  ID_FILEOPEN,
  ID_FILESAVE,
  ID_PRINT,
  ID_PRINTPRE,
  ID_CUT,
  ID_COPY,
  ID_PASTE,
  ID_DELETE,
  ID_UNDO,
  ID_REDOW,
  ID_PROPERTIES,
  ID_FIND,
  ID_REPLACE,
  ID_HELP,
};

//////////////////////////////////////////////////////////////////////

class Console : public MultilineEdit
{
  Font m_font;
  
public:

  Console(Widget* parent)
    : MultilineEdit("", parent, MultilineEditStyle + ScrollStyle)
    , m_font("Courier New", 10)
  {
    setFont(&m_font);
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

class MainFrame : public Frame
{
  Console m_console;
  ToolBar m_toolBar;

public:

  MainFrame()
    : Frame("StdCommands")
    , m_console(this)
    , m_toolBar("Standard", this, ToolSetStyle + FlatToolSetStyle)
  {
    setLayout(new ClientLayout);

    // load the standard buttons
    m_toolBar.loadStandardImageList();

    m_toolBar.addButton(new ToolButton(ID_FILENEW, STD_FILENEW));
    m_toolBar.addButton(new ToolButton(ID_FILEOPEN, STD_FILEOPEN));
    m_toolBar.addButton(new ToolButton(ID_FILESAVE, STD_FILESAVE));
    m_toolBar.addSeparator();
    m_toolBar.addButton(new ToolButton(ID_PRINT, STD_PRINT));
    m_toolBar.addButton(new ToolButton(ID_PRINTPRE, STD_PRINTPRE));
    m_toolBar.addSeparator();
    m_toolBar.addButton(new ToolButton(ID_CUT, STD_CUT));
    m_toolBar.addButton(new ToolButton(ID_COPY, STD_COPY));
    m_toolBar.addButton(new ToolButton(ID_PASTE, STD_PASTE));
    m_toolBar.addButton(new ToolButton(ID_DELETE, STD_DELETE));
    m_toolBar.addSeparator();
    m_toolBar.addButton(new ToolButton(ID_UNDO, STD_UNDO));
    m_toolBar.addButton(new ToolButton(ID_REDOW, STD_REDOW));
    m_toolBar.addSeparator();
    m_toolBar.addButton(new ToolButton(ID_PROPERTIES, STD_PROPERTIES));
    m_toolBar.addButton(new ToolButton(ID_FIND, STD_FIND));
    m_toolBar.addButton(new ToolButton(ID_REPLACE, STD_REPLACE));
    m_toolBar.addButton(new ToolButton(ID_HELP, STD_HELP));

    // setup the defaults dock areas
    defaultDockAreas();

    // put the tool bar in the top dock area
    m_toolBar.dockIn(getDockArea(Side::Top));
  }

protected:

  virtual bool onCommand(CommandId id)
  {
    switch (id) {
      case ID_FILENEW:
	m_console.println("ID_FILENEW");
	MsgBox::show(this, "New", "New file...");
	break;
      case ID_FILEOPEN:   m_console.println("ID_FILEOPEN"); break;
      case ID_FILESAVE:   m_console.println("ID_FILESAVE"); break;
      case ID_PRINT:      m_console.println("ID_PRINT"); break;
      case ID_PRINTPRE:   m_console.println("ID_PRINTPRE"); break;
      case ID_CUT:        m_console.println("ID_CUT"); break;
      case ID_COPY:       m_console.println("ID_COPY"); break;
      case ID_PASTE:      m_console.println("ID_PASTE"); break;
      case ID_DELETE:     m_console.println("ID_DELETE"); break;
      case ID_UNDO:       m_console.println("ID_UNDO"); break;
      case ID_REDOW:      m_console.println("ID_REDOW"); break;
      case ID_PROPERTIES: m_console.println("ID_PROPERTIES"); break;
      case ID_FIND:       m_console.println("ID_FIND"); break;
      case ID_REPLACE:    m_console.println("ID_REPLACE"); break;
      case ID_HELP:       m_console.println("ID_HELP"); break;
      default:
	return false;		// "id" not used
    }
    return true;		// "id" was used
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main() {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example app;
  app.run();
  return 0;
}
