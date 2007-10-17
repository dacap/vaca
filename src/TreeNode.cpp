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
#include "Vaca/TreeViewEvent.h"
#include "Vaca/Debug.h"

using namespace Vaca;

TreeNode::TreeNode(const String &text, int imageIndex, int selectedImageIndex)
  : m_text(text)
  , m_image(imageIndex)
  , m_selectedImage(selectedImageIndex)
{
  m_parent = NULL;
  m_HTREEITEM = NULL;
  m_owner = NULL;
}

TreeNode::~TreeNode()
{
  if (m_owner)
    TreeView_DeleteItem(m_owner->getHWND(), m_HTREEITEM);

  for (Container::iterator it=m_children.begin();
       it!=m_children.end(); ++it)
    delete *it;
}

/**
 * Adds a new sub-node (child) to this one. After calling this method
 * @a node'll have this node as parent.
 */
void TreeNode::addNode(TreeNode *node)
{
  assert(node->m_HTREEITEM == NULL && node->m_parent == NULL);

  this->m_children.push_back(node);

  node->m_parent = this;
  node->addToTreeView(this->m_owner);
}

TreeNode *TreeNode::getParent()
{
  if (m_owner != NULL &&
      &m_owner->m_root == m_parent)
    return NULL;
  else
    return m_parent;
}

TreeView *TreeNode::getTreeView()
{
  return m_owner;
}

/**
 * Returns the collection of children.
 * 
 */
TreeNode::Container TreeNode::getChildren()
{
  return m_children;
}

/**
 * Returns true if this node should have the plus sign to be
 * expanded. The default implementation returns true if the m_children
 * member has elements. If you override this method isn't necessary to
 * call the base implementation.
 * 
 * @code
 * class MyTreeNode : public TreeNode
 * {
 * protected:
 *   bool hasChildren() {
 *     return ...;
 *   }
 * };
 * @endcode
 */
bool TreeNode::hasChildren()
{
  return !m_children.empty();
}

/**
 * Returns the label's text for the node. If you override this method
 * isn't necessary to call the base implementation.
 * 
 * @code
 * class MyTreeNode : public TreeNode
 * {
 * protected:
 *   String getText() {
 *     return "...";
 *   }
 * };
 * @endcode
 */
String TreeNode::getText()
{
  return m_text;
}

/**
 * Returns the index of the image that uses this node from the
 * ImageList. If you override this method isn't necessary to call the
 * base implementation.
 * 
 * @code
 * class MyTreeNode : public TreeNode
 * {
 * protected:
 *   int getImage() {
 *     int imageIndex = -1;
 *     // ... do some work here
 *     return imageIndex;
 *   }
 * };
 * @endcode
 */
int TreeNode::getImage()
{
  return m_image;
}

/**
 * Returns the index of the image in selected-state that uses this
 * node from the ImageList. If you override this method isn't
 * necessary to call the base implementation.
 * 
 * @seealso getImage()
 */
int TreeNode::getSelectedImage()
{
  return m_selectedImage;
}

/**
 * Sets the node's text. It's useful only if you are using the default
 * implementation of getText().
 * 
 */
void TreeNode::setText(const String &text)
{
  m_text = text;
}

/**
 * Sets the node's image. It's useful only if you are using the
 * default implementation of getImage().
 * 
 */
void TreeNode::setImage(int imageIndex)
{
  m_image = imageIndex;
}

/**
 * Sets the node's image in selected-state. It's useful only if you
 * are using the default implementation of getSelectedImage().
 * 
 */
void TreeNode::setSelectedImage(int selectedImageIndex)
{
  m_selectedImage = selectedImageIndex;
}

HTREEITEM TreeNode::getHTREEITEM()
{
  return m_HTREEITEM;
}

TreeNode *TreeNode::fromHTREEITEM(HWND hwnd, HTREEITEM htreeitem)
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
  // do nothing
}

void TreeNode::onBeforeCollapse(TreeViewEvent &ev)
{
  // do nothing
}

void TreeNode::onBeforeSelect(TreeViewEvent &ev)
{
  // do nothing
}

void TreeNode::onBeforeLabelEdit(TreeViewEvent &ev)
{
  // do nothing
}

void TreeNode::onAfterExpand(TreeViewEvent &ev)
{
  // do nothing
}

void TreeNode::onAfterCollapse(TreeViewEvent &ev)
{
  // do nothing
}

void TreeNode::onAfterSelect(TreeViewEvent &ev)
{
  // do nothing
}

/**
 * The default implementation changes the m_text field to be returned
 * in getText() only if @a ev wasn't canceled.
 * 
 */
void TreeNode::onAfterLabelEdit(TreeViewEvent &ev)
{
  // if the event isn't cancelled, change the label
  if (!ev.isCanceled())
    m_text = ev.getLabel();
}

void TreeNode::addToTreeView(TreeView *treeView)
{
  assert(m_owner == NULL);

  m_owner = treeView;
  if (treeView) {
    TVINSERTSTRUCT is;

    is.hParent = m_parent->m_HTREEITEM;
    is.hInsertAfter = TVI_LAST;
    is.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    is.item.pszText = LPSTR_TEXTCALLBACK;
    is.item.iImage = I_IMAGECALLBACK;
    is.item.iSelectedImage = I_IMAGECALLBACK;
    is.item.cChildren = I_CHILDRENCALLBACK;
    is.item.lParam = reinterpret_cast<LPARAM>(this);

    m_HTREEITEM = TreeView_InsertItem(treeView->getHWND(), &is);
  }

  for (Container::iterator it=m_children.begin();
       it!=m_children.end(); ++it) {
    if ((*it)->m_owner == NULL)
      (*it)->addToTreeView(treeView);
  }
}
