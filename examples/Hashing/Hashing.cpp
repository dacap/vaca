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

#include "md5.h"
#include "sha1.h"

#include <cstdio>

using namespace std;
using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// MD5

static String MD5File(const String& fileName)
{
  FILE* stream = _wfopen(fileName.c_str(), L"rb");
  if (stream == NULL)
    return String(L"Error loading file");

  MD5_CTX md5;
  MD5Init(&md5);

  unsigned char buf[1024];
  size_t len;
  while ((len = fread(buf, 1, 1024, stream)))
    MD5Update(&md5, buf, len);

  fclose(stream);

  unsigned char digest[16];
  MD5Final(digest, &md5);

  // transform "digest" to String
  String res;
  for(int c=0; c<16; ++c)
    res += format_string(L"%02x", digest[c]);
  return res;
}

//////////////////////////////////////////////////////////////////////
// SHA1

static String SHA1File(const String &fileName)
{
  FILE* stream = _wfopen(fileName.c_str(), L"rb");
  if (stream == NULL)
    return String(L"Error loading file");

  SHA1Context sha;
  SHA1Reset(&sha);

  unsigned char buf[1024];
  size_t len;
  while ((len = fread(buf, 1, 1024, stream)))
    SHA1Input(&sha, buf, len);

  fclose(stream);

  uint8_t digest[20];
  SHA1Result(&sha, digest);

  // transform "digest" to String
  String res;
  for(int c=0; c<20; ++c)
    res += format_string(L"%02x", digest[c]);
  return res;
}

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Label m_helpLabel;
  ListView m_filesList;
  TextEdit m_md5Edit;
  TextEdit m_shaEdit;
  LinkLabel m_md5Link;
  LinkLabel m_shaLink;

public:

  MainFrame()
    : Frame(L"Hashing")
    , m_helpLabel(L"Drop files to the list", this)
    , m_filesList(this, ListView::Styles::Default +
			ListView::Styles::SingleSelection +
			Widget::Styles::AcceptFiles)
    , m_md5Edit(L"", this, TextEdit::Styles::Default +
			   TextEdit::Styles::ReadOnly)
    , m_shaEdit(L"", this, TextEdit::Styles::Default +
			   TextEdit::Styles::ReadOnly)
    , m_md5Link(L"http://www.faqs.org/rfcs/rfc1321.html",
		L"RFC 1321 - The MD5 Message-Digest Algorithm", this)
    , m_shaLink(L"http://www.faqs.org/rfcs/rfc3174.html",
		L"RFC 3174 - US Secure Hash Algorithm 1 (SHA1)", this)
  {
    setLayout(Bix::parse(L"Y[%,f%,XY[%,fx%;%,fx%]]",
			 &m_helpLabel,
			 &m_filesList,
			 &m_md5Link, &m_md5Edit,
			 &m_shaLink, &m_shaEdit));

    // setup report view
    m_filesList.setType(ListViewType::Report);
    m_filesList.addColumn(new ListColumn(L"Filename"));
    m_filesList.addColumn(new ListColumn(L"MD5"));
    m_filesList.addColumn(new ListColumn(L"SHA1"));

    // signals
    m_filesList.DropFiles.connect(&MainFrame::onDropFilesInFilesList, this);
    m_filesList.AfterSelect.connect(&MainFrame::onSelectFileInList, this);

    // get the small image list from the system and put it in the ListView
    m_filesList.setSmallImageList(System::getSmallImageList());
  }

private:

  void onDropFilesInFilesList(DropFilesEvent& ev)
  {
    vector<String> files = ev.getFiles();

    // Add items in the list
    for (vector<String>::iterator
	   it = files.begin(); it != files.end(); ++it) {
      // Get the what image to use
      int imageIndex = System::getFileImageIndex((*it), true);

      // Create the new item
      ListItem* item = new ListItem(file_name(*it), imageIndex);

      // Calculates the MD5 and SHA1 of the file
      item->setText(MD5File(*it), 1);
      item->setText(SHA1File(*it), 2);

      // Add the item in the ListView
      m_filesList.addItem(item);
    }

    // set preferred with for each column
    int n = m_filesList.getColumnCount();
    for (int i=0; i<n; ++i)
      m_filesList.getColumn(i)->setPreferredWidth(true);
  }

  void onSelectFileInList(ListViewEvent& ev)
  {
    if (ListItem* item = ev.getItem()) {
      m_md5Edit.setText(item->getText(1));
      m_shaEdit.setText(item->getText(2));
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
