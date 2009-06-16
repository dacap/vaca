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

/// A ListView control.
/// 
class VACA_DLL ListView : public Widget 
{
  ImageList m_imageList;
  ImageList m_smallImageList;
  ImageList m_stateImageList;

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

  int addColumn(const String& header, TextAlign textAlign = TextAlign::Left);
  int insertColumn(int columnIndex, const String& header, TextAlign textAlign = TextAlign::Left);
  void removeColumn(int columnIndex);

  int getColumnCount();
  Rect getColumnBounds(int columnIndex);

  int getColumnWidth(int columnIndex);
  void setColumnWidth(int columnIndex, int width);
  void setPreferredColumnWidth(int columnIndex, bool useHeader = true);

  int addItem(const String& text, int imageIndex = -1);
  int insertItem(int itemIndex, const String& text, int imageIndex = -1);
  void removeItem(int itemIndex);
  void removeAllItems();

  int getItemCount() const;
  Rect getItemBounds(int itemIndex, int code = LVIR_BOUNDS);

  String getItemText(int itemIndex, int columnIndex = 0);
  void setItemText(int itemIndex, const String& text, int columnIndex = 0);
  void editItemText(int itemIndex);

  int getSelectedItemCount() const;
  bool isItemSelected(int itemIndex);
  void setItemSelected(int itemIndex, bool state);

  void ensureVisible(int itemIndex);

//   void sortItems(std::less<ListItem> functor);

//   int getCurrentItem();

  // signals
  Signal1<void, ListViewEvent&> BeforeSelect;
  Signal1<void, ListViewEvent&> AfterSelect;
  Signal1<void, ListViewEvent&> ColumnClick;

protected:
  // new events
  virtual void onBeforeSelect(ListViewEvent& ev);
  virtual void onAfterSelect(ListViewEvent& ev);
  virtual void onColumnClick(ListViewEvent& ev);

  // reflection
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

private:

  void insertDummyColumn();
  void removeDummyColumn();

};

/// Event where interact a ListView.
/// 
class ListViewEvent : public CancelableEvent
{
  int mItemIndex;
  int mColumnIndex;

public:

  ListViewEvent(ListView* listView, int itemIndex, int columnIndex)
    : CancelableEvent(listView)
    , mItemIndex(itemIndex)
    , mColumnIndex(columnIndex)
  {
  }

  int getItemIndex() { return mItemIndex; }
  int getColumnIndex() { return mColumnIndex; }

};

} // namespace Vaca

#endif // VACA_LISTVIEW_H
