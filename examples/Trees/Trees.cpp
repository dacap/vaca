// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#include <Vaca/Vaca.h>
#include "resource.h"

using namespace Vaca;

enum {
  IMAGE_SPHERE,
  IMAGE_PYRAMID,
  IMAGE_CUBE,
  IMAGE_INFINITE,
};

// This is a customized TreeNode
class InfiniteTreeNode : public TreeNode
{
public:

  InfiniteTreeNode(const String &text)
    : TreeNode(text, IMAGE_INFINITE)
  {
  }

  virtual bool hasChildren()
  {
    // An infinite node always as children
    return true;
  }

  // Before expand an we infinite tree node, we should add some children
  virtual void onBeforeExpand(TreeViewEvent &ev)
  {
    TreeNode::onBeforeExpand(ev);

    // Did we add the children already? if not...
    if (getChildren().empty()) {
      // Add three sub-infinite nodes as children
      for (int c=0; c<3; c++)
	addNode(new InfiniteTreeNode(format_string(L"%s/%d", getText().c_str(), c+1)));
    }
  }

  // How we can avoid to edit one node label?
  virtual void onBeforeLabelEdit(TreeViewEvent &ev)
  {
    TreeNode::onBeforeLabelEdit(ev);

    // ...cancelling the BeforeLabelEdit event
    ev.cancel();
  }

};

class MainFrame : public Frame
{
  ImageList m_imageList;
  CheckBox m_dragAndDrop1;
  CheckBox m_dragAndDrop2;
  TreeView m_treeView1;
  TreeView m_treeView2;
  Button m_addItem;
  Button m_deleteItem;
  Button m_from1to2;
  Button m_from2to1;
  StatusBar m_statusBar;

public:

  MainFrame()
    : Frame(L"Trees (WIP)")
    , m_imageList(ResourceId(IDB_IMAGES), 14, Color::Black)
    , m_dragAndDrop1(L"Drag && Drop", this)
    , m_dragAndDrop2(L"Drag && Drop", this)
    , m_treeView1(this)
    , m_treeView2(this, TreeView::Styles::Default +
			TreeView::Styles::EditLabel)
    , m_addItem(L"+", this)
    , m_deleteItem(L"-", this)
    , m_from1to2(L">", this)
    , m_from2to1(L"<", this)
    , m_statusBar(this)
  {
    m_treeView1.setImageList(m_imageList);
    m_treeView2.setImageList(m_imageList);

    // layout
    setLayout(Bix::parse(L"Y[fX[Y[%,%],fY[%,f%],Y[%,%],fY[%,f%]]]",
			 &m_addItem, &m_deleteItem,
			 &m_dragAndDrop1, &m_treeView1,
			 &m_from1to2, &m_from2to1,
			 &m_dragAndDrop2, &m_treeView2));

    m_addItem.setPreferredSize(Size(32, 16));
    m_deleteItem.setPreferredSize(Size(32, 16));
    m_from1to2.setPreferredSize(Size(32, 16));
    m_from2to1.setPreferredSize(Size(32, 16));

    m_dragAndDrop1.setSelected(m_treeView1.isDragAndDrop());
    m_dragAndDrop2.setSelected(m_treeView2.isDragAndDrop());

    // Add some nodes
    onAddItem();
    m_treeView2.addNode(new InfiniteTreeNode(L"Infinite Node"));

    // bind some events
    m_dragAndDrop1.Click.connect(Bind(&MainFrame::onDragAndDrop, this, &m_dragAndDrop1, &m_treeView1));
    m_dragAndDrop2.Click.connect(Bind(&MainFrame::onDragAndDrop, this, &m_dragAndDrop2, &m_treeView2));

    m_treeView1.AfterExpand.connect(&MainFrame::onAfterExpand, this);
    m_treeView1.AfterCollapse.connect(&MainFrame::onAfterCollapse, this);
    m_treeView1.AfterSelect.connect(&MainFrame::onAfterSelect, this);
    m_treeView1.AfterLabelEdit.connect(&MainFrame::onAfterLabelEdit, this);

    m_addItem.Click.connect(Bind(&MainFrame::onAddItem, this));
    m_deleteItem.Click.connect(Bind(&MainFrame::onDeleteItem, this));
    m_from1to2.Click.connect(Bind(&MainFrame::onFromTo, this, &m_treeView1, &m_treeView2));
    m_from2to1.Click.connect(Bind(&MainFrame::onFromTo, this, &m_treeView2, &m_treeView1));
  }

protected:

  void onDragAndDrop(CheckBox* cb, TreeView* treeView)
  {
    treeView->setDragAndDrop(cb->isSelected());
  }

  // onAfter some event (like onAfterExpand or onAfterCollapse)
  void onAfter(TreeViewEvent& ev, const String& action)
  {
    m_statusBar.setText(format_string(L"You %s \"%s\" item.",
				      action.c_str(),
				      ev.getTreeNode()->getText().c_str()));

    // the label could be bigger, relayout...
    layout();
  }

  void onAfterExpand   (TreeViewEvent& ev) { onAfter(ev, L"expanded"); }
  void onAfterCollapse (TreeViewEvent& ev) { onAfter(ev, L"collapsed"); }
  void onAfterSelect   (TreeViewEvent& ev) { onAfter(ev, L"selected"); }
  void onAfterLabelEdit(TreeViewEvent& ev) { onAfter(ev, L"edited"); }

  // adds three items in the m_treeView1
  void onAddItem()
  {
    m_treeView1.addNode(new TreeNode(L"A", IMAGE_SPHERE));
    m_treeView1.addNode(new TreeNode(L"B", IMAGE_PYRAMID));
    m_treeView1.addNode(new TreeNode(L"C", IMAGE_CUBE));
    m_treeView1.requestFocus();
  }

  // deletes the selected item in the m_treeView1
  void onDeleteItem()
  {
    TreeNode* node = m_treeView1.getSelectedNode();
    if (node != NULL)
      delete node;

    m_treeView1.requestFocus();
  }

  // when a button to pass items from one TreeView to other is pressed
  // this method is called...
  void onFromTo(TreeView* treeA, TreeView* treeB)
  {
    // get the selected node in the tree "A"
    TreeNode* nodeA = treeA->getSelectedNode();
    if (nodeA != NULL) {
      // remove the selected node in the tree "A"
      treeA->removeNode(nodeA);

      // ...and put it as a child of the selected node in the tree "B"
      TreeNode* nodeB = treeB->getSelectedNode();
      if (nodeB != NULL)
	nodeB->addNode(nodeA);
      else
	treeB->addNode(nodeA);	// ...or in the root

      treeB->setSelectedNode(nodeA);
    }
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
