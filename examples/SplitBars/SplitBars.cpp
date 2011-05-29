// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, Jie Zhang
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

    sp1->setFirstWidget(firstPanel);
    sp1->setSecondWidget(sp2);

    sp2->setFirstWidget(subFirstPanel);
    sp2->setSecondWidget(subSecondPanel);
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

int vaca_main()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}

#include "Vaca/main.h"
