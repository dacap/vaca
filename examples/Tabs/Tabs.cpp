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

#include <Vaca/Vaca.hpp>

using namespace Vaca;

enum {
  ID_EXIT = 1000,
  ID_CHANGE_FONT,
  ID_MULTILINE,
  ID_READ_ME
};

class MainFrame : public Frame
		, public CommandsClient
{
  TabBase m_tab;
  Label m_labelHere;
  Label m_labelLast;
  LinkLabel m_label0;
  LinkLabel m_label1;
  LinkLabel m_label2;
  LinkLabel m_label3;
  Font m_tabFont;

  class SideMenuItem : public MenuItem
  {
    Side m_side;
    TabBase& m_tab;

  public:
    SideMenuItem(const String& text, Side side, TabBase& tab)
      : MenuItem(text)
      , m_side(side)
      , m_tab(tab) { }

    virtual void onAction(MenuItemEvent& ev) {
      MenuItem::onAction(ev);
      m_tab.setSide(m_side);
    }
    
    virtual void onUpdate(MenuItemEvent& ev) {
      MenuItem::onUpdate(ev);
      setRadio(m_tab.getSide() == m_side);
    }
    
  };

public:

  MainFrame()
    : Frame("Tabs")
    , m_tab(this)
    , m_labelHere("", &m_tab)
    , m_labelLast("Start", &m_tab)
    , m_label0("Go to page0", &m_tab)
    , m_label1("Go to page1", &m_tab)
    , m_label2("Go to page2", &m_tab)
    , m_label3("Go to page3", &m_tab)
    , m_tabFont(*Font::getDefault())
  {
    setMenuBar(createMenuBar());
    setLayout(new ClientLayout);

    // commands
    SignalCommand* cmd;

    addCommand(new SignalCommand(ID_EXIT, Bind(&MainFrame::setVisible, this, false)));
    addCommand(new SignalCommand(ID_CHANGE_FONT, &MainFrame::onChangeFont, this));
    addCommand(cmd = new SignalCommand(ID_MULTILINE));
    cmd->Execute.connect(&MainFrame::onMultiline, this);
    cmd->Enabled.connect(&MainFrame::onUpdateMultiline, this);
    addCommand(new SignalCommand(ID_READ_ME, &MainFrame::onReadMe, this));

    // m_tab
    m_tab.setLayout(new BoxLayout(Orientation::Vertical, false));

    m_tab.addPage("Page0");
    m_tab.addPage("Page1");
    m_tab.addPage("Page2");
    m_tab.addPage("Page3");

    m_tab.PageChange.connect(Bind(&MainFrame::onPageChange, this));

    // labels
    m_label0.Action.connect(Bind(&MainFrame::onPageLink, this, 0));
    m_label1.Action.connect(Bind(&MainFrame::onPageLink, this, 1));
    m_label2.Action.connect(Bind(&MainFrame::onPageLink, this, 2));
    m_label3.Action.connect(Bind(&MainFrame::onPageLink, this, 3));

    updatePage();

    setSize(256, getPreferredSize().h);
  }

private:

  MenuBar* createMenuBar()
  {
    MenuBar* menuBar  = new MenuBar();
    Menu* appMenu     = new Menu("&Tabs");
    Menu* optionsMenu = new Menu("&Options");
    Menu* sideMenu    = new Menu("&Side");

    appMenu->add("E&xit", ID_EXIT);

    optionsMenu->add("Change &Font", ID_CHANGE_FONT);
    optionsMenu->add("&Multiline", ID_MULTILINE);
    optionsMenu->add(sideMenu);

    sideMenu->add(new SideMenuItem("&Top", Side::Top, m_tab));
    sideMenu->add(new SideMenuItem("&Left", Side::Left, m_tab));
    sideMenu->add(new SideMenuItem("&Bottom", Side::Bottom, m_tab));
    sideMenu->add(new SideMenuItem("&Right", Side::Right, m_tab));

    menuBar->add(appMenu);
    menuBar->add(optionsMenu);
    menuBar->add("&Read me", ID_READ_ME);

    return menuBar;
  }

  void onChangeFont()
  {
    FontDialog dialog(&m_tabFont, this);
    if (dialog.doModal()) {
      m_tab.setFont(&m_tabFont);
      m_tab.invalidate(true);
    }
  }

  void onMultiline()
  {
    m_tab.setMultiline(!m_tab.isMultiline());
  }

  bool onUpdateMultiline()
  {
    getMenuBar()->getMenuItemById(ID_MULTILINE)
      ->setChecked(m_tab.isMultiline());
    return (m_tab.getSide() != Side::Left &&
	    m_tab.getSide() != Side::Right);
  }

  // void onUpdateSide(MenuItemEvent& ev)
  // {
  //   if (SideMenuItem* menuItem = dynamic_cast<SideMenuItem*>(ev.getMenuItem())) {
  //     menuItem->setRadio(m_tab.getSide() == menuItem->getSide());
  //   }
  // }

  void onPageLink(int page)
  {
    m_tab.setActivePage(page);
    updatePage();
  }

  void onPageChange()
  {
    updatePage();
    m_labelLast.setText("Last clicked " + m_tab.getPageText(m_tab.getActivePage()));
  }

  void onReadMe()
  {
    // TODO remove this
    MsgBox::show(this, "Win32 Limitations",
		 "A Tab widget with pages in Left or Right sides\n"
		 "must be Multiline. Also, when you use themes on\n"
		 "WinXP, you can't use Left or Right sides at all.\n");
  }

private:

  void updatePage()
  {
    m_labelHere.setText("We are in " + m_tab.getPageText(m_tab.getActivePage()));
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
  Example* app(new Example);
  app->run();
  delete app;
  return 0;
}
