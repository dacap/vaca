// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/TreeView.h"
#include "vaca/TreeViewEvent.h"
#include "vaca/System.h"
#include "vaca/Debug.h"
#include "vaca/ImageList.h"
#include "vaca/WidgetClass.h"
#include "vaca/MouseEvent.h"
#include "vaca/Cursor.h"
#include "vaca/SetCursorEvent.h"

#include "vaca/Brush.h"
#include "vaca/Pen.h"
#include "vaca/win32.h"

using namespace vaca;

// ======================================================================
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

// ======================================================================
// TreeView

TreeView::TreeView(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_TREEVIEW), parent, style)
{
  m_root.m_owner = this;
  m_deleted = false;
  m_dragSource = NULL;

  // Widget::setBgColor(Color(TreeView_GetBkColor(getHandle())));
  setBgColor(System::getColor(COLOR_WINDOW));
}

TreeView::TreeView(HWND handle)
  : Widget(handle)
{
  m_root.m_owner = this;
  m_deleted = false;
  m_dragSource = NULL;

  // Widget::setBgColor(Color(TreeView_GetBkColor(getHandle())));
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

bool TreeView::isDragAndDrop() const
{
  return !hasStyle(Styles::NoDragAndDrop);
}

void TreeView::setDragAndDrop(bool state)
{
  if (!state)
    addStyle(Styles::NoDragAndDrop);
  else
    removeStyle(Styles::NoDragAndDrop);
}

/**
   Sets the ImageList of icons to be used in the TreeNode.

   @param imageList
     The ImageList to be used. It will not be deleted.
*/
void TreeView::setImageList(const ImageList& imageList)
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

int TreeView::getRowHeight() const
{
  assert(::IsWindow(getHandle()));

  return TreeView_GetItemHeight(getHandle());
}

void TreeView::setRowHeight(int height)
{
  assert(::IsWindow(getHandle()));
  assert(height > 0);

  TreeView_SetItemHeight(getHandle(), height);
}

/**
   Adds a node in the root of the tree.
*/
void TreeView::addNode(TreeNode* node)
{
  // the node must be alone (not inside another TreeView)
  assert(node != NULL);

  // add the node in the root of the TreeView
  m_root.addNode(node);
}

/**
   Removes the specified node whatever it's inside the tree.

   @warning The node must be inside the tree.
*/
void TreeView::removeNode(TreeNode* node)
{
  assert(node != NULL);
  assert(node->m_parent != NULL);
  assert(node->m_owner == this);

  node->m_parent->removeNode(node);
}

int TreeView::getNodeCount() const
{
  assert(::IsWindow(getHandle()));

  return TreeView_GetCount(getHandle());
}

int TreeView::getVisibleCount() const
{
  assert(::IsWindow(getHandle()));

  return TreeView_GetVisibleCount(getHandle());
}

/**
   Returns the selected node of the tree.

   @return The selected node. NULL if there is no selected item in the tree.
*/
TreeNode* TreeView::getSelectedNode() const
{
  assert(::IsWindow(getHandle()));

  HTREEITEM htreeitem = TreeView_GetSelection(getHandle());
  if (htreeitem != NULL)
    return TreeNode::fromHandle(getHandle(), htreeitem);
  else
    return NULL;
}

/**
   Selects the node in the tree. The @a node must be a children of the
   tree.
*/
void TreeView::setSelectedNode(TreeNode* node)
{
  assert(::IsWindow(getHandle()));
  assert(node->m_owner == this);
  assert(node->getHandle() != NULL);

  TreeView_SelectItem(getHandle(), node->getHandle());
}

TreeNode* TreeView::getDropTarget() const
{
  assert(::IsWindow(getHandle()));

  HTREEITEM htreeitem = TreeView_GetDropHilight(getHandle());
  if (htreeitem != NULL)
    return TreeNode::fromHandle(getHandle(), htreeitem);
  else
    return NULL;
}

void TreeView::setDropTarget(TreeNode* node)
{
  assert(::IsWindow(getHandle()));

  if (node) {
    assert(node->m_owner == this);
    assert(node->getHandle() != NULL);

    TreeView_SelectDropTarget(getHandle(), node->getHandle());
  }
  else
    TreeView_SelectDropTarget(getHandle(), NULL);
}

TreeNode* TreeView::getNodeInPoint(const Point& pt)
{
  assert(::IsWindow(getHandle()));

  TVHITTESTINFO tvht;
  ZeroMemory(&tvht, sizeof(TVHITTESTINFO));
  tvht.flags = TVHT_ONITEM;
  tvht.pt.x = pt.x;
  tvht.pt.y = pt.y;

  HTREEITEM htreeitem = reinterpret_cast<HTREEITEM>
    (sendMessage(TVM_HITTEST, 0, reinterpret_cast<LPARAM>(&tvht)));

  if (htreeitem != NULL)
    return TreeNode::fromHandle(getHandle(), htreeitem);
  else
    return NULL;
}

void TreeView::setBgColor(const Color& color)
{
  assert(::IsWindow(getHandle()));

  Widget::setBgColor(color);
  TreeView_SetBkColor(getHandle(), convert_to<COLORREF>(color));
}

void TreeView::onMouseMove(MouseEvent& ev)
{
  if (m_dragSource) {
    ImageList_DragLeave(NULL);

    TreeNode* node = getNodeInPoint(ev.getPoint());
    if (node)
      setDropTarget(node);
    else
      setDropTarget(NULL);

    Point pt = getAbsoluteClientBounds().getOrigin() + ev.getPoint();

    ImageList_DragMove(pt.x, pt.y);
    ImageList_DragEnter(NULL, pt.x, pt.y);
  }
}

void TreeView::onMouseUp(MouseEvent& ev)
{
  if (m_dragSource) {
    releaseMouse();

    ImageList_DragLeave(NULL);
    ImageList_EndDrag();

    TreeNode* dropNode = getDropTarget();

    if (dropNode != NULL &&
	m_dragSource != dropNode &&
	!m_dragSource->isAncestorOf(dropNode)) {
      // Remove the node from the TreeView
      removeNode(m_dragSource);

      // Add it again in the target node
      dropNode->addNode(m_dragSource);
    }

    setDropTarget(NULL);

    // Select the source
    setSelectedNode(m_dragSource);
    m_dragSource = NULL;
  }
}

void TreeView::onSetCursor(SetCursorEvent& ev)
{
  if (!ev.isConsumed() && m_dragSource) {
    TreeNode* dropNode = getDropTarget();

    if (dropNode != NULL &&
	m_dragSource != dropNode &&
	!m_dragSource->isAncestorOf(dropNode)) {
      ev.setCursor(Cursor(SysCursor::Arrow));
    }
    else {
      ev.setCursor(Cursor(SysCursor::Forbidden));
    }
  }
  Widget::onSetCursor(ev);
}

/**
   You can cancel this event (Event::cancel).
*/
void TreeView::onBeforeExpand(TreeViewEvent& ev)
{
  BeforeExpand(ev);
  ev.getTreeNode()->onBeforeExpand(ev);
}

/**
   You can cancel this event (Event::cancel).
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
   You can cancel this event to cancel the label editing.

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
   You can cancel this event to avoid to change the item text.
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

      // ======================================================================
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

      // ======================================================================
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

      // ======================================================================
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

      // ======================================================================
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
      m_dragSource = node;

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

      int ex = 0;
      if (m_normalImageList != NULL)
	ex += m_normalImageList.getImageSize().w+2;

      ImageList_BeginDrag(m_dragImage.getHandle(), 0,
			  lppnmtv->ptDrag.x - node->getBounds().x + ex,
			  lppnmtv->ptDrag.y - node->getBounds().y);
      //setCursor(Cursor(SysCursor::Forbidden));

      // m_dragStartPoint = Point(lppnmtv->ptDrag.x, lppnmtv->ptDrag.y);

      ::ClientToScreen(getHandle(), &(lppnmtv->ptDrag));
      ImageList_DragEnter(NULL, lppnmtv->ptDrag.x, lppnmtv->ptDrag.y);

      return true;
    }

  }

  return false;
}
