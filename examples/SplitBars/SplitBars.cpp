// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, Jie Zhang
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <Vaca/Vaca.h>
#include "resource.h"

using namespace Vaca;

class OrientationSwitchCommand : public Command
{
  SplitBar* sp;

public:
  OrientationSwitchCommand(CommandId id, SplitBar* sp)
    : Command(id)
    , sp(sp)
  {
  }

  virtual void execute()
  {
    sp->setOrientation(sp->getOrientation() == Orientation::Vertical
		       ? Orientation::Horizontal:
			 Orientation::Vertical);
  }

  virtual bool isChecked()
  {
    return sp->getOrientation() == Orientation::Vertical;
  }

};

class FullDragCommand : public Command
{
  SplitBar* sp1;
  SplitBar* sp2;

public:
  FullDragCommand(CommandId id, SplitBar* sp1, SplitBar* sp2)
    : Command(id)
    , sp1(sp1)
    , sp2(sp2)
  {
  }

  virtual void execute()
  {
    sp1->setFullDrag(!sp1->isFullDrag());
    sp2->setFullDrag(!sp2->isFullDrag());
  }

  virtual bool isChecked()
  {
    return sp1->isFullDrag();
  }

};

class MainFrame : public Frame,
		  public CommandsClient
{
public:

  MainFrame()
    : Frame(L"SplitBar demo")
  {
    setMenuBar(new MenuBar(CommandId(IDR_MAINMENU)));

    // The position of SplitBars are specified in percentage by
    // default, and the starting position is 50%
    SplitBar* sp1 = new SplitBar(Orientation::Vertical, this);
    SplitBar* sp2 = new SplitBar(Orientation::Horizontal, sp1,
				 SplitBar::Styles::Default |
				 SplitBar::Styles::ByPixels);
    TextEdit* firstPanel
      = new TextEdit(L"First Panel", sp1, TextEdit::Styles::TextArea +
					  Widget::Styles::Scroll);
    TextEdit* subFirstPanel =
      new TextEdit(L"Sub First Panel", sp2, TextEdit::Styles::TextArea +
					    Widget::Styles::Scroll);
    TextEdit* subSecondPanel =
      new TextEdit(L"Sub Second Panel", sp2, TextEdit::Styles::TextArea +
					     Widget::Styles::Scroll);

    sp1->setPane1(firstPanel);
    sp1->setPane2(sp2);

    sp2->setPane1(subFirstPanel);
    sp2->setPane2(subSecondPanel);
    sp2->setBarPosition(100);	// In pixels

    firstPanel->setBgColor(Color(200, 255, 200));
    subFirstPanel->setBgColor(Color(255, 200, 200));
    subSecondPanel->setBgColor(Color(200, 200, 255));

    setLayout(new ClientLayout);

    // add command listeners

    addCommand(new SignalCommand(CommandId(ID_FILE_EXIT), &MainFrame::onFileExit, this));

    addCommand(new OrientationSwitchCommand(CommandId(ID_VIEW_S_SP1), sp1));
    addCommand(new OrientationSwitchCommand(CommandId(ID_VIEW_S_SP2), sp2));
    addCommand(new FullDragCommand(CommandId(ID_FULLDRAG), sp1, sp2));

    addCommand(new SignalCommand(CommandId(ID_HELP_ABOUT), &MainFrame::onAppAbout, this));
  }

private:

  void onFileExit()
  {
    setVisible(false);
  }

  void onAppAbout()
  {
    MsgBox::show(this,
		 L"SplitBar demo",
		 L"SplitBar demo - written by nanothyll @2008",
		 MsgBox::Type::Ok,
		 MsgBox::Icon::Information);
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
