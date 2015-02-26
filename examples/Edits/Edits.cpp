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
  TextEdit m_edit1;
  TextEdit m_edit2;
  TextEdit m_edit3;
  TextEdit m_edit4;

public:

  MainFrame()
    : Frame(L"Edits")
    , m_edit1(L"Default TextEdit widget with some text that you can edit.", this)
    , m_edit2(L"A TextEdit widget without client-edge and right-aligned.", this,
	      TextEdit::Styles::Default +
	      TextEdit::Styles::RightAligned
	      - Widget::Styles::ClientEdge)
    , m_edit3(L"A read-only TextEdit widget without client-edge and with modified bgColor", this,
	      TextEdit::Styles::Default
	      - Widget::Styles::ClientEdge)
    , m_edit4(L"A TextEdit widget with TextArea style.\r\nIt has multiple lines\r\nof text...", this,
	      TextEdit::Styles::TextArea +
	      Widget::Styles::Scroll)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_edit4.setConstraint(new BoxConstraint(true));

    m_edit3.setReadOnly(true);
    m_edit3.setBgColor(System::getColor(COLOR_3DFACE));

    setSize(getPreferredSize());
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
