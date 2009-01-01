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

#include "Vaca/TreeView.h"
#include "Vaca/TreeViewEvent.h"
#include "Vaca/System.h"
#include "Vaca/Debug.h"
#include "Vaca/ImageList.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/MouseEvent.h"
#include "Vaca/Cursor.h"

#include "Vaca/Brush.h"
#include "Vaca/Pen.h"

// Recommended by the MSDN
#define M_AFTERDRAGANDDROP		(WM_APP+1)

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// TreeViewIterator


TreeViewIterator::TreeViewIterator()
{
  m_currentNode = NULL;
}

TreeViewIterator::TreeViewIterator(TreeNode* node)
{
  m_currentNode = node;
}

TreeViewIterator::TreeViewIterator(const TreeViewIterator& other)
{
  m_currentNode = other.m_currentNode;
}

TreeViewIterator& TreeViewIterator::operator=(const TreeViewIterator& other)
{
  m_currentNode = other.m_currentNode;
  return *this;
}

void TreeViewIterator::increment()
{
  // does the node have children? 
  if (!m_currentNode->m_children.empty()) {
    // return the first one
    m_currentNode = m_currentNode->m_children.front();
  }
  // if the node doesn't have children, we must go to the next brother
  else {
    while (m_currentNode->m_parent != NULL) {
      std::vector<TreeNode*>::iterator it, begin, end;

      begin = m_currentNode->m_parent->m_children.begin();
      end = m_currentNode->m_parent->m_children.end();

      it = std::find(begin, end, m_currentNode);
      if (it == end) {
	m_currentNode = m_currentNode->m_parent;
      }
      else {
	++it;
	if (it == end) {
	  m_currentNode = m_currentNode->m_parent;
	}
	else {
	  m_currentNode = *it;
	  break;
	}
      }
    }
  }
}

void TreeViewIterator::decrement()
{
  // TODO
}

bool TreeViewIterator::equal(TreeViewIterator const& other) const
{
  return (this->m_currentNode == other.m_currentNode);
}
    
//////////////////////////////////////////////////////////////////////
// TreeView

TreeView::TreeView(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_TREEVIEW), parent, style)
{
  m_root.m_owner = this;
  m_deleted = false;
  m_isDragging = false;

//   Widget::setBgColor(Color(TreeView_GetBkColor(getHandle())));
  setBgColor(System::getColor(COLOR_WINDOW));
}

TreeView::~TreeView()
{
  m_deleted = true;
}

TreeView::iterator TreeView::begin()
{
  if (!m_root.m_children.empty())
    return iterator(m_root.m_children.front());
  else
    return iterator(&m_root);
}

TreeView::iterator TreeView::end()
{
  return iterator(&m_root);
}

bool TreeView::isDragAndDrop()
{
  return (getStyle().regular & TVS_DISABLEDRAGDROP) != 0;
}

void TreeView::setDragAndDrop(bool state)
{
  if (!state)
    addStyle(Style(TVS_DISABLEDRAGDROP, 0));
  else
    removeStyle(Style(TVS_DISABLEDRAGDROP, 0));
}

/**
 * Sets the ImageList of icons to be used in the TreeNode.
 *
 * @param imageList
 *   The ImageList to be used. It will not be deleted.
 */
void TreeView::setNormalImageList(const ImageList& imageList)
{
  assert(::IsWindow(getHandle()));

  m_normalImageList = imageList;
  TreeView_SetImageList(getHandle(), m_normalImageList.getHandle(), LVSIL_NORMAL);
}

void TreeView::setStateImageList(const ImageList& imageList)
{
  assert(::IsWindow(getHandle()));

  m_stateImageList = imageList;
  TreeView_SetImageList(getHandle(), m_stateImageList.getHandle(), LVSIL_STATE);
}

/**
 * Returns the root node of the tree.
 *
 * The root node is never displayed.
 */
TreeNode* TreeView::getRootNode()
{
  return &m_root;
}

/**
 * Adds a node in the root of the tree.
 */
void TreeView::addNode(TreeNode* node)
{
  // the node must be alone (not inside another TreeView)
  assert(node != NULL);

  // add the node in the root of the TreeView
  m_root.addNode(node);
}

/**
 * Removes the specified node whatever it's inside the tree.
 * 
 * @warning The node must be inside the tree.
 */
void TreeView::removeNode(TreeNode* node)
{
  assert(node != NULL);

  // we must to scan all the nodes to find the direct parent of "node"
  std::vector<TreeNode*> parents;
  parents.push_back(&m_root);

  while (!parents.empty()) {
    // get the last element and remove it from the tail
    TreeNode* parent = *(parents.end()-1);
    parents.erase(parents.end()-1);

    // the parent of the node is the current parent
    if (node->m_parent == parent) {
      // remove the node from its parent
      parent->removeNode(node);

      // done
      return;
    }
    // does the parent have children? (it should, but "parent" could
    // be a leaf of the tree...)?
    else if (parent->hasChildren()) {
      // add all the children of "parent" to "parents" so we can
      // continuing the search...
      TreeNode::Container children = parent->getChildren();
      std::copy(children.begin(),
		children.end(),
		std::back_inserter(parents));
    }
  }

  // does the node not belong to the TreeView?
  assert(false);
}

/**
 * Returns the selected node of the tree.
 * 
 * @return The selected node. NULL if there is no selected item in the tree.
 */
TreeNode* TreeView::getSelectedNode()
{
  assert(::IsWindow(getHandle()));

  HTREEITEM htreeitem = TreeView_GetSelection(getHandle());
  if (htreeitem != NULL)
    return TreeNode::fromHandle(getHandle(), htreeitem);
  else
    return NULL;
}

/**
 * Selects the node in the tree. The @a node must be a children of the
 * tree.
 */
void TreeView::setSelectedNode(TreeNode* node)
{
  assert(::IsWindow(getHandle()));
  assert(node->m_owner == this);

  TreeView_SelectItem(getHandle(), node->getHandle());
}

void TreeView::setBgColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  Widget::setBgColor(color);
  TreeView_SetBkColor(getHandle(), color.getColorRef());
}

void TreeView::onMouseMove(MouseEvent& ev)
{
  if (m_isDragging) {
    ImageList_DragLeave(NULL);
    
    TVHITTESTINFO tvht;
    ZeroMemory(&tvht, sizeof(TVHITTESTINFO));
    tvht.flags = TVHT_ONITEM;
    tvht.pt.x = ev.getX();
    tvht.pt.y = ev.getY();
    // ScreenToClient(getHandle(), &(tvht.pt));

    HTREEITEM h_item = reinterpret_cast<HTREEITEM>
      (sendMessage(TVM_HITTEST, 0, reinterpret_cast<LPARAM>(&tvht)));
    if (h_item) {
      TreeView_SelectDropTarget(getHandle(), h_item);
      setCursor(Cursor(SysCursor::Arrow));
    }
    else {
      TreeView_SelectDropTarget(getHandle(), NULL);
      setCursor(Cursor(SysCursor::Forbidden));
    }

    Point pt = ev.getPoint() + getAbsoluteClientBounds().getOrigin();
    ImageList_DragMove(pt.x, pt.y);
    ImageList_DragEnter(NULL, pt.x, pt.y);
  }
}

void TreeView::onMouseUp(MouseEvent& ev)
{
  if (m_isDragging) {
    m_isDragging = false;
    releaseMouse();
    setCursor(Cursor(SysCursor::Arrow));
    
    HTREEITEM h_item = NULL;
    TVHITTESTINFO tv_ht;
    TVITEM tv;
    ZeroMemory(&tv, sizeof(TVITEM));
    ZeroMemory(&tv_ht, sizeof(TVHITTESTINFO));
	
    ImageList_DragLeave(NULL);
    ImageList_EndDrag();
    tv_ht.pt = ev.getPoint();
    tv_ht.flags = TVHT_ONITEM;
    h_item = (HTREEITEM)sendMessage(TVM_HITTEST, 0, (LPARAM)&tv_ht);
    if (h_item)
      PostMessage(getHandle(), M_AFTERDRAGANDDROP, (WPARAM)0, (LPARAM)h_item);
    else
      TreeView_SelectDropTarget(getHandle(), NULL);
  }
}

/**
 * You can cancel this event (Event::cancel).
 */
void TreeView::onBeforeExpand(TreeViewEvent& ev)
{
  BeforeExpand(ev);
  ev.getTreeNode()->onBeforeExpand(ev);
}

/**
 * You can cancel this event (Event::cancel).
 */
void TreeView::onBeforeCollapse(TreeViewEvent& ev)
{
  BeforeCollapse(ev);
  ev.getTreeNode()->onBeforeCollapse(ev);
}

void TreeView::onBeforeSelect(TreeViewEvent& ev)
{
  if (!m_deleted) {
    BeforeSelect(ev);
    ev.getTreeNode()->onBeforeSelect(ev);
  }
}

/**
 * You can cancel this event to cancel the label editing.
 * 
 */
void TreeView::onBeforeLabelEdit(TreeViewEvent& ev)
{
  BeforeLabelEdit(ev);
  ev.getTreeNode()->onBeforeLabelEdit(ev);
}

void TreeView::onAfterExpand(TreeViewEvent& ev)
{
  AfterExpand(ev);
  ev.getTreeNode()->onAfterExpand(ev);
}

void TreeView::onAfterCollapse(TreeViewEvent& ev)
{
  AfterCollapse(ev);
  ev.getTreeNode()->onAfterCollapse(ev);
}

void TreeView::onAfterSelect(TreeViewEvent& ev)
{
  if (!m_deleted) {
    AfterSelect(ev);
    ev.getTreeNode()->onAfterSelect(ev);
  }
}

/**
 * You can cancel this event to avoid to change the item text.
 * 
 */
void TreeView::onAfterLabelEdit(TreeViewEvent& ev)
{
  ev.getTreeNode()->onAfterLabelEdit(ev);
  AfterLabelEdit(ev);
}

bool TreeView::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  if (Widget::onReflectedNotify(lpnmhdr, lResult))
    return true;

  switch (lpnmhdr->code) {

    case TVN_GETDISPINFO: {
      LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
      TreeNode* node = reinterpret_cast<TreeNode*>(lptvdi->item.lParam);

      assert(node != NULL);
      // why why WHY?  When I delete a TreeNode (calling TreeView_DeleteItem)
      // Windows sends a TVN_GETDISPINFO...
      if (node->m_deleted)
	break;

      if (lptvdi->item.mask & TVIF_TEXT) {
	m_tmpBuffer = node->getText();
	lptvdi->item.pszText = const_cast<LPTSTR>(m_tmpBuffer.c_str());
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
      TreeNode* node = reinterpret_cast<TreeNode*>(lppnmtv->itemNew.lParam);

      assert(node != NULL);
      if (node->m_deleted)
	break;

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
      TreeNode* node = reinterpret_cast<TreeNode*>(lppnmtv->itemNew.lParam);

      assert(node != NULL);
      if (node->m_deleted)
	break;

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
      TreeNode* node = reinterpret_cast<TreeNode*>(lppnmtv->itemNew.lParam);

      assert(node != NULL);
      if (node->m_deleted)
	break;

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
      TreeNode* node = reinterpret_cast<TreeNode*>(lppnmtv->itemNew.lParam);

      assert(node != NULL);
      if (node->m_deleted)
	break;

      TreeViewEvent ev(this, node);
      onAfterSelect(ev);

      lResult = 0;
      return true;
    }

    case TVN_BEGINLABELEDIT: {
      LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
      TreeNode* node = reinterpret_cast<TreeNode*>(lptvdi->item.lParam);

      assert(node != NULL);
      if (node->m_deleted)
	break;

      TreeViewEvent ev(this, node);
      onBeforeLabelEdit(ev);

      // cancel editing
      lResult = ev.isCanceled() ? TRUE: FALSE; // TRUE cancels the editing
      return true;
    }

    case TVN_ENDLABELEDIT: {
      LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr);
      TreeNode* node = reinterpret_cast<TreeNode*>(lptvdi->item.lParam);

      assert(node != NULL);
      if (node->m_deleted)
	break;

      TreeViewEvent ev(this, node,
		       lptvdi->item.pszText != NULL ? String(lptvdi->item.pszText):
						      String(L""));

      // the label editing was canceled by the user?
      if (lptvdi->item.pszText == NULL)
	ev.cancel();

      onAfterLabelEdit(ev);

      // cancel label change (this hasn't effect, because we use LPSTR_TEXTCALLBACK)
      lResult = ev.isCanceled() ? FALSE: TRUE; // FALSE rejects the edited text
      return true;
    }

    case TVM_DELETEITEM: {
      // what we can do?
      break;
    }

    case TVN_BEGINDRAG:
    case TVN_BEGINRDRAG: {
      LPNMTREEVIEW lppnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
      TreeNode* node = reinterpret_cast<TreeNode*>(lppnmtv->itemNew.lParam);

      captureMouse();
      m_isDragging = true;

#if 1
      m_dragImage = ImageList(TreeView_CreateDragImage(getHandle(),
						       lppnmtv->itemNew.hItem));
#else
      m_dragImage = ImageList(Size(64, 32));
      Image img(Size(64, 32), 32);
      Brush black(Color::Black);
      img.getGraphics()->fillRect(black, 0, 0, 64, 32);
      Pen white(Color::White);
      img.getGraphics()->drawRect(white, 0, 0, 64, 32);
      ImageList_Add(m_dragImage.getHandle(), img.getHandle(), NULL);
#endif

      ImageList_BeginDrag(m_dragImage.getHandle(), 0, 0, 0);
      setCursor(Cursor(SysCursor::Forbidden));

      ::ClientToScreen(getHandle(), &(lppnmtv->ptDrag));
      ImageList_DragEnter(NULL, lppnmtv->ptDrag.x, lppnmtv->ptDrag.y);

      return true;
    }

  }

  return false;
}

bool TreeView::wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
  if (Widget::wndProc(message, wParam, lParam, lResult))
    return true;

  switch (message) {

    case M_AFTERDRAGANDDROP: {
      TreeView_SelectDropTarget(getHandle(), NULL);
      TreeView_SelectItem(getHandle(), (HTREEITEM)lParam);
      break;
    }

  }

  return false;
}
