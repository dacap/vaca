// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_COMBOBOX_H
#define VACA_COMBOBOX_H

#include "vaca/base.h"
#include "vaca/Widget.h"

#include <vector>

namespace vaca {

/**
   Combo box control. Widget to select an option from a drop down
   list.
*/
class VACA_DLL ComboBox : public Widget
{
  Size m_maxItemSize;

public:

  struct Styles {
    /**
       Default style for ComboBox widget. It is the classic ComboBox in
       which the user can select only items in the list (but can't edit
       text nor items).

       It contains the following styles:
       @li Widget::Styles::Visible
       @li Widget::Styles::Focusable
       @li Widget::Styles::VerticalScroll
       @li Widget::Styles::ClientEdge

       Example:
       @code
       Frame frame(...);
       // Use:
       ComboBox combo1(&frame);
       // or:
       ComboBox combo2(&frame, ComboBox::Styles::Default);
       @endcode

       @win32
       It is WS_VISIBLE | WS_TABSTOP | WS_VSCROLL,
       CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST |
       WS_EX_CLIENTEDGE
       @endwin32
    */
    static constexpr Style Default =
      Widget::Styles::Visible |
      Widget::Styles::Focusable |
      Widget::Styles::ClientEdge |
      Widget::Styles::VerticalScroll |
      Style(CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST, 0);

    /**
       Special style for ComboBox so the user can enter any text (not only
       the items in the list).

       Example:
       @code
       Frame frame(...);
       ComboBox combo(&frame, ComboBox::Styles::Editable);
       @endcode

       @win32
       It is WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
       CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWN | WS_EX_CLIENTEDGE
       @endwin32
    */
    static constexpr Style Editable =
      Widget::Styles::Visible |
      Widget::Styles::Focusable |
      Widget::Styles::ClientEdge |
      Widget::Styles::VerticalScroll |
      Style(CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_DROPDOWN, 0);

  };

  ComboBox(Widget* parent, Style style = Styles::Default);
  virtual ~ComboBox();

  int addItem(const String& text);
  void insertItem(int itemIndex, const String& text);
  void removeItem(int itemIndex);
  void removeAllItems();

  int getItemCount();

  String getItemText(int itemIndex);
  void setItemText(int itemIndex, const String& text);

  int getSelectedItem();
  void setSelectedItem(int itemIndex);
  void setSelectedItem(const String& firstText);

  void setDropDownVisibile(bool visible);
  bool isDropDownVisible();
  Rect getDropDownBounds();

  // Signals
  Signal<void(Event&)> SelChange;  ///< @see onSelChange
  Signal<void(Event&)> EditChange; ///< @see onEditChange

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onLayout(LayoutEvent& ev);

  // New events
  virtual void onSelChange(Event& ev);
  virtual void onEditChange(Event& ev);

  // Reflected notifications
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);

private:
  int getHeightForAllItems();
  void updateMaxItemSize(const String& text);

};

} // namespace vaca

#endif // VACA_COMBOBOX_H
