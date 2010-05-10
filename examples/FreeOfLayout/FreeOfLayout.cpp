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

int vaca_main()
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

#include "Vaca/main.h"
