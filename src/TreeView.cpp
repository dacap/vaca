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
#include "Vaca/ImageList.h"

using namespace Vaca;

TreeView::TreeView(Widget *parent, Style style)
  : Widget(WC_TREEVIEW, parent, style)
{
  mRoot.mOwner = this;

//   Widget::setBgColor(Color(TreeView_GetBkColor(getHWND())));
  setBgColor(System::getColor(COLOR_WINDOW));
}

TreeView::~TreeView()
{
}

void TreeView::setImageList(ImageList &imageList, int type)
{
  assert(getHWND() != NULL);
  assert(imageList.isValid());

  TreeView_SetImageList(getHWND(), imageList.getHIMAGELIST(), type);
}

void TreeView::setNormalImageList(ImageList &imageList)
{
  setImageList(imageList, LVSIL_NORMAL);
}

void TreeView::setStateImageList(ImageList &imageList)
{
  setImageList(imageList, LVSIL_STATE);
}

void TreeView::addNode(TreeNode *node)
{
  mRoot.addNode(node);
}

TreeNode *TreeView::getSelectedNode()
{
  assert(getHWND() != NULL);

  HTREEITEM htreeitem = TreeView_GetSelection(getHWND());
  if (htreeitem != NULL)
    return TreeNode::fromHTREEITEM(getHWND(), htreeitem);
  else
    return NULL;
}

void TreeView::setSelectedNode(TreeNode *node)
{
  assert(getHWND() != NULL);

  TreeView_SelectItem(getHWND(), node->getHTREEITEM());
}

void TreeView::setBgColor(Color color)
{
  assert(getHWND() != NULL);

  Widget::setBgColor(color);
  TreeView_SetBkColor(getHWND(), color.getColorRef());
}

/**
 * You can cancel this event (Event::cancel).
 */
void TreeView::onBeforeExpand(TreeViewEvent &ev)
{
  BeforeExpand(ev);
  ev.getTreeNode()->onBeforeExpand(ev);
}

/**
 * You can cancel this event (Event::cancel).
 */
void TreeView::onBeforeCollapse(TreeViewEvent &ev)
{
  BeforeCollapse(ev);
  ev.getTreeNode()->onBeforeCollapse(ev);
}

void TreeView::onBeforeSelect(TreeViewEvent &ev)
{
  BeforeSelect(ev);
  ev.getTreeNode()->onBeforeSelect(ev);
}

/**
 * You can cancel this event to cancel the label editing.
 * 
 */
void TreeView::onBeforeLabelEdit(TreeViewEvent &ev)
{
  BeforeLabelEdit(ev);
  ev.getTreeNode()->onBeforeLabelEdit(ev);
}

void TreeView::onAfterExpand(TreeViewEvent &ev)
{
  AfterExpand(ev);
  ev.getTreeNode()->onAfterExpand(ev);
}

void TreeView::onAfterCollapse(TreeViewEvent &ev)
{
  AfterCollapse(ev);
  ev.getTreeNode()->onAfterCollapse(ev);
}

void TreeView::onAfterSelect(TreeViewEvent &ev)
{
  AfterSelect(ev);
  ev.getTreeNode()->onAfterSelect(ev);
}

/**
 * You can cancel this event to avoid to change the item text.
 * 
 */
void TreeView::onAfterLabelEdit(TreeViewEvent &ev)
{
  ev.getTreeNode()->onAfterLabelEdit(ev);
  AfterLabelEdit(ev);
}

bool TreeView::onNotify(LPNMHDR lpnmhdr, LRESULT &lResult)
{
  switch (lpnmhdr->code) {

    case TVN_GETDISPINFO: {
      LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

      if (lptvdi->item.mask & TVIF_TEXT) {
	mTmpBuffer = node->getText();
	lptvdi->item.pszText = const_cast<LPTSTR>(mTmpBuffer.c_str());
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

    case TVN_BEGINLABELEDIT: {
      LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

      TreeViewEvent ev(this, node);
      onBeforeLabelEdit(ev);

      // cancel editing
      lResult = ev.isCanceled() ? TRUE: FALSE; // TRUE cancels the editing
      return true;
    }

    case TVN_ENDLABELEDIT: {
      LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
      TreeNode *node = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

      TreeViewEvent ev(this, node,
		       lptvdi->item.pszText != NULL ? String(lptvdi->item.pszText):
						      String(""));

      // the label editing was canceled by the user?
      if (lptvdi->item.pszText == NULL)
	ev.cancel();

      onAfterLabelEdit(ev);

      // cancel label change (this hasn't effect, because we use LPSTR_TEXTCALLBACK)
      lResult = ev.isCanceled() ? FALSE: TRUE; // FALSE rejects the edited text
      return true;
    }

//     case TVN_BEGINDRAG: {
//       LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
//       TreeNode *node = reinterpret_cast<TreeNode *>(lptvdi->item.lParam);

// //       ItemEvent(tree->evBeginDrag, *ctrl, *node).fire();
//       return true;
//     }

  }

  return false;
}
