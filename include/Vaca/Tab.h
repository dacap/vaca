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

#ifndef VACA_TAB_H
#define VACA_TAB_H

#include "Vaca/Widget.h"
#include "Vaca/Font.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/Register.h"

namespace Vaca {

//////////////////////////////////////////////////////////////////////
// TabBase

/**
 * Widget that contains tabs to switch between them. This class
 * doesn't controls pages automatically, it's useful when you want a
 * special behaviour for the tabs (see @c Tabs example). If you want
 * to put one widget in the each page, and swap them when the user
 * change the pages, you should use the Tab widget.
 *
 * @see Tab
 */
class VACA_DLL TabBase : public Widget
{
  Font m_userFont;
  Font m_tabFont;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  TabBase(Widget* parent, Style style = Styles::Default);
  virtual ~TabBase();

  virtual Font getFont() const;
  virtual void setFont(Font font);

  virtual Rect getLayoutBounds() const;

  Side getSide();
  void setSide(Side side);

  bool isMultiline();
  void setMultiline(bool state);

  int addPage(const String& text);
  int insertPage(int pageIndex, const String& text);
  void removePage(int pageIndex);

  int getPageCount();
  int getRowCount();

  int getActivePage();
  void setActivePage(int pageIndex);

  String getPageText(int pageIndex);
  void setPageText(int pageIndex, const String& text);

//   void setPadding(Size padding);

  Size getNonClientSize();

  // signals
//   Signal1<void, Event&> PageChanging;
  Signal1<void, Event&> PageChange; ///< @see onPageChange

protected:
  // events
  virtual void onPreferredSize(Size& sz);
  
  // reflection
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

  // new events
//   virtual void onPageChanging(Event& ev);
  virtual void onPageChange(Event& ev);

private:

  void updateFont();

};

//////////////////////////////////////////////////////////////////////
// Tab

/**
 * This class automatically controls the visibility of each page (TabPage). 
 *
 * You don't need to setup a layout manager for this widget, because
 * it uses the ClientLayout manager to arrange its TabPage(s).
 */
class VACA_DLL Tab : public TabBase
{
public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  Tab(Widget* parent, Style style = Styles::Default);
  virtual ~Tab();

  TabPage* getPage(int pageIndex);

protected:

  virtual void onPageChange(Event& ev);

};

/**
 * Represents the Win32 class used by TabPage.
 */
class TabPageClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.TabPage"); }
};

/**
 * A page for an automatic Tab.
 */
class VACA_DLL TabPage : public Register<TabPageClass>, public Widget
{
  int m_index;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  TabPage(const String& text, Tab* parent, Style style = Styles::Default);
  virtual ~TabPage();

  virtual String getText() const;
  virtual void setText(const String& str);

  int getPageIndex();
  
};

} // namespace Vaca

#endif // VACA_TAB_H
