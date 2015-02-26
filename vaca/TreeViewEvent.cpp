// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/TreeViewEvent.h"
#include "vaca/TreeView.h"

using namespace vaca;

TreeViewEvent::TreeViewEvent(TreeView* treeView, TreeNode* treeNode, String label)
  : CancelableEvent(treeView)
  , m_treeNode(treeNode)
  , m_label(label)
{
}

TreeViewEvent::~TreeViewEvent()
{
}

TreeNode* TreeViewEvent::getTreeNode()
{
  return m_treeNode;
}

String TreeViewEvent::getLabel()
{
  return m_label;
}

