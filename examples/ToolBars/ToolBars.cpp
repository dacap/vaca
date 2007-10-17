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
#include "resource.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class Console : public MultilineEdit
{
  Font mFont;
  
public:

  Console(Widget *parent)
    : MultilineEdit("", parent, MultilineEditStyle + ScrollStyle)
    , mFont("Verdana", 10)
  {
    setFont(mFont);
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
  ToolBar mToolBar1;
  ToolBar mToolBar2;
  ToolBar mToolBar3;
  DockBar mDockBar;
  Button mButton1;
  Button mButton2;
  Button mButton3;
  ImageList mImageList;
  Console mConsole;
  Panel mBottomPanel;
  Button mButton4;
  ToggleButton mButton5;
  ToggleButton mButton6;

public:

  MainFrame(Style toolSetStyle)
    : Frame("ToolBars")
    , mToolBar1("ToolBar 1", this, toolSetStyle)
    , mToolBar2("ToolBar 2", this, toolSetStyle)
    , mToolBar3("ToolBar 3", this, toolSetStyle)
    , mDockBar("mDockBar", this)
    , mButton1("Setup default dock areas (BandedDockArea)", &mDockBar)
    , mButton2("Setup basic dock areas (BasicDockArea)", &mDockBar)
    , mButton3("Clear all dock areas", &mDockBar)
    , mImageList(IDB_TOOLBAR, 16, Color(192, 192, 192))
    , mConsole(this)
    , mBottomPanel(this)
    , mButton4("Show all dock bars", &mBottomPanel)
    , mButton5("Full Drag mode", &mBottomPanel)
    , mButton6("Floating Gripper mode", &mBottomPanel)
  {
    setLayout(new BoxLayout(Vertical, false));
    mConsole.setConstraint(new BoxConstraint(true));
    mBottomPanel.setLayout(new BoxLayout(Horizontal, true, 0));

    mToolBar1.getSet().setImageList(mImageList);
    mToolBar1.getSet().addButton(0, 1000, TBSTATE_ENABLED);
    mToolBar1.getSet().addButton(1, 1001, TBSTATE_ENABLED);
    mToolBar1.getSet().addButton(2, 1002, TBSTATE_ENABLED);
    mToolBar1.getSet().addSeparator();
    mToolBar1.getSet().addButton(3, 1003, TBSTATE_ENABLED);
    mToolBar1.getSet().addButton(4, 1004, TBSTATE_ENABLED);
    mToolBar1.getSet().addButton(5, 1005, TBSTATE_ENABLED);

    mToolBar2.getSet().setImageList(mImageList);
    mToolBar2.getSet().addButton(2, 1002, TBSTATE_ENABLED);
    mToolBar2.getSet().addToggleButton(2, 1002, TBSTATE_ENABLED);
    mToolBar2.getSet().addToggleButton(2, 1002, 0);
    mToolBar2.getSet().addToggleButton(2, 1002, TBSTATE_CHECKED);
    mToolBar2.getSet().addToggleButton(2, 1002, TBSTATE_ENABLED | TBSTATE_CHECKED);
    mToolBar2.getSet().addToggleButton(2, 1002, TBSTATE_ENABLED | TBSTATE_INDETERMINATE);
    mToolBar2.getSet().addToggleButton(2, 1002, TBSTATE_ENABLED | TBSTATE_MARKED);

    mToolBar3.getSet().setImageList(mImageList);
    mToolBar3.getSet().addButton(2, 1002, TBSTATE_ENABLED);
    mToolBar3.getSet().addDropDownButton(2, 1002, TBSTATE_ENABLED);
    mToolBar3.getSet().addWholeDropDownButton(2, 1002, TBSTATE_ENABLED);
    mToolBar3.getSet().addWholeDropDownButton(2, 1002, TBSTATE_ENABLED);

    mDockBar.setLayout(new BoxLayout(Vertical, true));

    // make floating the three dock-bars
    mToolBar1.floatOut();
    mToolBar2.floatOut();
    mToolBar3.floatOut();
    mDockBar.floatOut();

    // put floating dock bars in a better place (the default place is
    // the origin of the client area of this MainFrame)
    Point origin = getAbsoluteClientBounds().getOrigin();
    Size size = getAbsoluteClientBounds().getSize();

    Size size1 = mToolBar1.getDockFrame()->preferredSize();
    mToolBar1.getDockFrame()->setBounds(Rect(origin.x+size.w-size1.w,
					     origin.y,
					     size1.w,
					     size1.h));

    Size size2 = mToolBar2.getDockFrame()->preferredSize();
    mToolBar2.getDockFrame()->setBounds(Rect(origin.x+size.w-size2.w,
					     origin.y+size1.h,
					     size2.w,
					     size2.h));

    Size size3 = mToolBar3.getDockFrame()->preferredSize();
    mToolBar3.getDockFrame()->setBounds(Rect(origin.x+size.w-size3.w,
					     origin.y+size1.h+size2.h,
					     size3.w,
					     size3.h));

    Size size4 = mDockBar.getDockFrame()->preferredSize();
    mDockBar.getDockFrame()->setBounds(Rect(origin.x+size.w-size4.w,
					    origin.y+size1.h+size2.h+size3.h,
					    size4.w,
					    size4.h));

    // actions
    mButton1.Action.connect(Bind(&MainFrame::onDefaultDockAreas, this));
    mButton2.Action.connect(Bind(&MainFrame::onBasicDockAreas, this));
    mButton3.Action.connect(Bind(&MainFrame::onClearDockAreas, this));
    mButton4.Action.connect(Bind(&MainFrame::onShowAll, this));
    mButton5.Action.connect(Bind(&MainFrame::onToggleFullDrag, this));
    mButton6.Action.connect(Bind(&MainFrame::onToggleFloatingGripper, this));

    // TODO when floating-gripper works, we can comment this line
    mButton6.setEnabled(false);

    mConsole.println("You must to setup dock areas to dock the tool bars");
  }

protected:

  virtual void onGotFocus(Event &ev)
  {
    Frame::onGotFocus(ev);
    mConsole.focus();
  }

private:

  void onDefaultDockAreas()
  {
    defaultDockAreas();
    layout();

    mConsole.println("Try to dock the tool bars in the new default (banded) dock areas");
  }

  void onBasicDockAreas()
  {
    clearDockAreas();
    addDockArea(new BasicDockArea(TopSide,    this));
    addDockArea(new BasicDockArea(BottomSide, this));
    addDockArea(new BasicDockArea(LeftSide,   this));
    addDockArea(new BasicDockArea(RightSide,  this));
    layout();

    mConsole.println("Try to dock the tool bars in the new basic dock areas");
  }

  void onClearDockAreas()
  {
    clearDockAreas();
    layout();

    mConsole.println("All dock bars were cleared, right now you can't dock the bars");
  }

  void onShowAll()
  {
    mToolBar1.setVisible(true);
    mToolBar2.setVisible(true);
    mToolBar3.setVisible(true);
    mDockBar.setVisible(true);

    mConsole.println("Show all dock bars");
  }

  void onToggleFullDrag()
  {
    bool state = mButton5.isSelected();
    
    mToolBar1.setFullDrag(state);
    mToolBar2.setFullDrag(state);
    mToolBar3.setFullDrag(state);
    mDockBar.setFullDrag(state);

    mConsole.println(String("Full Drag mode ") + (state ? "enabled": "disabled"));
  }

  void onToggleFloatingGripper()
  {
    bool state = mButton6.isSelected();

    mToolBar1.setFloatingGripper(state);
    mToolBar2.setFloatingGripper(state);
    mToolBar3.setFloatingGripper(state);
    mDockBar.setFloatingGripper(state);

    mConsole.println(String("Floating Gripper mode ") + (state ? "enabled": "disabled"));
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd1;
  MainFrame mMainWnd2;

public:

  Example()
    : mMainWnd1(ToolSetStyle)
    , mMainWnd2(ToolSetStyle + FlatToolSetStyle)
  {
  }
  
  virtual void main(std::vector<String> args) {
    mMainWnd1.setVisible(true);
    mMainWnd2.setVisible(true);
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
