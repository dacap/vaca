// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <Vaca/Vaca.h>
#include "../resource.h"

using namespace Vaca;

class MainFrame : public Frame
{
  SpinButton m_spinButton1;
  SpinButton m_spinButton2;
  SpinButton m_spinButton3;
  Spinner m_spinner1;
  Spinner m_spinner2;

public:

  MainFrame()
    : Frame(L"Spinners")
    , m_spinButton1(this)
    , m_spinButton2(this, SpinButton::Styles::Default +
			  SpinButton::Styles::Horizontal)
    , m_spinButton3(this, SpinButton::Styles::Default +
			  SpinButton::Styles::HotTrack)
    , m_spinner1(0, 10, 0, this)
    , m_spinner2(-5, 5, 0, this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_spinButton1.setConstraint(new BoxConstraint(true));
    m_spinButton2.setConstraint(new BoxConstraint(true));
    m_spinButton3.setConstraint(new BoxConstraint(true));

    m_spinButton1.Change.connect(&MainFrame::onSpinButtonChange, this);
    m_spinButton2.Change.connect(&MainFrame::onSpinButtonChange, this);

    setSize(Size(200, 160));
    center();
  }

protected:
  
  void onSpinButtonChange(SpinButtonEvent& ev)
  {
    if (Spinner* spinner = dynamic_cast<Spinner*>(ev.getSource())) {
      spinner->setValue(spinner->getValue() + ev.getDelta());
    }
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
