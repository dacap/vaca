// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>

#include "../resource.h"

using namespace vaca;

static LPMALLOC g_pMalloc = NULL;
static IShellFolder* g_pDesktop = NULL;

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
      g_pMalloc->Free(m_relativePidl);
    g_pMalloc->Free(m_pidl);
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
      pFolder = g_pDesktop;
    else
      g_pDesktop->BindToObject(m_pidl, NULL, IID_IShellFolder, (LPVOID *)&pFolder);

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

      if (pFolder != g_pDesktop)
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
      pFolder = g_pDesktop;
    else
      g_pDesktop->BindToObject(dynamic_cast<FileTreeNode *>(getParent())->m_pidl,
				      NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      if (pFolder->GetDisplayNameOf(m_relativePidl, SHGDN_INFOLDER, &strDispName) != S_OK) {
	if (g_pDesktop->GetDisplayNameOf(m_pidl, SHGDN_INFOLDER, &strDispName) != S_OK) {
	  assert(false);
	}
      }

      if (pFolder != g_pDesktop)
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
      pFolder = g_pDesktop;
    else
      g_pDesktop->BindToObject(dynamic_cast<FileTreeNode *>(getParent())->m_pidl,
				      NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&m_relativePidl, &attr);

      if (pFolder != g_pDesktop)
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

    LPITEMIDLIST pidlNew = (LPITEMIDLIST)g_pMalloc->Alloc(cb1 + cb2);
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
  bool m_isLoading;

public:

  MainFrame()
    : Frame(L"MiniExplorer")
    , m_treeView(this, TreeView::Styles::Default
		       - TreeView::Styles::EditLabel
		       + TreeView::Styles::NoDragAndDrop)
    , m_isLoading(false)
  {
    // the m_treeView will use all the client area
    setLayout(new ClientLayout);

    // set the small system image list for the TreeView
    m_treeView.setImageList(System::getSmallImageList());

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
    m_isLoading = true;
  }

  void endLoading()
  {
    m_isLoading = false;
  }

  virtual void onSetCursor(SetCursorEvent& ev)
  {
    if (!ev.isConsumed() && m_isLoading) {
      ev.setCursor(Cursor(SysCursor::Wait));
    }
    Widget::onSetCursor(ev);
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
    SHGetMalloc(&g_pMalloc);

    // get desktop IShellFolder interface
    SHGetDesktopFolder(&g_pDesktop);

    // create MainFrame
    m_mainFrame = new MainFrame;
    m_mainFrame->setIcon(ResourceId(IDI_VACA));
  }

  virtual ~Example()
  {
    // destroy MainFrame
    delete m_mainFrame;

    // relase desktop IShellFolder interface
    g_pDesktop->Release();

    // release IMalloc interface
    g_pMalloc->Release();
    g_pMalloc = NULL;

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
