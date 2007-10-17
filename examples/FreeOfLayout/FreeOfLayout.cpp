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

class MainFrame : public Dialog
{
  Label mUsernameLabel;
  Edit mUsernameEdit;
  Label mPasswordLabel;
  PasswordEdit mPasswordEdit;
  CheckBox mAdministrator;
  Button mLoginButton;
  Button mCancelButton;

public:

  MainFrame()
    : Dialog("FreeOfLayout")
    , mUsernameLabel("&Username:", this)
    , mUsernameEdit("", this)
    , mPasswordLabel("&Password:", this)
    , mPasswordEdit("", this)
    , mAdministrator("&Administrador account", this)
    , mLoginButton("&Login", this)
    , mCancelButton("&Cancel", this)
  {
    // if we don't setup a layout manager through this->setLayout(),
    // we are free of layout manager...

    mUsernameLabel.setBounds(2,                   2,          70,  23);
    mUsernameEdit .setBounds(2+70+2,              2,          130, 23);
    mPasswordLabel.setBounds(2,                   2+(23+2)*1, 70,  23);
    mPasswordEdit .setBounds(2+70+2,              2+(23+2)*1, 130, 23);
    mAdministrator.setBounds(2+70+2,              2+(23+2)*2, 130, 23);
    mLoginButton  .setBounds(2+70+2+130-(66+2)*2, 2+(23+2)*3, 66,  23);
    mCancelButton .setBounds(2+70+2+130-(66+2)*1, 2+(23+2)*3, 66,  23);

    mLoginButton.setDefault(true);

    mLoginButton.Action.connect(Bind(&MainFrame::defaultOkAction, this));
    mCancelButton.Action.connect(Bind(&MainFrame::defaultCancelAction, this));
    mAdministrator.Action.connect(Bind(&MainFrame::onAdministrator, this));

    setSize(getNonClientSize() +
	    Size(2+70+2+130+2, 2+(23+2)*4));
    center();
  }

  String getUserName()
  {
    return mUsernameEdit.getText();
  }
  
protected:

  void onAdministrator()
  {
    bool enabled = !mAdministrator.isSelected();
    mUsernameLabel.setEnabled(enabled);
    mUsernameEdit.setEnabled(enabled);
    if (!enabled)
      mUsernameEdit.setText("root");
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
public:
  virtual void main(std::vector<String> args) {
    MainFrame dlg;

    if (dlg.doModal())
      dlg.msgBox("Welcome '"+dlg.getUserName()+"'",
		 "Information",
		 MB_OK | MB_ICONINFORMATION);
    else
      dlg.msgBox("You canceled the operation",
		 "Information",
		 MB_OK | MB_ICONINFORMATION);
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
