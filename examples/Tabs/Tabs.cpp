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

class MainFrame : public Frame
{
  MenuBar mMenuBar;
  Menu mAppMenu;
  Menu mOptionsMenu;
  Menu mSideMenu;
  TabBase mTab;
  Label mLabelHere;
  Label mLabelLast;
  LinkLabel mLabel0;
  LinkLabel mLabel1;
  LinkLabel mLabel2;
  LinkLabel mLabel3;
  Font mTabFont;

public:

  MainFrame()
    : Frame("Tabs")
    , mAppMenu("&Tabs")
    , mOptionsMenu("&Options")
    , mSideMenu("&Side")
    , mTab(this)
    , mLabelHere("", &mTab)
    , mLabelLast("Start", &mTab)
    , mLabel0("Go to page0", &mTab)
    , mLabel1("Go to page1", &mTab)
    , mLabel2("Go to page2", &mTab)
    , mLabel3("Go to page3", &mTab)
    , mTabFont(Font::getDefault())
  {
    initializeMenuBar();
    setMenuBar(&mMenuBar);
    setLayout(new ClientLayout);

    // mTab
    mTab.setLayout(new BoxLayout(Vertical, false));

    mTab.addPage("Page0");
    mTab.addPage("Page1");
    mTab.addPage("Page2");
    mTab.addPage("Page3");

    mTab.PageChange.connect(Bind(&MainFrame::onPageChange, this));

    // labels
    mLabel0.Action.connect(Bind(&MainFrame::onPageLink, this, 0));
    mLabel1.Action.connect(Bind(&MainFrame::onPageLink, this, 1));
    mLabel2.Action.connect(Bind(&MainFrame::onPageLink, this, 2));
    mLabel3.Action.connect(Bind(&MainFrame::onPageLink, this, 3));

    updatePage();

    setSize(256, preferredSize().h);
  }

private:

  void initializeMenuBar()
  {
    MenuItem *menuItem;

    // Tabs/Exit
    menuItem = mAppMenu.add("E&xit");
    menuItem->Action.connect(Bind(&MainFrame::dispose, this));

    // Options/Change Font
    menuItem = mOptionsMenu.add("Change &Font");
    menuItem->Action.connect(Bind(&MainFrame::onChangeFont, this));

    // Options/Multiline
    menuItem = mOptionsMenu.add("&Multiline");
    menuItem->Action.connect(Bind(&MainFrame::onMultiline, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdateMultiline, this));

    // Options/Side
    mOptionsMenu.add(&mSideMenu);

    // Options/Side/Top
    menuItem = mSideMenu.add("&Top");
    menuItem->Action.connect(Bind(&Tab::setSide, &mTab, TopSide));
    menuItem->Update.connect(Bind(&MainFrame::onUpdateSide, this, TopSide, boost::arg<1>()));

    // Options/Side/Left
    menuItem = mSideMenu.add("&Left");
    menuItem->Action.connect(Bind(&Tab::setSide, &mTab, LeftSide));
    menuItem->Update.connect(Bind(&MainFrame::onUpdateSide, this, LeftSide, boost::arg<1>()));

    // Options/Side/Bottom
    menuItem = mSideMenu.add("&Bottom");
    menuItem->Action.connect(Bind(&Tab::setSide, &mTab, BottomSide));
    menuItem->Update.connect(Bind(&MainFrame::onUpdateSide, this, BottomSide, boost::arg<1>()));

    // Options/Side/Right
    menuItem = mSideMenu.add("&Right");
    menuItem->Action.connect(Bind(&Tab::setSide, &mTab, RightSide));
    menuItem->Update.connect(Bind(&MainFrame::onUpdateSide, this, RightSide, boost::arg<1>()));

    // Menu bar
    mMenuBar.add(&mAppMenu);
    mMenuBar.add(&mOptionsMenu);
    mMenuBar.add("&Read me")->
      Action.connect(Bind(&MainFrame::msgBox, this,
			  "A Tab widget with pages in Left or Right sides\n"
			  "must be Multiline. Also, when you use themes on\n"
			  "WinXP, you can't use Left or Right sides at all.\n",
			  "Win32 Limitation", MB_OK));
  }

  void onChangeFont()
  {
    FontDialog dialog(mTabFont, this);
    if (dialog.doModal()) {
      mTab.setFont(mTabFont);
      mTab.invalidate(true);
    }
  }

  void onMultiline()
  {
    mTab.setMultiline(!mTab.isMultiline());
  }

  void onUpdateMultiline(MenuItemEvent &ev)
  {
    ev.getMenuItem()->setChecked(mTab.isMultiline());
    ev.getMenuItem()->setEnabled(mTab.getSide() != LeftSide &&
				 mTab.getSide() != RightSide);
  }

  void onUpdateSide(Side side, MenuItemEvent &ev)
  {
    ev.getMenuItem()->setRadio(mTab.getSide() == side);
  }

  void onPageLink(int page)
  {
    mTab.setActivePage(page);
    updatePage();
  }

  void onPageChange()
  {
    updatePage();
    mLabelLast.setText("Last selected " + mTab.getPageText(mTab.getActivePage()));
  }

private:

  void updatePage()
  {
    mLabelHere.setText("We are in " + mTab.getPageText(mTab.getActivePage()));
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
