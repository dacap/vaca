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

#ifndef VACA_TREENODE_H
#define VACA_TREENODE_H

#include <commctrl.h>
#include <vector>

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

class TreeViewEvent;

/**
 * A TreeView node.  It has a text label, an image, a selected image
 * asociated, and a state (collapsed/expanded). A node could be a
 * parent of sub-TreeNodes.
 */
class VACA_DLL TreeNode
{
public:

  typedef std::vector<TreeNode *> Container;

private:

  friend class TreeView;

  String mText;
  TreeNode *mParent;
  Container mChildren;
  HTREEITEM mHtreeitem;
  TreeView *mTreeView;

public:

  TreeNode();
  TreeNode(const String &text);
  virtual ~TreeNode();

  void addNode(TreeNode *node);

  // TODO setText, isExpanded(), isCollapsed()
  //void setText(const String &text);
  //bool isExpanded();
  //bool isCollapsed();

  TreeNode *getParent();
  TreeView *getTreeView();
  Container getChildren();

  virtual bool hasChildren();
  virtual String getText();
  virtual int getImage();
  virtual int getSelectedImage();

  // TODO program this
//   void expand() { }
//   void collapse() { }

  // TODO fix this.
//   virtual void onExpand() { }
//   virtual void onCollapse() { }

  HTREEITEM getHtreeitem();

  static TreeNode *fromHtreeitem(HWND hwnd, HTREEITEM htreeitem);

protected:
  // new events
  virtual void onBeforeExpand(TreeViewEvent &ev);
  virtual void onBeforeCollapse(TreeViewEvent &ev);
  virtual void onBeforeSelect(TreeViewEvent &ev);
  virtual void onAfterExpand(TreeViewEvent &ev);
  virtual void onAfterCollapse(TreeViewEvent &ev);
  virtual void onAfterSelect(TreeViewEvent &ev);

private:
  void addToTreeView(TreeView *treeview);

};

} // namespace Vaca

#endif
