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

#include <Vaca/Vaca.h>

using namespace Vaca;

enum {
  CMD_FILENEW = 1000,
  CMD_FILEOPEN,
  CMD_FILESAVE,
  CMD_PRINT,
  CMD_PRINTPRE,
  CMD_CUT,
  CMD_COPY,
  CMD_PASTE,
  CMD_DELETE,
  CMD_UNDO,
  CMD_REDOW,
  CMD_PROPERTIES,
  CMD_FIND,
  CMD_REPLACE,
  CMD_HELP,
};

//////////////////////////////////////////////////////////////////////

class Console : public MultilineEdit
{
  Font m_font;
  
public:

  Console(Widget *parent)
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
    m_toolBar.getSet().loadStandardImageList();

    m_toolBar.getSet().addButton(STD_FILENEW, CMD_FILENEW, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_FILEOPEN, CMD_FILEOPEN, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_FILESAVE, CMD_FILESAVE, TBSTATE_ENABLED);
    m_toolBar.getSet().addSeparator();
    m_toolBar.getSet().addButton(STD_PRINT, CMD_PRINT, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_PRINTPRE, CMD_PRINTPRE, TBSTATE_ENABLED);
    m_toolBar.getSet().addSeparator();
    m_toolBar.getSet().addButton(STD_CUT, CMD_CUT, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_COPY, CMD_COPY, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_PASTE, CMD_PASTE, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_DELETE, CMD_DELETE, TBSTATE_ENABLED);
    m_toolBar.getSet().addSeparator();
    m_toolBar.getSet().addButton(STD_UNDO, CMD_UNDO, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_REDOW, CMD_REDOW, TBSTATE_ENABLED);
    m_toolBar.getSet().addSeparator();
    m_toolBar.getSet().addButton(STD_PROPERTIES, CMD_PROPERTIES, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_FIND, CMD_FIND, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_REPLACE, CMD_REPLACE, TBSTATE_ENABLED);
    m_toolBar.getSet().addButton(STD_HELP, CMD_HELP, TBSTATE_ENABLED);

    // setup the defaults dock areas
    defaultDockAreas();

    // put the tool bar in the top dock area
    m_toolBar.dockIn(getDockArea(Side::Top));
  }

protected:

  virtual bool onActionById(int actionId)
  {
    switch (actionId) {
      case CMD_FILENEW:
	m_console.println("CMD_FILENEW");
	msgBox("New file...", "New", MB_OK);
	break;
      case CMD_FILEOPEN:   m_console.println("CMD_FILEOPEN"); break;
      case CMD_FILESAVE:   m_console.println("CMD_FILESAVE"); break;
      case CMD_PRINT:      m_console.println("CMD_PRINT"); break;
      case CMD_PRINTPRE:   m_console.println("CMD_PRINTPRE"); break;
      case CMD_CUT:        m_console.println("CMD_CUT"); break;
      case CMD_COPY:       m_console.println("CMD_COPY"); break;
      case CMD_PASTE:      m_console.println("CMD_PASTE"); break;
      case CMD_DELETE:     m_console.println("CMD_DELETE"); break;
      case CMD_UNDO:       m_console.println("CMD_UNDO"); break;
      case CMD_REDOW:      m_console.println("CMD_REDOW"); break;
      case CMD_PROPERTIES: m_console.println("CMD_PROPERTIES"); break;
      case CMD_FIND:       m_console.println("CMD_FIND"); break;
      case CMD_REPLACE:    m_console.println("CMD_REPLACE"); break;
      case CMD_HELP:       m_console.println("CMD_HELP"); break;
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
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
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
