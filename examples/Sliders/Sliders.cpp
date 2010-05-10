// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
#include <memory>
#include "../resource.h"

using namespace Vaca;

class MainFrame : public Dialog
{
  Label m_rangeL;
  TextEdit m_minValue;
  TextEdit m_maxValue;
  Label m_valueL;
  TextEdit m_value;
  GroupBox m_orienL;
  GroupBox m_sidesL;
  GroupBox m_ticks;
  RadioGroup m_orienGroup;
  RadioButton m_horz;
  RadioButton m_vert;
  CheckBox m_side1;
  CheckBox m_side2;
  CheckBox m_showTicks;
  Label m_tickFreqL;
  TextEdit m_tickFreq;
  Separator m_sep;
  Slider m_slider;

public:

  MainFrame()
    : Dialog(L"Sliders", NULL, Dialog::Styles::Default +
			       Frame::Styles::Resizable)
    , m_rangeL(L"Range (Min/Max):", this)
    , m_minValue(L"0", this)
    , m_maxValue(L"100", this)
    , m_valueL(L"Value:", this)
    , m_value(L"100", this)
    , m_orienL(L"Orientation:", this)
    , m_sidesL(L"Sides:", this)
    , m_ticks(L"Ticks:", this)
    , m_horz(L"Horizontal", m_orienGroup, &m_orienL)
    , m_vert(L"Vertical", m_orienGroup, &m_orienL)
    , m_side1(L"Top", &m_sidesL)
    , m_side2(L"Bottom", &m_sidesL)
    , m_showTicks(L"Visible", &m_ticks)
    , m_tickFreqL(L"Frequency:", &m_ticks)
    , m_tickFreq(L"", &m_ticks)
    , m_sep(this)
    , m_slider(this)
  {
    setLayout(Bix::parse(L"X[Y[XY[%,eX[%,%];%,%],X[%,%],%],%,f%]",
			 &m_rangeL, &m_minValue, &m_maxValue,
			 &m_valueL, &m_value,
			 &m_orienL, &m_sidesL,
			 &m_ticks,
			 &m_sep, &m_slider));
    m_orienL.setLayout(new BoxLayout(Orientation::Vertical, false));
    m_sidesL.setLayout(new BoxLayout(Orientation::Vertical, false));
    m_ticks.setLayout(Bix::parse(L"Y[%,X[%,%]]",
				 &m_showTicks,
				 &m_tickFreqL, &m_tickFreq));

    m_minValue.setPreferredSize(Size(64, m_minValue.getPreferredSize().h));
    m_maxValue.setPreferredSize(Size(64, m_maxValue.getPreferredSize().h));
    m_value.setPreferredSize(Size(64, m_value.getPreferredSize().h));
    m_tickFreq.setPreferredSize(Size(64, m_tickFreq.getPreferredSize().h));

    m_minValue.Change.connect(Bind(&MainFrame::onRangesChange, this));
    m_maxValue.Change.connect(Bind(&MainFrame::onRangesChange, this));
    m_value.Change.connect(Bind(&MainFrame::onRangesChange, this));

    m_slider.Change.connect(Bind(&MainFrame::onSliderChange, this));

    m_orienGroup.setSelectedIndex(0);
    m_orienGroup.Change.connect(&MainFrame::onStyleChange, this);
    m_side1.Click.connect(&MainFrame::onStyleChange, this);
    m_side2.Click.connect(&MainFrame::onStyleChange, this);
    m_showTicks.Click.connect(&MainFrame::onStyleChange, this);
    m_tickFreq.Change.connect(&MainFrame::onStyleChange, this);

    onRangesChange();

    setSize(getPreferredSize()+Size(128, 0));
    center();
  }

private:

  // updates the m_slider widget through the current configuration of
  // in the other widgets
  void onRangesChange()
  {
    int minValue = convert_to<int>(m_minValue.getText());
    int maxValue = convert_to<int>(m_maxValue.getText());
    int value = convert_to<int>(m_value.getText());

    bool minValueError = (minValue < Slider::MinLimit || minValue > maxValue || minValue > value);
    bool maxValueError = (maxValue > Slider::MaxLimit || maxValue < minValue || maxValue < value);
    bool valueError = (value < minValue || value > maxValue);

    updateEditColors(m_minValue, minValueError);
    updateEditColors(m_maxValue, maxValueError);
    updateEditColors(m_value, valueError);

    if (minValueError || maxValueError || valueError)
      return;

    m_slider.setRange(minValue, maxValue);
    m_slider.setValue(value);
  }

  void updateEditColors(TextEdit& edit, bool error)
  {
    edit.setBgColor(error ? Color::Red: Color::White);
    edit.setFgColor(error ? Color::White: Color::Black);
    edit.invalidate(true);
  }

  void onStyleChange(Event& ev)
  {
    bool horz = m_horz.isSelected();

    m_slider.setOrientation(horz ? Orientation::Horizontal:
				   Orientation::Vertical);

    Sides sides;

    if (horz) {
      m_side1.setText(L"Top");
      m_side2.setText(L"Bottom");

      if (m_side1.isSelected()) sides |= Sides::Top;
      if (m_side2.isSelected()) sides |= Sides::Bottom;
    }
    else {
      m_side1.setText(L"Left");
      m_side2.setText(L"Right");

      if (m_side1.isSelected()) sides |= Sides::Left;
      if (m_side2.isSelected()) sides |= Sides::Right;
    }

    m_slider.setSides(sides);
    m_slider.setTickVisible(m_showTicks.isSelected());
    m_slider.setTickFreq(convert_to<int>(m_tickFreq.getText()));
  }

  void onSliderChange()
  {
    m_value.setText(convert_to<String>(m_slider.getValue()));
  }

};

int vaca_main()
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

#include "Vaca/main.h"
