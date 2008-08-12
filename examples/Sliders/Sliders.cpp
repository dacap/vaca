// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

class SlidersPanel : public Panel
{
  Slider* m_slider[6];

public:

  SlidersPanel(Orientation orientation, Widget* parent)
    : Panel(parent)
  {
    bool isHorz = (orientation == Orientation::Horizontal);
      
    setLayout(new BoxLayout(isHorz ? Orientation::Vertical:
				     Orientation::Horizontal, false));

    for (int c=0; c<6; ++c) {
      // create the slider
      m_slider[c] = new Slider(this);

      // sliders have a range of 0-100 by default
      m_slider[c]->setRange(0, c < 2 ? 100: 10);
      
      // sliders are horizontal by default
      m_slider[c]->setOrientation(orientation);

      // sliders don't have tick marks by default, so we'll show tick
      // marks on sliders 2, 3, and 5
      if (c == 2 || c == 3 || c == 5) {
	// set tick marks to a visible state
	m_slider[c]->setVisibleTickMarks(true);

	// add a tick mark in the middle (the first and the last tick
	// marks are added automatically)
	m_slider[c]->addTickMark((m_slider[c]->getMinimum() +
				 m_slider[c]->getMaximum()) / 2);
      }
    }

    m_slider[0]->pointToSide(isHorz ? Side::Bottom: Side::Right);
    m_slider[1]->pointToSide(isHorz ? Side::Top:    Side::Left);
    m_slider[2]->pointToSide(isHorz ? Side::Bottom: Side::Right);
    m_slider[3]->pointToSide(isHorz ? Side::Top:    Side::Left);
    m_slider[4]->pointNowhere();
    m_slider[5]->pointNowhere();
  }

  virtual ~SlidersPanel()
  {
    for (int c=0; c<6; ++c)
      delete m_slider[c];
  }

  void connectSliders()
  {
    for (int c=0; c<3; ++c) {
      Slider* s1 = m_slider[c*2  ];
      Slider* s2 = m_slider[c*2+1];

      s1->Change.connect(Bind(&SlidersPanel::onSliderChange, this, s1, s2));
      s2->Change.connect(Bind(&SlidersPanel::onSliderChange, this, s2, s1));

      onSliderChange(s1, s2);
    }
  }

protected:
  
  void onSliderChange(Slider* source, Slider* destination)
  {
    destination->setValue(source->getValue());
  }
  
};

class MainFrame : public Frame
{
  Panel m_topPanel;
  SlidersPanel m_leftPanel;
  SlidersPanel m_rightPanel;
  Button m_button;

public:

  MainFrame()
    : Frame("Sliders")
    , m_topPanel(this)
    , m_leftPanel(Orientation::Horizontal, &m_topPanel)
    , m_rightPanel(Orientation::Vertical, &m_topPanel)
    , m_button("Connect Sliders", this)
  {
    // prepare layout
    setLayout(new BoxLayout(Orientation::Vertical, false)); // no-homogeneous
    m_topPanel.setConstraint(new BoxConstraint(true)); // expansible
    m_topPanel.setLayout(new BoxLayout(Orientation::Horizontal, true, 0)); // homogeneous, no-border

    // when the "m_button" is pressed, this is the sequence of commands
    // to execute:
    //   ...
    //   m_leftPanel.connectSliders();
    //   m_rightPanel.connectSliders();
    //   m_button.setEnabled(false);
    //   ...
    m_button.Action.connect(Bind(&SlidersPanel::connectSliders, &m_leftPanel));
    m_button.Action.connect(Bind(&SlidersPanel::connectSliders, &m_rightPanel));
    m_button.Action.connect(Bind(&Button::setEnabled, &m_button, false));

    // set the size, center the frame, and we are done
    setSize(getPreferredSize());
    center();
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main() {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example* app(new Example);
  app->run();
  delete app;
  return 0;
}
