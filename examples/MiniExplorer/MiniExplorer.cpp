// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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
#include <shlobj.h>
#include <shlwapi.h>

using namespace Vaca;

namespace globals {
  LPMALLOC pMalloc = NULL;
  IShellFolder* pDesktop = NULL;
}

//////////////////////////////////////////////////////////////////////

class FileTreeNode : public TreeNode
{
  enum Subfolders { UnknownSubfolders,
		    WithSubfolders,
		    WithoutSubfolders };
  LPITEMIDLIST m_pidl;
  LPITEMIDLIST m_relativePidl;
  Subfolders m_hasSubfolders;

public:

  FileTreeNode(LPITEMIDLIST pidl, LPITEMIDLIST pidlParent)
  {
    // the PIDL
    if (pidlParent != NULL) {
      m_pidl = concatPidl(pidlParent, pidl);
      m_relativePidl = pidl;
    }
    else {
      m_pidl = pidl;
      m_relativePidl = pidl;
    }

    // setup text
    setText(getDisplayNameByPidl());

    // setup icon
    SHFILEINFO shfi;
    ZeroMemory(reinterpret_cast<PVOID>(&shfi), sizeof(shfi));
    SHGetFileInfo(reinterpret_cast<LPTSTR>(m_pidl), 0,
		  &shfi, sizeof(shfi),
		  SHGFI_PIDL |
		  SHGFI_SYSICONINDEX |
		  SHGFI_SMALLICON);
    setImage(shfi.iIcon);

    // sub-folders
    m_hasSubfolders = UnknownSubfolders;
  }

  virtual ~FileTreeNode()
  {
    // destroy PIDLs using IMalloc...
    if (m_relativePidl != m_pidl)
      globals::pMalloc->Free(m_relativePidl);
    globals::pMalloc->Free(m_pidl);
  }

  virtual bool hasChildren()
  {
    // calculate sub-folders?
    if (m_hasSubfolders == UnknownSubfolders)
      m_hasSubfolders = findSubFolders() ? WithSubfolders:
					   WithoutSubfolders;

    return m_hasSubfolders == WithSubfolders;
  }

protected:
  
  virtual void onBeforeExpand(TreeViewEvent &ev)
  {
    IShellFolder* pFolder = NULL;

    // it's the desktop
    if (this->getParent() == NULL)
      pFolder = globals::pDesktop;
    else
      globals::pDesktop->BindToObject(m_pidl, NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      if (getChildren().empty()) {
	IEnumIDList* pEnum = NULL;
	LPITEMIDLIST itemPidl = NULL;
	ULONG fetched;

	m_hasSubfolders = WithoutSubfolders;	// right now, it hasn't sub-folders

	pFolder->EnumObjects(NULL, SHCONTF_FOLDERS, &pEnum);

	if (pEnum != NULL) {
	  while (pEnum->Next(1, &itemPidl, &fetched) == S_OK && fetched == 1) {
	    this->addNode(new FileTreeNode(itemPidl, m_pidl));

	    if (m_hasSubfolders != WithSubfolders)
	      m_hasSubfolders = WithSubfolders; // well, now it has sub-folders
	  }

	  pEnum->Release();
	}
      }

      if (pFolder != globals::pDesktop)
	pFolder->Release();
    }
    else {
      m_hasSubfolders = WithoutSubfolders;
    }
  }

private:

  // returns the human readable name of the location
  // indicated by the PIDL
  String getDisplayNameByPidl()
  {
    STRRET strDispName;
    TCHAR pszDisplayName[MAX_PATH];
    IShellFolder* pFolder = NULL;

    if (this->getParent() == NULL)
      pFolder = globals::pDesktop;
    else
      globals::pDesktop->BindToObject(dynamic_cast<FileTreeNode *>(getParent())->m_pidl,
				      NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      if (pFolder->GetDisplayNameOf(m_relativePidl, SHGDN_INFOLDER, &strDispName) != S_OK) {
	if (globals::pDesktop->GetDisplayNameOf(m_pidl, SHGDN_INFOLDER, &strDispName) != S_OK) {
	  assert(false);
	}
      }

      if (pFolder != globals::pDesktop)
	pFolder->Release();
    }
    
    StrRetToBuf(&strDispName, m_pidl, pszDisplayName, MAX_PATH);
    return pszDisplayName;
  }

  bool findSubFolders()
  {
    ULONG attr = SFGAO_HASSUBFOLDER | SFGAO_VALIDATE;
    IShellFolder *pFolder = NULL;

    if (this->getParent() == NULL)
      pFolder = globals::pDesktop;
    else
      globals::pDesktop->BindToObject(dynamic_cast<FileTreeNode *>(getParent())->m_pidl,
				      NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&m_relativePidl, &attr);

      if (pFolder != globals::pDesktop)
	pFolder->Release();
    }

    return (attr & SFGAO_HASSUBFOLDER) != 0;
  }
  
  LPITEMIDLIST concatPidl(LPITEMIDLIST pidlHead, LPITEMIDLIST pidlTail)
  {
    assert(pidlHead != NULL);
    assert(pidlTail != NULL);

    UINT cb1 = getPidlSize(pidlHead) - sizeof(pidlHead->mkid.cb);
    UINT cb2 = getPidlSize(pidlTail);

    LPITEMIDLIST pidlNew = (LPITEMIDLIST)globals::pMalloc->Alloc(cb1 + cb2);
    if (pidlNew) {
      CopyMemory(pidlNew, pidlHead, cb1);
      CopyMemory(((LPSTR)pidlNew) + cb1, pidlTail, cb2);
    }
    return pidlNew;
  }

  UINT getPidlSize(LPITEMIDLIST pidl)
  {
    UINT cbTotal = 0;
    if (pidl) {
      cbTotal += sizeof(pidl->mkid.cb);    // Null terminator
      while (pidl) {
	cbTotal += pidl->mkid.cb;
	pidl = getNextPidl(pidl);
      }
    }
    return cbTotal;
  }

  LPITEMIDLIST getNextPidl(LPITEMIDLIST pidl)
  {
    if (pidl != NULL && pidl->mkid.cb > 0) {
      pidl = (LPITEMIDLIST)(((LPBYTE)(pidl)) + pidl->mkid.cb);
      if (pidl->mkid.cb > 0)
	return pidl;
    }
    return NULL;
  } 

};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  TreeView m_treeView;

public:

  MainFrame()
    : Frame(L"MiniExplorer")
    , m_treeView(this, TreeView::Styles::Default
		       - TreeView::Styles::EditLabel)
  {
    // the m_treeView will use all the client area
    setLayout(new ClientLayout);

    // set the small system image list for the TreeView
    m_treeView.setNormalImageList(System::getImageList(true));

    // get the desktop PIDL
    LPITEMIDLIST pidl = NULL;
    SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, 0, &pidl);

    // add the root node (for the desktop)
    m_treeView.addNode(new FileTreeNode(pidl, NULL));

    m_treeView.BeforeExpand.connect(Bind(&MainFrame::startLoading, this));
    m_treeView.AfterExpand.connect(Bind(&MainFrame::endLoading, this));
  }

private:

  void startLoading()
  {
    setCursor(Cursor(SysCursor::Wait));
  }

  void endLoading()
  {
    setCursor(Cursor(SysCursor::Arrow));
  }
    

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame *m_mainFrame;

public:

  Example()
  {
    // the Application constructor calls CoInitialize...

    // get IMalloc interface
    SHGetMalloc(&globals::pMalloc);

    // get desktop IShellFolder interface
    SHGetDesktopFolder(&globals::pDesktop);

    // create MainFrame
    m_mainFrame = new MainFrame;
  }

  virtual ~Example()
  {
    // destroy MainFrame
    delete m_mainFrame;

    // relase desktop IShellFolder interface
    globals::pDesktop->Release();
    
    // release IMalloc interface
    globals::pMalloc->Release();
    globals::pMalloc = NULL;

    // the Application destructor calls CoUninitialize...
  }

private:

  virtual void main()
  {
    m_mainFrame->setVisible(true);
  }
  
};

int VACA_MAIN()
{
  Example app;
  app.run();
  return 0;
}
