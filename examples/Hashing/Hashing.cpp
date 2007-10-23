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

#include <Vaca/Vaca.h>

#include "md5.h"
#include "sha1.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// MD5

static String MD5File(const String &fileName)
{
  FILE *stream = _tfopen(fileName.c_str(), _T("rb"));
  if (stream == NULL)
    return String("Error loading file");

  MD5_CTX md5;
  MD5Init(&md5);

  unsigned char buf[1024];
  size_t len;
  while (len = fread(buf, 1, 1024, stream))
    MD5Update(&md5, buf, len);

  fclose(stream);

  unsigned char digest[16];
  MD5Final(digest, &md5);

  // transform "digest" to String
  String res;
  for(int c=0; c<16; ++c)
    res += String::fromInt(digest[c], 16, 2);
  return res;
}

//////////////////////////////////////////////////////////////////////
// SHA1

static String SHA1File(const String &fileName)
{
  FILE *stream = _tfopen(fileName.c_str(), _T("rb"));
  if (stream == NULL)
    return String("Error loading file");

  SHA1Context sha;
  SHA1Reset(&sha);

  unsigned char buf[1024];
  size_t len;
  while (len = fread(buf, 1, 1024, stream))
    SHA1Input(&sha, buf, len);

  fclose(stream);

  uint8_t digest[20];
  SHA1Result(&sha, digest);

  // transform "digest" to String
  String res;
  for(int c=0; c<20; ++c)
    res += String::fromInt(digest[c], 16, 2);
  return res;
}

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Label m_helpLabel;
  ListView m_filesList;
  Edit m_md5Edit;
  Edit m_shaEdit;
  LinkLabel m_md5Label;
  LinkLabel m_shaLabel;
  ImageList m_imageList;

public:

  MainFrame()
    : Frame("Hashing")
    , m_helpLabel("Drop files to the list", this)
    , m_filesList(this, ListViewStyle + AcceptFilesStyle + SingleSelectionListViewStyle)
    , m_md5Edit("", this, EditStyle + ReadOnlyEditStyle)
    , m_shaEdit("", this, EditStyle + ReadOnlyEditStyle)
    , m_md5Label("http://www.faqs.org/rfcs/rfc1321.html", "RFC 1321 - The MD5 Message-Digest Algorithm", this)
    , m_shaLabel("http://www.faqs.org/rfcs/rfc3174.html", "RFC 3174 - US Secure Hash Algorithm 1 (SHA1)", this)
  {
    setLayout(Bix::parse("Y[%,f%,XY[%,fx%;%,fx%]]",
			 &m_helpLabel,
			 &m_filesList,
			 &m_md5Label, &m_md5Edit,
			 &m_shaLabel, &m_shaEdit));

    // setup report view
    m_filesList.setType(ListViewType::Report);
    m_filesList.addColumn("Filename");
    m_filesList.addColumn("MD5");
    m_filesList.addColumn("SHA1");

    // signals
    m_filesList.DropFiles.connect(Bind(&MainFrame::onDropFilesInFilesList, this));
    m_filesList.AfterSelect.connect(Bind(&MainFrame::onSelectFileInList, this));

    // image list (get the small image list from the system)
    System::getImageList(m_imageList, true);
    m_filesList.setSmallImageList(&m_imageList);
  }

private:

  void onDropFilesInFilesList(DropFilesEvent &ev)
  {
    std::vector<String> files = ev.getFiles();
    
    // m_filesList.removeAllItems();

    // add items in the list
    for (std::vector<String>::iterator
	   it = files.begin(); it != files.end(); ++it) {
      // get the what image to use
      int imageIndex =
	System::getFileImageIndex((*it), true);

      // add the new item and hold its index
      int itemIndex =
	m_filesList.addItem((*it).getFileName(), imageIndex);

      // calculates the MD5 and SHA1 of the file
      m_filesList.setItemText(itemIndex, MD5File(*it), 1);
      m_filesList.setItemText(itemIndex, SHA1File(*it), 2);
    }

    // set preferred with for each column
    int n = m_filesList.getColumnCount();
    for (int i=0; i<n; ++i)
      m_filesList.setPreferredColumnWidth(i, true);
  }

  void onSelectFileInList(ListViewEvent &ev)
  {
    int itemIndex = ev.getItemIndex();
    if (itemIndex >= 0) {
      m_md5Edit.setText(m_filesList.getItemText(itemIndex, 1));
      m_shaEdit.setText(m_filesList.getItemText(itemIndex, 2));
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
