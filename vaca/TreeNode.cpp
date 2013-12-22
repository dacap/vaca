// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/TreeNode.h"
#include "Vaca/TreeView.h"
#include "Vaca/TreeViewEvent.h"
#include "Vaca/Debug.h"
#include "Vaca/win32.h"

using namespace Vaca;

TreeNode::TreeNode(const String& text, int imageIndex, int selectedImageIndex)
  : m_text(text)
  , m_image(imageIndex)
  , m_selectedImage(selectedImageIndex >= 0 ? selectedImageIndex: imageIndex)
{
  m_parent = NULL;
  m_handle = NULL;
  m_owner = NULL;

  m_deleted = false;
}

TreeNode::~TreeNode()
{
  m_deleted = true;

  // remove this node from the "m_children" field of the parent
  if (m_parent != NULL)
    remove_from_container(m_parent->m_children, this);

  // clone "m_children" collection, because when a TreeNode is
  // deleted, it is removed by its self from the m_children of the
  // parent node (see the live of above)
  TreeNodeList clone = m_children;

  // delete all sub-nodes (children), using the cloned collection
  for (TreeNodeList::iterator
	 it=clone.begin(); it!=clone.end(); ++it)
    delete *it;

  // finally, we can remove the node from the Win32 TreeView control
  // (because it shouldn't have any children)
  if (m_owner != NULL) {
    assert(TreeView_GetChild(m_owner->getHandle(), m_handle) == NULL);

    TreeView_DeleteItem(m_owner->getHandle(), m_handle);
  }
}

TreeNode* TreeNode::getParent()
{
  if (m_owner != NULL && &m_owner->m_root == m_parent)
    return NULL;
  else
    return m_parent;
}

/**
   Returns the collection of children.

*/
TreeNodeList TreeNode::getChildren()
{
  return m_children;
}

TreeView* TreeNode::getTreeView()
{
  return m_owner;
}

/**
   Adds a new sub-node (child) to this one.

   After calling this member function @a node'll have this node as parent.
*/
void TreeNode::addNode(TreeNode* node)
{
  assert(node != NULL);
  assert(node->m_handle == NULL && node->m_parent == NULL);

  // add this node in the "m_children" container of the parent
  this->m_children.push_back(node);

  node->m_parent = this;
  node->addToTreeView(this->m_owner);
}

/**
   Removes a child node.

   @warning The specified @param node must be a child of the object @a this.
*/
void TreeNode::removeNode(TreeNode* node)
{
  assert(node != NULL);
  assert(node->m_parent == this);

  // remove this node from the "m_children" container of the parent
  remove_from_container(this->m_children, node);

  node->m_parent = NULL;
  node->removeFromTreeView();

  assert(node->m_handle == NULL);
}

bool TreeNode::isAncestorOf(TreeNode* child) const
{
  assert(child != NULL);

  child = child->getParent();
  while (child != NULL) {
    if (child == this)
      return true;

    child = child->getParent();
  }

  return false;
}

/**
   Returns true if this node should have the plus sign to be
   expanded. The default implementation returns true if the m_children
   member has elements. If you override this member function isn't
   necessary to call the base implementation.

   @code
   class MyTreeNode : public TreeNode
   {
   protected:
     bool hasChildren() {
       return ...;
     }
   };
   @endcode
*/
bool TreeNode::hasChildren()
{
  return !m_children.empty();
}

/**
   Returns the label's text for the node. If you override this member function
   isn't necessary to call the base implementation.

   @code
   class MyTreeNode : public TreeNode
   {
   protected:
     String getText() {
       return "...";
     }
   };
   @endcode
*/
String TreeNode::getText()
{
  return m_text;
}

/**
   Returns the index of the image that uses this node from the
   ImageList. If you override this member function isn't necessary
   to call the base implementation.

   @code
   class MyTreeNode : public TreeNode
   {
   protected:
     int getImage() {
       int imageIndex = -1;
       // ... do some work here
       return imageIndex;
     }
   };
   @endcode
*/
int TreeNode::getImage()
{
  return m_image;
}

/**
   Returns the index of the image in selected-state that uses this
   node from the ImageList. If you override this member function
   is not necessary to call the base implementation.

   @see getImage()
*/
int TreeNode::getSelectedImage()
{
  return m_selectedImage;
}

/**
   Sets the node's text. It's useful only if you are using the default
   implementation of getText().

*/
void TreeNode::setText(const String& text)
{
  m_text = text;
}

/**
   Sets the node's image. It's useful only if you are using the
   default implementation of getImage().

*/
void TreeNode::setImage(int imageIndex)
{
  m_image = imageIndex;
}

/**
   Sets the node's image in selected-state. It's useful only if you
   are using the default implementation of getSelectedImage().

*/
void TreeNode::setSelectedImage(int selectedImageIndex)
{
  m_selectedImage = selectedImageIndex;
}

bool TreeNode::isExpanded()
{
  // TODO
  return false;
}

void TreeNode::setExpanded(bool state)
{
  assert(m_handle != NULL && m_owner != NULL);

  TreeView_Expand(m_owner->getHandle(), m_handle, state ? TVE_EXPAND: TVE_COLLAPSE);
}

Rect TreeNode::getBounds() const
{
  assert(m_handle != NULL && m_owner != NULL);

  RECT rc;
  TreeView_GetItemRect(m_owner->getHandle(), m_handle, (WPARAM)&rc, TRUE);
  return convert_to<Rect>(rc);
}

Rect TreeNode::getRowBounds() const
{
  assert(m_handle != NULL && m_owner != NULL);

  RECT rc;
  TreeView_GetItemRect(m_owner->getHandle(), m_handle, (WPARAM)&rc, FALSE);
  return convert_to<Rect>(rc);
}

void TreeNode::ensureVisible()
{
  assert(m_handle != NULL && m_owner != NULL);

  TreeView_EnsureVisible(m_owner->getHandle(), m_handle);
}

HTREEITEM TreeNode::getHandle()
{
  return m_handle;
}

TreeNode* TreeNode::fromHandle(HWND hwnd, HTREEITEM htreeitem)
{
  TVITEMEX tvie;

  ZeroMemory(&tvie, sizeof(TVITEMEX));

  tvie.mask = TVIF_PARAM;
  tvie.hItem = htreeitem;

  TreeView_GetItem(hwnd, &tvie);

  return reinterpret_cast<TreeNode*>(tvie.lParam);
}

void TreeNode::onBeforeExpand(TreeViewEvent& ev)
{
  // do nothing
}

void TreeNode::onBeforeCollapse(TreeViewEvent& ev)
{
  // do nothing
}

void TreeNode::onBeforeSelect(TreeViewEvent& ev)
{
  // do nothing
}

void TreeNode::onBeforeLabelEdit(TreeViewEvent& ev)
{
  // do nothing
}

void TreeNode::onAfterExpand(TreeViewEvent& ev)
{
  // do nothing
}

void TreeNode::onAfterCollapse(TreeViewEvent& ev)
{
  // do nothing
}

void TreeNode::onAfterSelect(TreeViewEvent& ev)
{
  // do nothing
}

/**
   The default implementation changes the m_text field to be returned
   in getText() only if @a ev wasn't canceled.

*/
void TreeNode::onAfterLabelEdit(TreeViewEvent& ev)
{
  // if the event isn't cancelled, change the label
  if (!ev.isCanceled())
    m_text = ev.getLabel();
}

/**
   Connects the node and its children to the Win32 TreeView.
*/
void TreeNode::addToTreeView(TreeView* treeView)
{
  assert(m_owner == NULL);

  m_owner = treeView;
  if (treeView != NULL) {
    TVINSERTSTRUCT is;

    is.hParent = m_parent->m_handle;
    is.hInsertAfter = TVI_LAST;
    is.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    is.item.pszText = LPSTR_TEXTCALLBACK;
    is.item.iImage = I_IMAGECALLBACK;
    is.item.iSelectedImage = I_IMAGECALLBACK;
    is.item.cChildren = I_CHILDRENCALLBACK;
    is.item.lParam = reinterpret_cast<LPARAM>(this);

    m_handle = TreeView_InsertItem(treeView->getHandle(), &is);
  }

  // Add all children too
  for (TreeNodeList::iterator
	 it=m_children.begin(); it!=m_children.end(); ++it) {
    if ((*it)->m_owner == NULL)
      (*it)->addToTreeView(treeView);
  }
}

/**
   Disconnects the node (and its children) from the Win32 TreeView.
*/
void TreeNode::removeFromTreeView()
{
  // Remove all children from the TreeView control
  for (TreeNodeList::iterator
	 it=m_children.begin(); it!=m_children.end(); ++it) {
    (*it)->removeFromTreeView();
  }

  // Remove the node from the Win32 TreeView control (because it
  // shouldn't have any children)
  if (m_owner != NULL) {
    assert(m_handle != NULL);

    TreeView_DeleteItem(m_owner->getHandle(), m_handle);

    m_handle = NULL;
    m_owner = NULL;
  }

  assert(m_handle == NULL);
}
