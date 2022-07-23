// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

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

    addCommand(new SignalCommand(ID_EXIT, [this]{ setVisible(false); }));
    addCommand(new SignalCommand(ID_CHANGE_FONT, &MainFrame::onChangeFont, this));
    addCommand(cmd = new SignalCommand(ID_MULTILINE));
    cmd->Execute.connect([this]{ onMultiline(); });
    cmd->Enabled.connect([this]{ return onUpdateMultiline(); });
    addCommand(new SignalCommand(ID_READ_ME, &MainFrame::onReadMe, this));

    // m_tab
    m_tab.setLayout(new BoxLayout(Orientation::Vertical, false));

    m_tab.addPage(L"Page0");
    m_tab.addPage(L"Page1");
    m_tab.addPage(L"Page2");
    m_tab.addPage(L"Page3");

    m_tab.PageChange.connect([this]{ onPageChange(); });

    // labels
    m_label0.Click.connect([this]{ onPageLink(0); });
    m_label1.Click.connect([this]{ onPageLink(1); });
    m_label2.Click.connect([this]{ onPageLink(2); });
    m_label3.Click.connect([this]{ onPageLink(3); });

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

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
