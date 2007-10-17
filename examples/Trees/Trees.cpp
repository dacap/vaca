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

#include "Vaca/Vaca.h"

using namespace Vaca;

// this is a customized TreeNode
class InfiniteTreeNode : public TreeNode
{
public:

  InfiniteTreeNode(const String &text)
    : TreeNode(text)
  {
  }

  virtual bool hasChildren()
  {
    // an infinite node always as children
    return true;
  }

  // before expand an we infinite tree node, we should add some children
  virtual void onBeforeExpand(TreeViewEvent &ev)
  {
    // did we add the children already? if not...
    if (getChildren().empty()) {
      // add ten sub-infinite nodes as children
      for (int c=0; c<10; c++)
	addNode(new InfiniteTreeNode(getText() + " " +
				     String::fromInt(c)));
    }
  }

  // how we can avoid to edit one node label?
  virtual void onBeforeLabelEdit(TreeViewEvent &ev)
  {
    // ...cancelling the BeforeLabelEdit event
    ev.cancel();
  }

};

class MainFrame : public Frame
{
  TreeView mTreeView;
  Label mLabel;

public:

  MainFrame()
    : Frame("Trees")
    , mTreeView(this)
    , mLabel("", this)
  {
    // layout
    setLayout(new BoxLayout(Vertical, false));
    mTreeView.setConstraint(new BoxConstraint(true));

    // add the first node (the most simple node without children)
    mTreeView.addNode(new TreeNode("Leaf node"));

    // creates a node with three sub-nodes
    TreeNode *node = new TreeNode("Node with children");
    node->addNode(new TreeNode("Leaf A"));
    node->addNode(new TreeNode("Leaf B"));
    node->addNode(new TreeNode("Leaf C"));

    // add the second node (a node that has children)
    mTreeView.addNode(node);

    // add the third node (a custom node, the infinite node that we defined)
    mTreeView.addNode(new InfiniteTreeNode("Infinite Node"));

    // bind some events
    mTreeView.AfterExpand.connect(Bind(&MainFrame::onAfter, this, boost::arg<1>(), "expanded"));
    mTreeView.AfterCollapse.connect(Bind(&MainFrame::onAfter, this, boost::arg<1>(), "collapsed"));
    mTreeView.AfterSelect.connect(Bind(&MainFrame::onAfter, this, boost::arg<1>(), "selected"));
    mTreeView.AfterLabelEdit.connect(Bind(&MainFrame::onAfter, this, boost::arg<1>(), "edited"));
  }

protected:

  void onAfter(TreeViewEvent &ev, String action)
  {
    mLabel.setText("You "+action+" \""+ev.getTreeNode()->getText()+"\" item.");
    // the label could be bigger, relayout...
    layout();
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
