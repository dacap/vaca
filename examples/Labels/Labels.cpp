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
  Label m_longLabel;
  Label m_ellipsis1Label;
  Label m_ellipsis2Label;
  Label m_ellipsis3Label;
  Button m_alignButton;

public:

  MainFrame()
    : Frame(L"Labels", NULL, Frame::Styles::Default
			     - Frame::Styles::Maximizable)
    , m_longLabel(L"This is a long label to test the word wrap. Also, see\r\n"
		  L"how the BoxLayout manager fit all in the right place.", this)
    , m_ellipsis1Label(L"A label with word-ellipsis style.",
		       this, Label::Styles::Default +
			     Label::Styles::WordEllipsis)
    , m_ellipsis2Label(L"C:\\folder\\subFolder\\subSubFolder\\fileName.txt",
		       this, Label::Styles::Default +
			     Label::Styles::EndEllipsis)
    , m_ellipsis3Label(L"C:\\folder\\subFolder\\subSubFolder\\fileName.txt",
		      this, Label::Styles::Default +
			    Label::Styles::PathEllipsis)
    , m_alignButton(L"Set Alignment", this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));

    m_alignButton.Click.connect([this]{ onSetAlignment(); });

    setSize(getPreferredSize());
    center();
  }

protected:

  void onSetAlignment()
  {
    TextAlign align = m_longLabel.getTextAlign();
    switch (align) {
      case TextAlign::Left:   align = TextAlign::Center; break;
      case TextAlign::Center: align = TextAlign::Right;  break;
      case TextAlign::Right:  align = TextAlign::Left;   break;
    }
    m_longLabel.setTextAlign(align);
    m_ellipsis1Label.setTextAlign(align);
    m_ellipsis2Label.setTextAlign(align);
    m_ellipsis3Label.setTextAlign(align);
  }

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
