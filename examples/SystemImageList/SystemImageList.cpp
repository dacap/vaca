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
  ListView m_listView;
  ImageList m_imageList;
  Panel m_bottom;
  Button m_browseButton;
  Button m_refreshButton;

public:

  MainFrame()
    : Frame("System_imageList")
    , m_listView(this)
    , m_bottom(this)
    , m_browseButton("Browse to get more icons...", &m_bottom)
    , m_refreshButton("Refresh list", &m_bottom)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_listView.setConstraint(new BoxConstraint(true));
    m_bottom.setLayout(new BoxLayout(Orientation::Horizontal, true));

    // get the system image-list
    System::getImageList(m_imageList, false);

    // setup the ListView
    m_listView.setType(ListViewType::Icon);
    m_listView.setNormalImageList(&m_imageList);

    // bindings
    m_browseButton.Action.connect(Bind(&MainFrame::onBrowse, this));
    m_refreshButton.Action.connect(Bind(&MainFrame::onRefresh, this));

    // first refresh
    onRefresh();
  }

protected:

  void onBrowse()
  {
    OpenFileDialog dlg("Browse to get more icons...", this);
    dlg.doModal();
  }

  void onRefresh()
  {
    m_listView.removeAllItems();

    // add one item for each image in the system image-list
    int i, count = m_imageList.getImageCount();
    for (i = 0; i < count; ++i) {
      m_listView.addItem("Image #"+String::fromInt(i), i);
    }
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
