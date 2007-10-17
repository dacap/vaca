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

class SlidersPanel : public Panel
{
  Slider *mSlider[6];

public:

  SlidersPanel(Orientation orientation, Widget *parent)
    : Panel(parent)
  {
    bool isHorz = (orientation == Horizontal);
      
    setLayout(new BoxLayout(isHorz ? Vertical: Horizontal, false));

    for (int c=0; c<6; ++c) {
      // create the slider
      mSlider[c] = new Slider(this);

      // sliders have a range of 0-100 by default
      mSlider[c]->setRange(0, c < 2 ? 100: 10);
      
      // sliders are horizontal by default
      mSlider[c]->setOrientation(orientation);

      // sliders don't have tick marks by default, so we'll show tick
      // marks on sliders 2, 3, and 5
      if (c == 2 || c == 3 || c == 5) {
	// set tick marks to a visible state
	mSlider[c]->setVisibleTickMarks(true);

	// add a tick mark in the middle (the first and the last tick
	// marks are added automatically)
	mSlider[c]->addTickMark((mSlider[c]->getMinimum() +
				 mSlider[c]->getMaximum()) / 2);
      }
    }

    mSlider[0]->pointToSide(isHorz ? BottomSide: RightSide);
    mSlider[1]->pointToSide(isHorz ? TopSide:    LeftSide);
    mSlider[2]->pointToSide(isHorz ? BottomSide: RightSide);
    mSlider[3]->pointToSide(isHorz ? TopSide:    LeftSide);
    mSlider[4]->pointNowhere();
    mSlider[5]->pointNowhere();
  }

  virtual ~SlidersPanel()
  {
    for (int c=0; c<6; ++c)
      delete mSlider[c];
  }

  void connectSliders()
  {
    for (int c=0; c<3; ++c) {
      Slider *s1 = mSlider[c*2  ];
      Slider *s2 = mSlider[c*2+1];

      s1->Change.connect(Bind(&SlidersPanel::onSliderChange, this, s1, s2));
      s2->Change.connect(Bind(&SlidersPanel::onSliderChange, this, s2, s1));

      onSliderChange(s1, s2);
    }
  }

protected:
  
  void onSliderChange(Slider *source, Slider *destination)
  {
    destination->setPosition(source->getPosition());
  }
  
};

class MainFrame : public Frame
{
  Panel mTopPanel;
  SlidersPanel mLeftPanel;
  SlidersPanel mRightPanel;
  Button mButton;

public:

  MainFrame()
    : Frame("Sliders")
    , mTopPanel(this)
    , mLeftPanel(Horizontal, &mTopPanel)
    , mRightPanel(Vertical, &mTopPanel)
    , mButton("Connect Sliders", this)
  {
    // prepare layout
    setLayout(new BoxLayout(Vertical, false)); // no-homogeneous
    mTopPanel.setConstraint(new BoxConstraint(true)); // expansible
    mTopPanel.setLayout(new BoxLayout(Horizontal, true, 0)); // homogeneous, no-border

    // when the "mButton" is pressed, this is the sequence of commands
    // to execute:
    //   ...
    //   mLeftPanel.connectSliders();
    //   mRightPanel.connectSliders();
    //   mButton.setEnabled(false);
    //   ...
    mButton.Action.connect(Bind(&SlidersPanel::connectSliders, &mLeftPanel));
    mButton.Action.connect(Bind(&SlidersPanel::connectSliders, &mRightPanel));
    mButton.Action.connect(Bind(&Button::setEnabled, &mButton, false));

    // set the size, center the frame, and we are done
    setSize(preferredSize());
    center();
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
