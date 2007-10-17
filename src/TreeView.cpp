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
#include "Vaca/TreeView.h"
#include "Vaca/TreeViewEvent.h"
#include "Vaca/System.h"
#include "Vaca/Debug.h"

using namespace Vaca;

TreeView::TreeView(Widget *parent, Style style)
  : Widget(WC_TREEVIEW, parent, style)
{
  mRoot.mTreeView = this;

//   Widget::setBgColor(Color(TreeView_GetBkColor(getHwnd())));
  setBgColor(System::getColor(COLOR_WINDOW));
}

void TreeView::addNode(TreeNode *node)
{
  mRoot.addNode(node);
}

TreeNode *TreeView::getSelectedNode()
{
  VACA_ASSERT(getHwnd() != NULL);

  HTREEITEM htreeitem = TreeView_GetSelection(getHwnd());
  if (htreeitem != NULL)
    return TreeNode::fromHtreeitem(getHwnd(), htreeitem);
  else
    return NULL;
}

void TreeView::setSelectedNode(TreeNode *node)
{
  VACA_ASSERT(getHwnd() != NULL);

  TreeView_SelectItem(getHwnd(), node->getHtreeitem());
}

void TreeView::setBgColor(Color color)
{
  VACA_ASSERT(getHwnd() != NULL);

  Widget::setBgColor(color);
  TreeView_SetBkColor(getHwnd(), color.getColorRef());
}

/**
 * You can cancel this event (Event::cancel).
 */
void TreeView::onBeforeExpand(TreeViewEvent &ev)
{
  ev.getTreeNode()->onBeforeExpand(ev);
  BeforeExpand(ev);
}

/**
 * You can cancel this event (Event::cancel).
 */
void TreeView::onBeforeCollapse(TreeViewEvent &ev)
{
  ev.getTreeNode()->onBeforeCollapse(ev);
  BeforeCollapse(ev);
}

void TreeView::onBeforeSelect(TreeViewEvent &ev)
{
  ev.getTreeNode()->onBeforeSelect(ev);
  BeforeSelect(ev);
}

void TreeView::onAfterExpand(TreeViewEvent &ev)
{
  ev.getTreeNode()->onAfterExpand(ev);
  AfterExpand(ev);
}

void TreeView::onAfterCollapse(TreeViewEvent &ev)
{
  ev.getTreeNode()->onAfterCollapse(ev);
  AfterCollapse(ev);
}

void TreeView::onAfterSelect(TreeViewEvent &ev)
{
  ev.getTreeNode()->onAfterSelect(ev);
  AfterSelect(ev);
}

bool TreeView::onNotify(LPNMHDR lpnmhdr, LRESULT &lResult)
{
  switch (lpnmhdr->code) {

    case TVN_GETDISPINFO: {
      LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

      if (lptvdi->item.mask & TVIF_TEXT) {
	String text = node->getText();
	if (!text.empty()) {
	  // TODO threads!!!
	  static LPTSTR buf = NULL;
	  if (buf != NULL)
	    delete buf;
	  int length = text.size()+1;
	  buf = new Character[length];
	  text.copyTo(buf, length);
	  lptvdi->item.pszText = reinterpret_cast<LPTSTR>(buf);
	}
	else
	  lptvdi->item.pszText = reinterpret_cast<LPTSTR>(NULL);
      }

      if (lptvdi->item.mask & TVIF_IMAGE)
	lptvdi->item.iImage = node->getImage();

      if (lptvdi->item.mask & TVIF_SELECTEDIMAGE)
	lptvdi->item.iSelectedImage = node->getSelectedImage();

      if (lptvdi->item.mask & TVIF_CHILDREN)
	lptvdi->item.cChildren = node->hasChildren();

      return true;
    }

      //////////////////////////////////////////////////////////////////////
      // TVN_ITEMEXPANDING

    case TVN_ITEMEXPANDING: {
      LPNMTREEVIEW lppnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lppnmtv->itemNew.lParam);

      if (lppnmtv->action == TVE_EXPAND) {
	TreeViewEvent ev(this, node);
	onBeforeExpand(ev);
	// prevent expand
	if (ev.isCanceled()) {
	  lResult = TRUE;
	  return true;
	}
      }
      else if (lppnmtv->action == TVE_COLLAPSE) {
	TreeViewEvent ev(this, node);
	onBeforeCollapse(ev);
	// prevent collapse
	if (ev.isCanceled()) {
	  lResult = TRUE;
	  return true;
	}
      }

      return false;
    }

      //////////////////////////////////////////////////////////////////////
      // TVN_SELCHANGING

    case TVN_SELCHANGING: {
      LPNMTREEVIEW lppnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lppnmtv->itemNew.lParam);

      TreeViewEvent ev(this, node);
      onBeforeSelect(ev);
      // prevent expand
      if (ev.isCanceled()) {
	lResult = TRUE;
	return true;
      }

      return false;
    }

      //////////////////////////////////////////////////////////////////////
      // TVN_ITEMEXPANDED

    case TVN_ITEMEXPANDED: {
      LPNMTREEVIEW lppnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lppnmtv->itemNew.lParam);

      if (lppnmtv->action == TVE_EXPAND) {
	TreeViewEvent ev(this, node);
	onAfterExpand(ev);
      }
      else if (lppnmtv->action == TVE_COLLAPSE) {
	TreeViewEvent ev(this, node);
	onAfterCollapse(ev);
      }

      lResult = 0;
      return true;
    }

      //////////////////////////////////////////////////////////////////////
      // TVN_SELCHANGED

    case TVN_SELCHANGED: {
      LPNMTREEVIEW lppnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lppnmtv->itemNew.lParam);

      TreeViewEvent ev(this, node);
      onAfterSelect(ev);

      lResult = 0;
      return true;
    }

//     case TVN_BEGINLABELEDIT: {
//       LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
//       TreeNode *item = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

// //       ItemEvent(tree->evBeginLabelEdit, *ctrl, *item).fire();
//       return true;
//     }

//     case TVN_ENDLABELEDIT: {
//       LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
//       TreeNode *item = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

// //       ItemEvent(tree->evEndLabelEdit, *ctrl, *item).fire();
//       return true;
//     }

//     case TVN_BEGINDRAG: {
//       LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
//       TreeNode *node = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

// //       ItemEvent(tree->evBeginDrag, *ctrl, *node).fire();
//       return true;
//     }

  }

  return false;
}
