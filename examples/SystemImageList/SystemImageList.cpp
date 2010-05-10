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

class MainFrame : public Frame
{
  Button m_browseButton;
  Label m_listTypeLabel;
  ComboBox m_listType;
  ListView m_listView;

public:

  MainFrame()
    : Frame(L"System Image List")
    , m_browseButton(L"Browse to collect more icons...", this)
    , m_listTypeLabel(L"ListViewType:", this)
    , m_listType(this)
    , m_listView(this)
  {
    setLayout(Bix::parse(L"Y[X[%,%,%],f%]",
			 &m_browseButton, &m_listTypeLabel,
			 &m_listType, &m_listView));

    // Setup the ListView
    m_listView.setType(ListViewType::Icon);
    m_listView.setImageList(System::getImageList());
    m_listView.setSmallImageList(System::getSmallImageList());

    ListColumn* column = new ListColumn(L"Image Index");
    column->setWidth(100);
    m_listView.addColumn(column);

    // Setup list view type combo
    m_listType.addItem(L"Icon");
    m_listType.addItem(L"Report");
    m_listType.addItem(L"SmallIcon");
    m_listType.addItem(L"List");
    m_listType.setSelectedItem(0);

    // Bindings
    m_browseButton.Click.connect(Bind(&MainFrame::onBrowse, this));
    m_listType.SelChange.connect(Bind(&MainFrame::onListTypeChange, this));

    refresh();
  }

protected:

  void onBrowse()
  {
    OpenFileDialog dlg(L"Browse to collect more icons...", this);
    dlg.doModal();

    refresh();
  }

  void onListTypeChange()
  {
    int item = m_listType.getSelectedItem();
    ListViewType type = m_listView.getType();
    switch (item) {
      case 0: type = ListViewType::Icon; break;
      case 1: type = ListViewType::Report; break;
      case 2: type = ListViewType::SmallIcon; break;
      case 3: type = ListViewType::List; break;
    }
    m_listView.setType(type);
  }

  void refresh()
  {
    m_listView.removeAllItems();

    // Add one item for each image in the system image-list
    int i, count = m_listView.getImageList().getImageCount();
    for (i = 0; i < count; ++i)
      m_listView.addItem(new ListItem(format_string(L"Image #%d", i), i));
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
