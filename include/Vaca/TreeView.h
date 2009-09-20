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

#ifndef VACA_TREEVIEW_H
#define VACA_TREEVIEW_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/TreeNode.h"
#include "Vaca/ImageList.h"
#include "Vaca/Point.h"

#include <iterator>

namespace Vaca {

/// @internal You should use @link Vaca::TreeView::iterator TreeView::iterator @endlink.
/// 
class VACA_DLL TreeViewIterator
  : public std::iterator<std::bidirectional_iterator_tag,
			 TreeNode*>
{
  TreeNode* m_currentNode;

public:
  TreeViewIterator();
  TreeViewIterator(const TreeViewIterator& other);
  explicit TreeViewIterator(TreeNode* node);

  TreeViewIterator& operator=(const TreeViewIterator& other);

  TreeNode*& operator*() { return m_currentNode; }
  TreeNode** operator->() { return &m_currentNode; }
  TreeViewIterator& operator++() {
    increment();
    return *this;
  }
  TreeViewIterator operator++(int) {
    TreeViewIterator i = *this;
    increment();
    return i;
  }
  TreeViewIterator& operator--() {
    decrement();
    return *this;
  }
  TreeViewIterator operator--(int) {
    TreeViewIterator i = *this;
    decrement();
    return i;
  }
  bool operator==(const TreeViewIterator& i) const { return equal(i); }
  bool operator!=(const TreeViewIterator& i) const { return !equal(i); }

private:
  void increment();
  void decrement();
  bool equal(TreeViewIterator const& other) const;
};

/// Handles a TreeView control.  A TreeView (internally) has a root
/// TreeNode that is the parent of the first-level's nodes.
/// 
class VACA_DLL TreeView : public Widget
{
  friend class TreeNode;

  TreeNode m_root;
  bool m_deleted;
  TreeNode* m_dragSource;
  ImageList m_dragImage;
  ImageList m_normalImageList;
  ImageList m_stateImageList;

  /// To use LPSTR_TEXTCALLBACK we need some space
  /// to allocate text temporally.
  String m_tmpBuffer;

public:

  /// Iterator to go through all the nodes of the tree (it does a deep
  /// scan).
  /// 
  typedef TreeViewIterator iterator;

  struct VACA_DLL Styles {
    static const Style EditLabel;
    static const Style ShowSelectionAlways;
    static const Style WithButtons;
    static const Style WithLines;
    static const Style RootWithLines;
    static const Style FullRowSelect;
    static const Style NoDragAndDrop;
    static const Style Default;
  };

  TreeView(Widget* parent, Style style = Styles::Default);
  explicit TreeView(HWND handle);
  virtual ~TreeView();

  iterator begin();
  iterator end();

  bool isDragAndDrop() const;
  void setDragAndDrop(bool state);

  void setImageList(const ImageList& imageList);
  void setStateImageList(const ImageList& imageList);

  int getRowHeight() const;
  void setRowHeight(int height);

  void addNode(TreeNode* node);
  void removeNode(TreeNode* node);

  int getNodeCount() const;
  int getVisibleCount() const;

  TreeNode* getSelectedNode() const;
  void setSelectedNode(TreeNode* node);

  TreeNode* getDropTarget() const;
  void setDropTarget(TreeNode* node);

  TreeNode* getNodeInPoint(const Point& pt);

  virtual void setBgColor(const Color& color);

  // Signals
  Signal1<void, TreeViewEvent&> BeforeExpand;
  Signal1<void, TreeViewEvent&> BeforeCollapse;
  Signal1<void, TreeViewEvent&> BeforeSelect;
  Signal1<void, TreeViewEvent&> BeforeLabelEdit;
  Signal1<void, TreeViewEvent&> AfterExpand;
  Signal1<void, TreeViewEvent&> AfterCollapse;
  Signal1<void, TreeViewEvent&> AfterSelect;
  Signal1<void, TreeViewEvent&> AfterLabelEdit;
//   Signal1<void, TreeViewEvent&> BeginDrag;
//   Signal1<void, TreeViewEvent&> EndDrag;

protected:
  // Events
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseUp(MouseEvent& ev);
  virtual void onSetCursor(SetCursorEvent& ev);

  // New events
  virtual void onBeforeExpand(TreeViewEvent& ev);
  virtual void onBeforeCollapse(TreeViewEvent& ev);
  virtual void onBeforeSelect(TreeViewEvent& ev);
  virtual void onBeforeLabelEdit(TreeViewEvent& ev);
  virtual void onAfterExpand(TreeViewEvent& ev);
  virtual void onAfterCollapse(TreeViewEvent& ev);
  virtual void onAfterSelect(TreeViewEvent& ev);
  virtual void onAfterLabelEdit(TreeViewEvent& ev);
//   virtual void onBeginDrag(TreeViewEvent& ev);
//   virtual void onEndDrag(TreeViewEvent& ev);

  // Reflected notifications
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

};

} // namespace Vaca

#endif // VACA_TREEVIEW_H
