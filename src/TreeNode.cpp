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

#include "stdvaca.h"
#include "Vaca/TreeNode.h"
#include "Vaca/TreeView.h"
#include "Vaca/Debug.h"

using namespace Vaca;

TreeNode::TreeNode()
{
  mText = "";
  mParent = NULL;
  mHtreeitem = NULL;
  mTreeView = NULL;
}

TreeNode::TreeNode(const String &text)
{
  mText = text;
  mParent = NULL;
  mHtreeitem = NULL;
  mTreeView = NULL;
}

TreeNode::~TreeNode()
{
  if (mTreeView)
    TreeView_DeleteItem(mTreeView->getHwnd(), mHtreeitem);

  for (Container::iterator it=mChildren.begin();
       it!=mChildren.end(); ++it)
    delete *it;
}

void TreeNode::addNode(TreeNode *node)
{
  VACA_ASSERT(node->mHtreeitem == NULL && node->mParent == NULL);

  this->mChildren.push_back(node);

  node->mParent = this;
  node->addToTreeView(this->mTreeView);
}

TreeNode *TreeNode::getParent()
{
  return mParent;
}

TreeView *TreeNode::getTreeView()
{
  return mTreeView;
}

TreeNode::Container TreeNode::getChildren()
{
  return mChildren;
}

bool TreeNode::hasChildren()
{
  return !mChildren.empty();
}

String TreeNode::getText()
{
  return mText;
}

int TreeNode::getImage()
{
  return -1;
}

int TreeNode::getSelectedImage()
{
  return -1;
}

HTREEITEM TreeNode::getHtreeitem()
{
  return mHtreeitem;
}

TreeNode *TreeNode::fromHtreeitem(HWND hwnd, HTREEITEM htreeitem)
{
  TVITEMEX tvie;

  ZeroMemory(&tvie, sizeof(TVITEMEX));

  tvie.mask = TVIF_PARAM;
  tvie.hItem = htreeitem;

  TreeView_GetItem(hwnd, &tvie);

  return reinterpret_cast<TreeNode *>(tvie.lParam);
}

void TreeNode::onBeforeExpand(TreeViewEvent &ev)
{
}

void TreeNode::onBeforeCollapse(TreeViewEvent &ev)
{
}

void TreeNode::onBeforeSelect(TreeViewEvent &ev)
{
}

void TreeNode::onAfterExpand(TreeViewEvent &ev)
{
}

void TreeNode::onAfterCollapse(TreeViewEvent &ev)
{
}

void TreeNode::onAfterSelect(TreeViewEvent &ev)
{
}

void TreeNode::addToTreeView(TreeView *treeview)
{
  VACA_ASSERT(mTreeView == NULL);

  mTreeView = treeview;
  if (treeview) {
    TVINSERTSTRUCT is;

    is.hParent = mParent->mHtreeitem;
    is.hInsertAfter = TVI_LAST;
    is.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    is.item.pszText = LPSTR_TEXTCALLBACK;
    is.item.iImage = I_IMAGECALLBACK;
    is.item.iSelectedImage = I_IMAGECALLBACK;
    is.item.cChildren = I_CHILDRENCALLBACK;
    is.item.lParam = reinterpret_cast<LPARAM>(this);

    mHtreeitem = TreeView_InsertItem(treeview->getHwnd(), &is);
  }

  for (Container::iterator it=mChildren.begin();
       it!=mChildren.end(); ++it) {
    if ((*it)->mTreeView == NULL)
      (*it)->addToTreeView(treeview);
  }
}
