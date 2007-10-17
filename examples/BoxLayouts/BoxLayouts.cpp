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
  Panel mTop;			// mTop's the panel to test
  Panel mBottom;
  Panel mBottomSub1;
  Panel mBottomSub2;
  Panel mBottomSub3;
  Label mLabel;
  MultilineEdit mEdit;
  Button mButton;
  RadioGroup mOrientation;
  RadioGroup mDistribution;
  RadioButton mHorizontal;
  RadioButton mVertical;
  RadioButton mHomogeneous;
  RadioButton mHeterogeneous;
  ToggleButton mExpandLabel;
  ToggleButton mExpandEdit;
  ToggleButton mExpandButton;
  
public:

  MainFrame()
    : Frame("BoxLayouts")
    , mTop(this)
    , mBottom(this)
    , mBottomSub1(&mBottom)
    , mBottomSub2(&mBottom)
    , mBottomSub3(&mBottom)
    , mLabel("Label", &mTop)
    , mEdit("Edit", &mTop, MultilineEditStyle + ScrollStyle)
    , mButton("Button", &mTop)
    , mHorizontal("Horizontal", mOrientation, &mBottomSub1)
    , mVertical("Vertical", mOrientation, &mBottomSub1)
    , mHomogeneous("Homogeneous", mDistribution, &mBottomSub2)
    , mHeterogeneous("Heterogeneous", mDistribution, &mBottomSub2)
    , mExpandLabel("Expand Label", &mBottomSub3)
    , mExpandEdit("Expand Edit", &mBottomSub3)
    , mExpandButton("Expand Button", &mBottomSub3)
  {
    // MainFrame layout
    setLayout(new BoxLayout(Vertical, false)); // heterogeneous
    mTop.setConstraint(new BoxConstraint(true)); // expansive

    // other layouts
    mTop.setLayout(new BoxLayout(Vertical, false)); // heterogeneous
    mEdit.setConstraint(new BoxConstraint(true)); // expansive
    
    // other layouts
    mBottom.setLayout(new BoxLayout(Horizontal, false)); // heterogeneous
    mBottomSub1.setLayout(new BoxLayout(Vertical, true)); // homogeneous
    mBottomSub2.setLayout(new BoxLayout(Vertical, true)); // homogeneous
    mBottomSub3.setLayout(new BoxLayout(Horizontal, true)); // homogeneous

    // mTop begins vertical and heterogeneous, and mEdit begins expansive
    mVertical.setSelected(true);
    mHeterogeneous.setSelected(true);
    mExpandEdit.setSelected(true);

    // bindings...
    mHorizontal.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    mVertical.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    mHomogeneous.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    mHeterogeneous.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    mExpandLabel.Action.connect(Bind(&MainFrame::onSetupConstraints, this));
    mExpandEdit.Action.connect(Bind(&MainFrame::onSetupConstraints, this));
    mExpandButton.Action.connect(Bind(&MainFrame::onSetupConstraints, this));

    // relayout when edit the text (to see how Edit::preferredSize
    // modifies the BoxLayout behavior)
    mEdit.Change.connect(Bind(&MainFrame::layout, this));
  }

protected:

  void onSetupLayout()
  {
    Orientation newOrientation = mVertical.isSelected() ? Vertical:
							  Horizontal;
    bool isHomogeneous = mHomogeneous.isSelected();
    
    // setup the new layout manager, and hold the old layout
    Layout *oldLayout =
      mTop.setLayout(new BoxLayout(newOrientation,
				   isHomogeneous));

    // delete the old layout manager
    delete oldLayout;

    // relayout the mTop panel
    mTop.layout();

    // when the BoxLayout is homogeneous, the expansive constraint
    // doesn't have effect
    mExpandLabel .setEnabled(!isHomogeneous);
    mExpandEdit  .setEnabled(!isHomogeneous);
    mExpandButton.setEnabled(!isHomogeneous);
  }

  void onSetupConstraints()
  {
    // when we set the a constraint, we must to delete the old
    // returned constraint...
    delete mLabel.setConstraint (new BoxConstraint(mExpandLabel .isSelected()));
    delete mEdit.setConstraint  (new BoxConstraint(mExpandEdit  .isSelected()));
    delete mButton.setConstraint(new BoxConstraint(mExpandButton.isSelected()));

    // relayout the mTop panel
    mTop.layout();
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
