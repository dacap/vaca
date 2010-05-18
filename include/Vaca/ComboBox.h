// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#ifndef VACA_COMBOBOX_H
#define VACA_COMBOBOX_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

#include <vector>

namespace Vaca {

/**
   Combo box control. Widget to select an option from a drop down
   list.
*/
class VACA_DLL ComboBox : public Widget
{
  Size m_maxItemSize;

public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style Editable;
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
  Signal1<void, Event&> SelChange;  ///< @see onSelChange
  Signal1<void, Event&> EditChange; ///< @see onEditChange

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

} // namespace Vaca

#endif // VACA_COMBOBOX_H
