// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

class MainFrame : public Dialog
{
  Label m_usernameLabel;
  TextEdit m_usernameEdit;
  Label m_passwordLabel;
  TextEdit m_passwordEdit;
  CheckBox m_administrator;
  Button m_loginButton;
  Button m_cancelButton;

public:

  MainFrame()
    : Dialog(L"FreeOfLayout")
    , m_usernameLabel(L"&Username:", this)
    , m_usernameEdit(L"", this)
    , m_passwordLabel(L"&Password:", this)
    , m_passwordEdit(L"", this, TextEdit::Styles::Password)
    , m_administrator(L"&Administrador account", this)
    , m_loginButton(L"&Login", IDOK, this)
    , m_cancelButton(L"&Cancel", IDCANCEL, this)
  {
    setIcon(ResourceId(IDI_VACA));

    // if we don't setup a layout manager through this->setLayout(),
    // we are free of layout manager...

    m_usernameLabel.setBounds(2,                   2,          70,  23);
    m_usernameEdit .setBounds(2+70+2,              2,          130, 23);
    m_passwordLabel.setBounds(2,                   2+(23+2)*1, 70,  23);
    m_passwordEdit .setBounds(2+70+2,              2+(23+2)*1, 130, 23);
    m_administrator.setBounds(2+70+2,              2+(23+2)*2, 130, 23);
    m_loginButton  .setBounds(2+70+2+130-(66+2)*2, 2+(23+2)*3, 66,  23);
    m_cancelButton .setBounds(2+70+2+130-(66+2)*1, 2+(23+2)*3, 66,  23);

    m_loginButton.setDefault(true);

    m_administrator.Click.connect(&MainFrame::onAdministrator, this);

    // so because we are not using layout managers we can't use
    // getPreferredSize method, so we have to setup the window
    // size manually
    setSize(getNonClientSize() +
	    Size(2+70+2+130+2, 2+(23+2)*4));
    center();
  }

  String getUserName()
  {
    return m_usernameEdit.getText();
  }

protected:

  void onAdministrator(Event& ev)
  {
    bool enabled = !m_administrator.isSelected();
    m_usernameLabel.setEnabled(enabled);
    m_usernameEdit.setEnabled(enabled);
    if (!enabled)
      m_usernameEdit.setText(L"root");
  }

};

int VACA_MAIN()
{
  Application app;
  MainFrame dlg;

  if (dlg.doModal())
    MsgBox::show(&dlg, L"Information",
		 L"Welcome '" + dlg.getUserName() + L"'",
		 MsgBox::Type::Ok,
		 MsgBox::Icon::Information);
  else
    MsgBox::show(&dlg, L"Information",
		 L"You canceled the operation",
		 MsgBox::Type::Ok,
		 MsgBox::Icon::Information);
  return 0;
}
