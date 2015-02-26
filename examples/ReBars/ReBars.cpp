// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, Jie Zhang
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "resource.h"

using namespace vaca;

using namespace vaca;

class MainFrame : public Frame
{
  ReBar rebar;
  TextEdit ed;

public:

  MainFrame()
    : Frame(L"Hello, ReBar!")
    , rebar(this)
    , ed(L"Hello,world!", this, TextEdit::Styles::TextArea)
  {
    setLayout(new ClientLayout);

    ComboBox* comboBox = new ComboBox(&rebar);
    comboBox->addItem(L"First element");
    comboBox->addItem(L"Second element");
    comboBox->addItem(L"Third element");

    ReBarBand rb = rebar.addBand(comboBox);
    rb.setText(L"Wow~~~ , do you like it?"); // TODO fix me
    rb.setColors(Color::Green, Color::Yellow); // TODO fix me

    rb = rebar.addBand(new Label(L"A Label in a ReBarBand", &rebar));

    rb = rebar.addBand(new Label(L"I am a fixed pane!", &rebar),
		       ReBarBandStyle::Simple |
		       ReBarBandStyle::FixedSize |
		       ReBarBandStyle::NoGripper);

    rb = rebar.addBand(new TextEdit(L"Hello, ReBar!", &rebar),
		       ReBarBandStyle::Simple |
		       ReBarBandStyle::Break);
    rb.setText(L"Another band!!!");
    rb.setColors(Color::Blue, Color::Red);

    // TODO fix me
    ToolSet* toolset = new ToolSet(&rebar);
    toolset->addButton(new ToolButton(CommandId(1), -1, L"Hi"));
    toolset->addButton(new ToolButton(CommandId(2), -1, L"Bye"));
    // toolset->setImageList(ImageList(ResourceId(IDR_TOOLBAR), 16, System::getColor(COLOR_BTNFACE)));
    rb = rebar.addBand(toolset);
  }

};

int VACA_MAIN()
{
  try {
    Application app;
    MainFrame frm;
    frm.setIcon(ResourceId(IDI_VACA));
    frm.setVisible(true);
    app.run();
  }
  catch (Exception& e) {
    MsgBox::showException(NULL, e);
  }
  return 0;
}
