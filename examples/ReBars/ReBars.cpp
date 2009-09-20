// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, Jie Zhang
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
#include "resource.h"

using namespace Vaca;

using namespace Vaca;

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
    e.show();
  }
  return 0;
}
