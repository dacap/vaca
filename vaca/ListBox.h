// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_LISTBOX_H
#define VACA_LISTBOX_H

#include "vaca/base.h"
#include "vaca/Widget.h"

#include <vector>

namespace vaca {

/**
   List box control. Widget to manage a list of strings.
*/
class VACA_DLL ListBox : public Widget
{
public:

  struct VACA_DLL Styles {

    /**
       Default style for ListBox.
    */
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Focusable |
      Widget::Styles::ClientEdge |
      Widget::Styles::Scroll |
      Style(LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, 0);

  };

  ListBox(Widget* parent, Style style = Styles::Default);
  virtual ~ListBox();

  int addItem(const String& text);
  void insertItem(int itemIndex, const String& text);
  void removeItem(int itemIndex);

  int getItemCount();
  Rect getItemBounds(int itemIndex);

  String getItemText(int itemIndex);
  void setItemText(int itemIndex, const String& text);

  int getSelectedItem();
  void setSelectedItem(int itemIndex);

  std::vector<int> getSelectedItems();

  // Signals
  Signal<void(Event&)> ItemDoubleClick; ///< @see onItemDoubleClick
  Signal<void(Event&)> SelChange; ///< @see onSelChange

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);

  // New events
  virtual void onItemDoubleClick(Event& ev);
  virtual void onSelChange(Event& ev);

  // Reflected notifications
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);

};

class VACA_DLL DragListBox : public ListBox
{
public:

  struct VACA_DLL Styles {
    static constexpr Style Default = ListBox::Styles::Default;
  };

  DragListBox(Widget* parent, Style style = Styles::Default);

};

} // namespace vaca

#endif // VACA_LISTBOX_H
