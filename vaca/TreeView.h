// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TREEVIEW_H
#define VACA_TREEVIEW_H

#include "vaca/base.h"
#include "vaca/Widget.h"
#include "vaca/TreeNode.h"
#include "vaca/ImageList.h"
#include "vaca/Point.h"

#include <iterator>

namespace vaca {

/**
   @internal You should use @link vaca::TreeView::iterator TreeView::iterator @endlink.
*/
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

/**
   Handles a TreeView control.  A TreeView (internally) has a root
   TreeNode that is the parent of the first-level's nodes.
*/
class VACA_DLL TreeView : public Widget
{
  friend class TreeNode;

  TreeNode m_root;
  bool m_deleted;
  TreeNode* m_dragSource;
  ImageList m_dragImage;
  ImageList m_normalImageList;
  ImageList m_stateImageList;

  /**
     To use LPSTR_TEXTCALLBACK we need some space
     to allocate text temporally.
  */
  String m_tmpBuffer;

public:

  /**
     Iterator to go through all the nodes of the tree (it does a deep
     scan).
  */
  typedef TreeViewIterator iterator;

  struct VACA_DLL Styles {
    /**
       With this style in a TreeeView the label of each TreeNode can be edited.
    */
    static constexpr Style EditLabel = Style(TVS_EDITLABELS, 0);

    static constexpr Style ShowSelectionAlways = Style(TVS_SHOWSELALWAYS, 0);
    static constexpr Style WithButtons = Style(TVS_HASBUTTONS, 0);
    static constexpr Style WithLines = Style(TVS_HASLINES, 0);
    static constexpr Style RootWithLines = Style(TVS_LINESATROOT, 0);
    static constexpr Style FullRowSelect = Style(TVS_FULLROWSELECT, 0);
    static constexpr Style NoDragAndDrop = Style(TVS_DISABLEDRAGDROP, 0);

    static constexpr Style Default = (Widget::Styles::Visible |
                                      Widget::Styles::Focusable |
                                      Widget::Styles::ClientEdge |
                                      TreeView::Styles::WithButtons |
                                      TreeView::Styles::WithLines |
                                      TreeView::Styles::RootWithLines |
                                      TreeView::Styles::ShowSelectionAlways);
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

} // namespace vaca

#endif // VACA_TREEVIEW_H
