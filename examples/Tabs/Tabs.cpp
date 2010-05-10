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

#include <Vaca/Vaca.h>
#include "../resource.h"

using namespace Vaca;

enum {
  ID_EXIT = 1000,
  ID_CHANGE_FONT,
  ID_MULTILINE,
  ID_READ_ME,
  ID_TOP,
  ID_RIGHT,
  ID_LEFT,
  ID_BOTTOM,
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

  class SideMenuItem : public MenuItem
  {
    Side m_side;
    TabBase& m_tab;

  public:
    SideMenuItem(const String& text, CommandId id, Side side, TabBase& tab)
      : MenuItem(text, id)
      , m_side(side)
      , m_tab(tab) { }

    virtual void onClick(MenuItemEvent& ev) {
      MenuItem::onClick(ev);
      m_tab.setSide(m_side);
    }
    
    virtual void onUpdate(MenuItemEvent& ev) {
      MenuItem::onUpdate(ev);
      setRadio(m_tab.getSide() == m_side);
    }
    
  };

public:

  MainFrame()
    : Frame(L"Tabs")
    , m_tab(this)
    , m_labelHere(L"", &m_tab)
    , m_labelLast(L"Start", &m_tab)
    , m_label0(L"Go to page0", &m_tab)
    , m_label1(L"Go to page1", &m_tab)
    , m_label2(L"Go to page2", &m_tab)
    , m_label3(L"Go to page3", &m_tab)
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

    m_tab.addPage(L"Page0");
    m_tab.addPage(L"Page1");
    m_tab.addPage(L"Page2");
    m_tab.addPage(L"Page3");

    m_tab.PageChange.connect(Bind(&MainFrame::onPageChange, this));

    // labels
    m_label0.Click.connect(Bind(&MainFrame::onPageLink, this, 0));
    m_label1.Click.connect(Bind(&MainFrame::onPageLink, this, 1));
    m_label2.Click.connect(Bind(&MainFrame::onPageLink, this, 2));
    m_label3.Click.connect(Bind(&MainFrame::onPageLink, this, 3));

    updatePage();

    setSize(256, getPreferredSize().h);
  }

private:

  MenuBar* createMenuBar()
  {
    MenuBar* menuBar  = new MenuBar();
    Menu* appMenu     = new Menu(L"&Tabs");
    Menu* optionsMenu = new Menu(L"&Options");
    Menu* sideMenu    = new Menu(L"&Side");

    appMenu->add(L"E&xit", ID_EXIT);

    optionsMenu->add(L"Change &Font", ID_CHANGE_FONT);
    optionsMenu->add(L"&Multiline", ID_MULTILINE);
    optionsMenu->add(sideMenu);

    sideMenu->add(new SideMenuItem(L"&Top", ID_TOP, Side::Top, m_tab));
    sideMenu->add(new SideMenuItem(L"&Left", ID_LEFT, Side::Left, m_tab));
    sideMenu->add(new SideMenuItem(L"&Bottom", ID_BOTTOM, Side::Bottom, m_tab));
    sideMenu->add(new SideMenuItem(L"&Right", ID_RIGHT, Side::Right, m_tab));

    menuBar->add(appMenu);
    menuBar->add(optionsMenu);
    menuBar->add(L"&Read me", ID_READ_ME);

    return menuBar;
  }

  void onChangeFont()
  {
    FontDialog dialog(m_tab.getFont(), this);
    if (dialog.doModal()) {
      m_tab.setFont(dialog.getFont());
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
    m_labelLast.setText(L"Last clicked " + m_tab.getPageText(m_tab.getActivePage()));
  }

  void onReadMe()
  {
    // TODO remove this
    MsgBox::show(this, L"Win32 Limitations",
		 L"A Tab widget with pages in Left or Right sides\n"
		 L"must be Multiline. Also, when you use themes on\n"
		 L"WinXP, you can't use Left or Right sides at all.\n");
  }

private:

  void updatePage()
  {
    m_labelHere.setText(L"We are in " + m_tab.getPageText(m_tab.getActivePage()));
  }

};

//////////////////////////////////////////////////////////////////////

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
