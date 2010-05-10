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
#include "../resource.h"

using namespace Vaca;

class MainFrame : public Dialog
{
  Widget m_controlPanel;
  Widget m_items;
  Button m_top;
  Button m_up;
  Button m_down;
  Button m_bottom;
  
public:

  MainFrame()
    : Dialog(L"Order")
    , m_controlPanel(this)
    , m_items(this)
    , m_top(L"&Top", &m_controlPanel)
    , m_up(L"&Up", &m_controlPanel)
    , m_down(L"&Down", &m_controlPanel)
    , m_bottom(L"&Bottom", &m_controlPanel)
  {
    setLayout(new BoxLayout(Orientation::Horizontal, false));
    m_controlPanel.setLayout(new BoxLayout(Orientation::Vertical, false));
    m_items.setLayout(new BoxLayout(Orientation::Vertical, false));
    m_items.setConstraint(new BoxConstraint(true));

    m_top.Click.connect(Bind(&MainFrame::onClickTop, this));
    m_up.Click.connect(Bind(&MainFrame::onClickUp, this));
    m_down.Click.connect(Bind(&MainFrame::onClickDown, this));
    m_bottom.Click.connect(Bind(&MainFrame::onClickBottom, this));

    for (int c=0; c<10; ++c) {
      CheckBox* box = new CheckBox(format_string(L"Item %d", c), &m_items);
      if (c == 1)
	box->setSelected(true);
    }

    setSize(getPreferredSize());
    center();
  }

protected:

  void onClickTop()
  {
    WidgetList children = m_items.getChildren();
    WidgetList::reverse_iterator it;

    for (it = children.rbegin(); it != children.rend(); ++it) {
      CheckBox* child = (CheckBox*)*it;

      if (child->isSelected() && child != m_items.getFirstChild())
	child->moveBeforeWidget(m_items.getFirstChild());
    }

    layout();
  }

  void onClickUp()
  {
    WidgetList children = m_items.getChildren();
    WidgetList::iterator it;

    for (it = children.begin(); it != children.end(); ++it) {
      CheckBox* child = (CheckBox*)*it;

      if (child->isSelected() && child->getPreviousSibling())
	child->moveBeforeWidget(child->getPreviousSibling());
    }

    layout();
  }

  void onClickDown()
  {
    WidgetList children = m_items.getChildren();
    WidgetList::reverse_iterator it;

    for (it = children.rbegin(); it != children.rend(); ++it) {
      CheckBox* child = (CheckBox*)*it;

      if (child->isSelected() && child->getNextSibling())
	child->moveAfterWidget(child->getNextSibling());
    }

    layout();
  }

  void onClickBottom()
  {
    WidgetList children = m_items.getChildren();
    WidgetList::iterator it;

    for (it = children.begin(); it != children.end(); ++it) {
      CheckBox* child = (CheckBox*)*it;

      if (child->isSelected() && child != m_items.getLastChild())
	child->moveAfterWidget(m_items.getLastChild());
    }

    layout();
  }

};

//////////////////////////////////////////////////////////////////////

int vaca_main()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}

#include "Vaca/main.h"
