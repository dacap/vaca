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

#ifndef VACA_LISTVIEW_H
#define VACA_LISTVIEW_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"
#include "Vaca/CancelableEvent.h"
#include "Vaca/ImageList.h"

#include <vector>

namespace Vaca {

//////////////////////////////////////////////////////////////////////

/// @see ListViewType
/// 
struct ListViewTypeEnum
{
  enum enumeration {
    Icon,
    Report,
    SmallIcon,
    List
  };
  static const enumeration default_value = List;
};

/// 
typedef Enum<ListViewTypeEnum> ListViewType;

//////////////////////////////////////////////////////////////////////

typedef std::vector<ListItem*> ListItemList;
typedef std::vector<ListColumn*> ListColumnList;

/// A ListView control.
/// 
class VACA_DLL ListView : public Widget 
{
  ListItemList m_items;
  ListColumnList m_columns;
  ImageList m_imageList;
  ImageList m_smallImageList;
  ImageList m_stateImageList;

  /// To use LPSTR_TEXTCALLBACK we need some space
  /// to allocate text temporally.
  String m_tmpBuffer;

public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style SingleSelection;
    static const Style ShowSelectionAlways;
  };

  ListView(Widget* parent, Style style = Styles::Default);
  virtual ~ListView();

  virtual void setBgColor(const Color& color);

  Color getTextColor() const;
  void setTextColor(const Color& color);

  Color getTextBgColor() const;
  void setTextBgColor(const Color& color);

  ListViewType getType() const;
  void setType(ListViewType type);

  void setImageList(const ImageList& imageList);
  void setSmallImageList(const ImageList& imageList);
  void setStateImageList(const ImageList& imageList);
  ImageList getImageList() const;
  ImageList getSmallImageList() const;
  ImageList getStateImageList() const;

  int addColumn(ListColumn* column);
  // int insertColumn(int columnIndex, const String& header, TextAlign textAlign = TextAlign::Left);
  void removeColumn(ListColumn* column);
  void removeAllColumns();

  int getColumnCount() const;
  ListColumn* getColumn(int columnIndex) const;

  int addItem(ListItem* item);
  // int insertItem(int itemIndex, const String& text, int imageIndex = -1);
  void removeItem(ListItem* item);
  void removeAllItems();

  int getItemCount() const;
  ListItem* getItem(int index) const;
  // Rect getItemBounds(int itemIndex, int code = LVIR_BOUNDS);

  // void editItemText(int itemIndex);

  int getSelectedItemCount() const;

//   void sortItems(std::less<ListItem> functor);
//   int getCurrentItem();

  // Signals
  Signal1<void, ListViewEvent&> BeforeSelect;
  Signal1<void, ListViewEvent&> AfterSelect;
  Signal1<void, ListViewEvent&> ColumnClick;

protected:
  // Events
  virtual void onResize(ResizeEvent& ev);

  // New events
  virtual void onBeforeSelect(ListViewEvent& ev);
  virtual void onAfterSelect(ListViewEvent& ev);
  virtual void onColumnClick(ListViewEvent& ev);

  // Reflected notifications
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

private:

  void insertDummyColumn();
  void removeDummyColumn();

};

/// Event where interact a ListView.
/// 
class ListViewEvent : public CancelableEvent
{
  ListItem* m_item;
  ListColumn* m_column;

public:

  ListViewEvent(ListView* listView, ListItem* item, ListColumn* column)
    : CancelableEvent(listView)
    , m_item(item)
    , m_column(column)
  {
  }

  ListItem* getItem() const { return m_item; }
  ListColumn* getColumn() const { return m_column; }

};

} // namespace Vaca

#endif // VACA_LISTVIEW_H
