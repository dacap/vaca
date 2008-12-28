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
private:
  ReBar rebar;
  TextEdit ed;

public:
  MainFrame()
    : Frame ("Hello, ReBar!")
    , rebar(this)
    , ed("Hello,world!", this, TextEdit::Styles::TextArea)
  {
    // maybe we should add a Frame::setReBar(ReBar* rebar) function
    // if we use setLayout (new ClientLayout), 
    // then ed control will fill the whole client bound and cover the rebar controls

    ReBarBand rb = rebar.addBand(new ComboBox(&rebar));
    rb.setText("Wow~~~ , do you like it? ");
    rb.setColors(Color::Green, Color::Yellow);

    rb = rebar.addBand(new Label(" I am a fixed pane! ", &rebar), 
		       ReBarBandStyle::Simple |
		       ReBarBandStyle::FixedSize |
		       ReBarBandStyle::NoGripper);

    rb = rebar.addBand(new TextEdit("Hello, ReBar!", &rebar),
		       ReBarBandStyle::Simple |
		       ReBarBandStyle::Break);
    rb.setText("Another band!!!");
    rb.setColors(Color::Blue, Color::Red);

    //		ToolSet * toolset = new ToolSet(&rebar);
    //		toolset->setImageList (ImageList(ResourceId(IDR_TOOLBAR1), 16, System::getColor(COLOR_BTNFACE)));
    //		toolset->updatePreferredSizes();
    // rebar will throw an exception as rebar need ToolSet::getPreferredSize() function work
    // but the function of toolset is not working correctly !!!
    //		rb = rebar.addBand (toolset);
    rebar.AutoSize.connect(&MainFrame::onAutoSize, this);
  }

public:
  void onAutoSize(Event& ev)
  {
    layoutReBar();
  }

protected:
  virtual void onResize(const Size& sz)	// make sure rebar will resize itself automatic
  {
    rebar.sendMessage(WM_SIZE, 0, 0);
  }

  void layoutReBar()	// layout the real client widget
  {
    Rect rc = getClientBounds();
    invalidate(rc, TRUE);
    rc.y += rebar.getBarHeight();
    ed.setBounds(rc);
  }
};

class MyApp : public Application
{
  MainFrame mainWnd;

  virtual void main() {
    mainWnd.setVisible(true);
  }
};

int VACA_MAIN()
{
  MyApp app;
  app.run();
  return 0;
}
