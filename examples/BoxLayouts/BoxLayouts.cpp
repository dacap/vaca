// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include <Vaca/Vaca.hpp>

using namespace Vaca;

class MainFrame : public Frame
{
  Panel m_top;			// m_top's the panel to test
  Panel m_bottom;
  Panel m_bottomSub1;
  Panel m_bottomSub2;
  Panel m_bottomSub3;
  Label m_label;
  MultilineEdit m_edit;
  Button m_button;
  RadioGroup m_orientation;
  RadioGroup m_distribution;
  RadioButton m_horizontal;
  RadioButton m_vertical;
  RadioButton m_homogeneous;
  RadioButton m_heterogeneous;
  ToggleButton m_expandLabel;
  ToggleButton m_expandEdit;
  ToggleButton m_expandButton;
  
public:

  MainFrame()
    : Frame("BoxLayouts")
    , m_top(this)
    , m_bottom(this)
    , m_bottomSub1(&m_bottom)
    , m_bottomSub2(&m_bottom)
    , m_bottomSub3(&m_bottom)
    , m_label("Label", &m_top)
    , m_edit("Edit", &m_top, MultilineEditStyle + ScrollStyle)
    , m_button("Button", &m_top)
    , m_horizontal("Horizontal", m_orientation, &m_bottomSub1)
    , m_vertical("Vertical", m_orientation, &m_bottomSub1)
    , m_homogeneous("Homogeneous", m_distribution, &m_bottomSub2)
    , m_heterogeneous("Heterogeneous", m_distribution, &m_bottomSub2)
    , m_expandLabel("Expand Label", &m_bottomSub3)
    , m_expandEdit("Expand Edit", &m_bottomSub3)
    , m_expandButton("Expand Button", &m_bottomSub3)
  {
    // MainFrame layout
    setLayout(new BoxLayout(Orientation::Vertical, false)); // heterogeneous
    m_top.setConstraint(new BoxConstraint(true)); // expansive

    // other layouts
    m_top.setLayout(new BoxLayout(Orientation::Vertical, false)); // heterogeneous
    m_edit.setConstraint(new BoxConstraint(true)); // expansive
    
    // other layouts
    m_bottom.setLayout(new BoxLayout(Orientation::Horizontal, false)); // heterogeneous
    m_bottomSub1.setLayout(new BoxLayout(Orientation::Vertical, true)); // homogeneous
    m_bottomSub2.setLayout(new BoxLayout(Orientation::Vertical, true)); // homogeneous
    m_bottomSub3.setLayout(new BoxLayout(Orientation::Horizontal, true)); // homogeneous

    // m_top begins vertical and heterogeneous, and m_edit begins expansive
    m_vertical.setSelected(true);
    m_heterogeneous.setSelected(true);
    m_expandEdit.setSelected(true);

    // bindings...
    m_horizontal.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    m_vertical.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    m_homogeneous.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    m_heterogeneous.Action.connect(Bind(&MainFrame::onSetupLayout, this));
    m_expandLabel.Action.connect(Bind(&MainFrame::onSetupConstraints, this));
    m_expandEdit.Action.connect(Bind(&MainFrame::onSetupConstraints, this));
    m_expandButton.Action.connect(Bind(&MainFrame::onSetupConstraints, this));

    // relayout when edit the text (to see how Edit::onPreferredSize
    // modifies the BoxLayout behavior)
    m_edit.Change.connect(Bind(&MainFrame::layout, this));
  }

protected:

  void onSetupLayout()
  {
    Orientation newOrientation = m_vertical.isSelected() ? Orientation::Vertical:
							   Orientation::Horizontal;

    bool isHomogeneous = m_homogeneous.isSelected();
    
    // setup the new layout manager, and hold the old layout
    Layout *oldLayout =
      m_top.setLayout(new BoxLayout(newOrientation,
				    isHomogeneous));

    // delete the old layout manager
    delete oldLayout;

    // relayout the m_top panel
    m_top.layout();

    // when the BoxLayout is homogeneous, the expansive constraint
    // doesn't have effect
    m_expandLabel .setEnabled(!isHomogeneous);
    m_expandEdit  .setEnabled(!isHomogeneous);
    m_expandButton.setEnabled(!isHomogeneous);
  }

  void onSetupConstraints()
  {
    // when we set a constraint, we must to delete the old returned
    // constraint...
    delete m_label.setConstraint (new BoxConstraint(m_expandLabel .isSelected()));
    delete m_edit.setConstraint  (new BoxConstraint(m_expandEdit  .isSelected()));
    delete m_button.setConstraint(new BoxConstraint(m_expandButton.isSelected()));

    // relayout the m_top panel
    m_top.layout();
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
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
