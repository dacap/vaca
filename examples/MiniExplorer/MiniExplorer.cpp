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

#include "Vaca/Vaca.h"
#include <boost/logic/tribool.hpp>

#include <shlobj.h>
#include <shlwapi.h>

using namespace Vaca;

ImageList mImageList;

LPMALLOC pMalloc = NULL;
IShellFolder *pDesktop = NULL;

//////////////////////////////////////////////////////////////////////

class FileTreeNode : public TreeNode
{
  LPITEMIDLIST mPidl, mRelativePidl;
  boost::tribool mHasSubFolders;
  
public:

  FileTreeNode(LPITEMIDLIST pidl, LPITEMIDLIST pidlParent)
  {
    // the PIDL
    if (pidlParent != NULL) {
      mPidl = concatPidl(pidlParent, pidl);
      mRelativePidl = pidl;
    }
    else {
      mPidl = pidl;
      mRelativePidl = pidl;
    }

    // setup text
    setText(getDisplayNameByPidl());

    // setup icon
    SHFILEINFO shfi;
    ZeroMemory(reinterpret_cast<PVOID>(&shfi), sizeof(shfi));
    SHGetFileInfo(reinterpret_cast<LPTSTR>(mPidl), 0,
		  &shfi, sizeof(shfi),
		  SHGFI_PIDL |
		  SHGFI_SYSICONINDEX |
		  SHGFI_SMALLICON);
    setImage(shfi.iIcon);

    // sub-folders
    mHasSubFolders = boost::indeterminate;
  }

  virtual ~FileTreeNode()
  {
    // destroy PIDLs using IMalloc...
    if (mRelativePidl != mPidl)
      pMalloc->Free(mRelativePidl);
    pMalloc->Free(mPidl);
  }

  virtual bool hasChildren()
  {
    // calculate sub-folders?
    if (boost::indeterminate(mHasSubFolders))
      mHasSubFolders = findSubFolders();

    return mHasSubFolders;
  }

protected:
  
  virtual void onBeforeExpand(TreeViewEvent &ev)
  {
    IShellFolder *pFolder = NULL;

    // it's the desktop
    if (this->getParent() == NULL)
      pFolder = pDesktop;
    else
      pDesktop->BindToObject(mPidl, NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      if (getChildren().empty()) {
	IEnumIDList *pEnum = NULL;
	LPITEMIDLIST itemPidl = NULL;
	ULONG fetched;

	mHasSubFolders = false;	// right now, it hasn't sub-folders

	pFolder->EnumObjects(NULL, SHCONTF_FOLDERS, &pEnum);

	if (pEnum != NULL) {
	  while (pEnum->Next(1, &itemPidl, &fetched) == S_OK && fetched == 1) {
	    this->addNode(new FileTreeNode(itemPidl, mPidl));

	    if (mHasSubFolders != true)
	      mHasSubFolders = true; // well, now it has sub-folders
	  }

	  pEnum->Release();
	}
      }

      if (pFolder != pDesktop)
	pFolder->Release();
    }
    else
      mHasSubFolders = false;
  }

private:

  // returns the human readable name of the location
  // indicated by the PIDL
  String getDisplayNameByPidl()
  {
    STRRET strDispName;
    TCHAR pszDisplayName[MAX_PATH];
    IShellFolder *pFolder = NULL;

    if (this->getParent() == NULL)
      pFolder = pDesktop;
    else
      pDesktop->BindToObject(dynamic_cast<FileTreeNode *>(getParent())->mPidl,
			     NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      if (pFolder->GetDisplayNameOf(mRelativePidl, SHGDN_INFOLDER, &strDispName) != S_OK) {
	if (pDesktop->GetDisplayNameOf(mPidl, SHGDN_INFOLDER, &strDispName) != S_OK) {
	  assert(false);
	}
      }

      if (pFolder != pDesktop)
	pFolder->Release();
    }
    
    StrRetToBuf(&strDispName, mPidl, pszDisplayName, MAX_PATH);
    return pszDisplayName;
  }

  bool findSubFolders()
  {
    ULONG attr = SFGAO_HASSUBFOLDER | SFGAO_VALIDATE;
    IShellFolder *pFolder = NULL;

    if (this->getParent() == NULL)
      pFolder = pDesktop;
    else
      pDesktop->BindToObject(dynamic_cast<FileTreeNode *>(getParent())->mPidl,
			     NULL, IID_IShellFolder, (LPVOID *)&pFolder);

    if (pFolder != NULL) {
      pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&mRelativePidl, &attr);

      if (pFolder != pDesktop)
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

    LPITEMIDLIST pidlNew = (LPITEMIDLIST)pMalloc->Alloc(cb1 + cb2);
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
  TreeView mTreeView;

public:

  MainFrame()
    : Frame("MiniExplorer")
    , mTreeView(this, TreeViewStyle - EditLabelTreeViewStyle)
  {
    // the mTreeView will use all the client area
    setLayout(new ClientLayout);

    // set the small system image list for the TreeView
    System::getImageList(mImageList, true);
    mTreeView.setNormalImageList(mImageList);

    // get the desktop PIDL
    LPITEMIDLIST pidl = NULL;
    SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, 0, &pidl);

    // add the root node (for the desktop)
    mTreeView.addNode(new FileTreeNode(pidl, NULL// , pDesktop, NULL
				       ));
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame *mMainWnd;

public:

  Example()
  {
    // the Application constructor calls CoInitialize...

    // get IMalloc interface
    SHGetMalloc(&pMalloc);

    // get desktop IShellFolder interface
    SHGetDesktopFolder(&pDesktop);

    // create MainFrame
    mMainWnd = new MainFrame;
  }

  virtual ~Example()
  {
    // destroy MainFrame
    delete mMainWnd;

    // relase desktop IShellFolder interface
    pDesktop->Release();
    
    // release IMalloc interface
    pMalloc->Release();
    pMalloc = NULL;

    // the Application destructor calls CoUninitialize...
  }

  virtual void main(std::vector<String> args)
  {
    mMainWnd->setVisible(true);
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
