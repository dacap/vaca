// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TREEVIEWEVENT_H
#define VACA_TREEVIEWEVENT_H

#include "vaca/base.h"
#include "vaca/CancelableEvent.h"

namespace vaca {

/**
   Event where interact a TreeView and TreeNode.
*/
class VACA_DLL TreeViewEvent : public CancelableEvent
{
  TreeNode* m_treeNode;
  String    m_label;

public:

  TreeViewEvent(TreeView* treeView, TreeNode* treeNode, String label = L"");
  virtual ~TreeViewEvent();

  TreeView* getTreeView();
  TreeNode* getTreeNode();

  String getLabel();

};

} // namespace vaca

#endif // VACA_TREEVIEWEVENT_H
