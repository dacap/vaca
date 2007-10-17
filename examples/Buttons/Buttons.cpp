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
  Panel mLeftPanel;
  Panel mRightPanel;
  Button mButton;
  ToggleButton mToggleButton;
  CheckBox mCheckBox;
  RadioGroup mRadioGroup1;
  RadioGroup mRadioGroup2;
  RadioButton mRadioButton1;
  RadioButton mRadioButton2;
  RadioButton mRadioButton3;
  RadioButton mRadioButton4;
  Label mLabel1;
  Label mLabel2;
  ToggleButton mDisableButton;
  int mActions;

public:

  MainFrame()
    : Frame("Buttons")
    , mLeftPanel(this)
    , mRightPanel(this)
    , mButton("Button", &mLeftPanel)
    , mToggleButton("ToggleButton", &mLeftPanel)
    , mCheckBox("CheckBox", &mLeftPanel)
    , mRadioButton1("RadioButton1_Group1", mRadioGroup1, &mLeftPanel)
    , mRadioButton2("RadioButton2_Group1", mRadioGroup1, &mLeftPanel)
    , mRadioButton3("RadioButton3_Group2", mRadioGroup2, &mLeftPanel)
    , mRadioButton4("RadioButton4_Group2", mRadioGroup2, &mLeftPanel)
    , mLabel1("", &mRightPanel)
    , mLabel2("", &mRightPanel)
    , mDisableButton("All Disabled", &mRightPanel)
    , mActions(-1)
  {
    setLayout(new BoxLayout(Horizontal, true)); // homogeneous
    mLeftPanel.setLayout(new BoxLayout(Vertical, true)); // homogeneous
    mRightPanel.setLayout(new BoxLayout(Vertical, false)); // no-homogeneous

    updateLabel();

    bindButton(mButton);
    bindButton(mToggleButton);
    bindButton(mCheckBox);
    bindButton(mRadioButton1);
    bindButton(mRadioButton2);
    bindButton(mRadioButton3);
    bindButton(mRadioButton4);

    mDisableButton.Action.connect(Bind(&MainFrame::toggleDisabled, this));

    setSize(preferredSize());
    center();
  }

private:

  void updateLabel()
  {
    mActions++;
    mLabel1.setText(String("Action signal fired ") + String::fromInt(mActions) + " time(s)");
    mLabel2.setText(String("CheckBox's ") + (mCheckBox.isSelected() ? "checked": " unchecked"));

    // the labels are bigger and bigger, so a relayout isn't a bad idea
    layout();
  }

  void bindButton(ButtonBase &button)
  {
    button.Action.connect(Bind(&MainFrame::updateLabel, this));
  }

  void toggleDisabled()
  {
    bool state = !mDisableButton.isSelected();
    mButton.setEnabled(state);
    mToggleButton.setEnabled(state);
    mCheckBox.setEnabled(state);
    mRadioButton1.setEnabled(state);
    mRadioButton2.setEnabled(state);
    mRadioButton3.setEnabled(state);
    mRadioButton4.setEnabled(state);
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
