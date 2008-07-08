// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#ifndef VACA_LISTBOX_HPP
#define VACA_LISTBOX_HPP

#include "Vaca/base.hpp"
#include "Vaca/Widget.hpp"

#include <vector>

namespace Vaca {

/**
 * Default style for ListBox: a visible child (ChildVisible), with tab
 * stop (FocusableStyle), with client-edges (ClientEdgeStyle), and
 * scroll bars (ScrollStyle), and LBS_NOINTEGRALHEIGHT | LBS_NOTIFY.
 */
#define ListBoxStyle		(ChildStyle +			\
				 FocusableStyle +		\
				 ClientEdgeStyle +		\
				 ScrollStyle +			\
				 Style(LBS_NOINTEGRALHEIGHT |	\
				       LBS_NOTIFY, 0))

#define DragListBoxStyle	(ListBoxStyle)

/**
 * List box control. Widget to manage a list of strings.
 */
class VACA_DLL ListBox : public Widget
{
public:

  ListBox(Widget* parent, Style style = ListBoxStyle);
  virtual ~ListBox();

  int addItem(const String& text);
  void insertItem(int itemIndex, const String& text);
  void removeItem(int itemIndex);

  int getItemCount();
  Rect getItemBounds(int itemIndex);

  String getItemText(int itemIndex);
  void setItemText(int itemIndex, const String& text);

  int getCurrentItem();
  void setCurrentItem(int itemIndex);

  std::vector<int> getSelectedItems();

  // signals
  Signal1<void, Event&> Action;	   ///< @see onAction
  Signal1<void, Event&> SelChange; ///< @see onSelChange

protected:
  // events
  virtual void onPreferredSize(Size& sz);

  // new events
  virtual void onAction(Event& ev);
  virtual void onSelChange(Event& ev);

  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);

};

class VACA_DLL DragListBox : public ListBox
{
public:

  DragListBox(Widget* parent, Style style = DragListBoxStyle);
  
};

} // namespace Vaca

#endif
