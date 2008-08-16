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
#include "resource.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class Console : public MultilineEdit
{
  Font m_font;
  
public:

  Console(Widget* parent)
    : MultilineEdit("", parent, MultilineEditStyle + ScrollStyle)
    , m_font("Verdana", 10)
  {
    setFont(&m_font);
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
  ToolBar m_toolBar1;
  ToolBar m_toolBar2;
  ToolBar m_toolBar3;
  DockBar m_dockBar;
  Button m_button1;
  Button m_button2;
  Button m_button3;
  ImageList m_imageList;
  Console m_console;
  Panel m_bottomPanel;
  Button m_button4;
  ToggleButton m_button5;
  ToggleButton m_button6;

public:

  MainFrame(Style toolSetStyle)
    : Frame("ToolBars")
    , m_toolBar1("ToolBar 1", this, toolSetStyle)
    , m_toolBar2("ToolBar 2", this, toolSetStyle)
    , m_toolBar3("ToolBar 3", this, toolSetStyle)
    , m_dockBar("DockBar", this)
    , m_button1("Setup BandedDockArea", &m_dockBar)
    , m_button2("Setup BasicDockAreas)", &m_dockBar)
    , m_button3("Clear DockAreas", &m_dockBar)
    , m_imageList(ResourceId(IDB_TOOLBAR), 16, Color(192, 192, 192))
    , m_console(this)
    , m_bottomPanel(this)
    , m_button4("Show all DockBars", &m_bottomPanel)
    , m_button5("Full Drag mode", &m_bottomPanel)
    , m_button6("Floating Gripper mode", &m_bottomPanel)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_console.setConstraint(new BoxConstraint(true));
    m_bottomPanel.setLayout(new BoxLayout(Orientation::Horizontal, false, 0));

    m_toolBar1.setImageList(m_imageList);
    m_toolBar2.setImageList(m_imageList);
    m_toolBar3.setImageList(m_imageList);

    m_toolBar1.addButton(new ToolButton(1000, 0));
    m_toolBar1.addButton(new ToolButton(1001, 1));
    m_toolBar1.addButton(new ToolButton(1002, 2));
    m_toolBar1.addSeparator();
    m_toolBar1.addButton(new ToolButton(1003, 3));
    m_toolBar1.addButton(new ToolButton(1004, 4));
    m_toolBar1.addButton(new ToolButton(1005, 5));

    for (int c=0; c<8; ++c) {
      m_toolBar2.addButton(new ToolButton(1001, 1));
      m_toolBar3.addButton(new ToolButton(1002, 2));
    }

    m_dockBar.setLayout(new BoxLayout(Orientation::Vertical, true));

    // make floating the three dock-bars
    m_toolBar1.floatOut();
    m_toolBar2.floatOut();
    m_toolBar3.floatOut();
    m_dockBar.floatOut();

    // put floating dock bars in a better place (the default place is
    // the origin of the client area of this MainFrame)
    Point origin = getAbsoluteClientBounds().getOrigin();
    Size size = getAbsoluteClientBounds().getSize();

    Size size1 = m_toolBar1.getDockFrame()->getPreferredSize();
    m_toolBar1.getDockFrame()->setBounds(Rect(origin.x+size.w-size1.w,
					     origin.y,
					     size1.w,
					     size1.h));

    Size size2 = m_toolBar2.getDockFrame()->getPreferredSize();
    m_toolBar2.getDockFrame()->setBounds(Rect(origin.x+size.w-size2.w,
					     origin.y+size1.h,
					     size2.w,
					     size2.h));

    Size size3 = m_toolBar3.getDockFrame()->getPreferredSize();
    m_toolBar3.getDockFrame()->setBounds(Rect(origin.x+size.w-size3.w,
					     origin.y+size1.h+size2.h,
					     size3.w,
					     size3.h));

    Size size4 = m_dockBar.getDockFrame()->getPreferredSize();
    m_dockBar.getDockFrame()->setBounds(Rect(origin.x+size.w-size4.w,
					    origin.y+size1.h+size2.h+size3.h,
					    size4.w,
					    size4.h));

    // actions
    m_button1.Action.connect(Bind(&MainFrame::onDefaultDockAreas, this));
    m_button2.Action.connect(Bind(&MainFrame::onBasicDockAreas, this));
    m_button3.Action.connect(Bind(&MainFrame::onClearDockAreas, this));
    m_button4.Action.connect(Bind(&MainFrame::onShowAll, this));
    m_button5.Action.connect(Bind(&MainFrame::onToggleFullDrag, this));
    m_button6.Action.connect(Bind(&MainFrame::onToggleFloatingGripper, this));

    m_console.println("You must to setup DockAreas to dock the Tool/DockBars");
  }

protected:

  virtual void onGotFocus(Event &ev)
  {
    Frame::onGotFocus(ev);
    m_console.requestFocus();
  }

private:

  void onDefaultDockAreas()
  {
    if (safeDeleteDockAreas()) {
      defaultDockAreas();
      layout();

      m_console.println("Try to dock the Tool/DockBars in the BandedDockAreas that are around the Frame");
    }
  }

  void onBasicDockAreas()
  {
    if (safeDeleteDockAreas()) {
      addDockArea(new BasicDockArea(Side::Top,    this));
      addDockArea(new BasicDockArea(Side::Bottom, this));
      addDockArea(new BasicDockArea(Side::Left,   this));
      addDockArea(new BasicDockArea(Side::Right,  this));
      layout();

      m_console.println("Try to dock the Tool/DockBars in the new BasicDockAreas that are around the Frame");
    }
  }

  void onClearDockAreas()
  {
    if (safeDeleteDockAreas()) {
      layout();
      m_console.println("All DockAreas were cleared, right now you can't dock the Tool/DockBars");
    }
  }

  void onShowAll()
  {
    m_toolBar1.setVisible(true);
    m_toolBar2.setVisible(true);
    m_toolBar3.setVisible(true);
    m_dockBar.setVisible(true);

    m_console.println("All DockBars are visible now");
  }

  void onToggleFullDrag()
  {
    bool state = m_button5.isSelected();
    
    m_toolBar1.setFullDrag(state);
    m_toolBar2.setFullDrag(state);
    m_toolBar3.setFullDrag(state);
    m_dockBar.setFullDrag(state);

    m_console.println(String("Full Drag mode ") + (state ? "enabled": "disabled"));
  }

  void onToggleFloatingGripper()
  {
    bool state = m_button6.isSelected();

    m_toolBar1.setFloatingGripper(state);
    m_toolBar2.setFloatingGripper(state);
    m_toolBar3.setFloatingGripper(state);
    m_dockBar.setFloatingGripper(state);

    m_console.println(String("Floating Gripper mode ") + (state ? "enabled": "disabled"));
  }

private:

  bool safeDeleteDockAreas()
  {
    if (m_toolBar1.isDocked() || m_toolBar2.isDocked() ||
	m_toolBar3.isDocked() || m_dockBar.isDocked()) {
      MsgBox::show(this, "Error",
		   "You can't change DockAreas if you have some Tool/DockBars docked.",
		   MsgBox::Type::Ok,
		   MsgBox::Icon::Error);
      return false;
    }
    else {
      deleteDockAreas();
      return true;
    }
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame1;
  MainFrame m_mainFrame2;

public:
  Example()
    : m_mainFrame1(ToolSetStyle)
    , m_mainFrame2(ToolSetStyle + FlatToolSetStyle)
  {
  }
  
  virtual void main() {
    m_mainFrame1.setVisible(true);
    m_mainFrame2.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  std::auto_ptr<Example> app(new Example);
  app->run();
  return 0;
}
