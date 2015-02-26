// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

class MainFrame : public Frame
{
  Label m_label1; ComboBox m_combo1;
  Label m_label2; ComboBox m_combo2;

public:

  MainFrame()
    : Frame(L"ComboBoxes")
    , m_label1(L"ComboBox::Styles::Default", this)
    , m_combo1(this)
    , m_label2(L"ComboBox::Styles::Editable", this)
    , m_combo2(this, ComboBox::Styles::Editable)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));

    for (int c=0; c<8; ++c) {
      String text = format_string(L"Item %d", c);
      m_combo1.addItem(text);
      m_combo2.addItem(text);
    }

    m_combo1.SelChange.connect(&MainFrame::onComboChange, this);

    setSize(getPreferredSize());
    center();
  }

protected:

  virtual void onResizing(CardinalDirection dir, Rect &rc)
  {
    rc.y = getBounds().y;

    if (dir != CardinalDirection::North &&
	dir != CardinalDirection::South) {
      // the height is calculated through the width that the user wants
      rc.h = getPreferredSize(Size(rc.getSize().w, 0)).h;
    }
    else {
      rc.h = getBounds().h;
    }
  }

private:

  void onComboChange(Event& ev)
  {
    m_combo2.setSelectedItem(m_combo1.getSelectedItem());
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
