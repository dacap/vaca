// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#ifndef VACA_TAB_HPP
#define VACA_TAB_HPP

#include "Vaca/Widget.hpp"
#include "Vaca/Font.hpp"
#include "Vaca/WidgetClass.hpp"
#include "Vaca/Register.hpp"
#include "Vaca/Panel.hpp"

namespace Vaca {

class TabBase;
class Tab;
class TabPage;

#define TabBaseStyle		ChildStyle
#define TabStyle		TabBaseStyle
#define TabPageStyle		PanelStyle

//////////////////////////////////////////////////////////////////////
// TabBase

/**
 * Widget that contains tabs to switch between them. This class
 * doesn't controls pages automatically, it's useful when you want a
 * special behaviour for the tabs (see Tabs example). If you want
 * to put one Panel in the each page, and swap them when the user
 * change the pages, you should use the Tab widget.
 *
 * @see Tab
 */
class VACA_DLL TabBase : public Widget
{
  Font* m_userFont;
  Font* m_tabFont;

public:

  TabBase(Widget* parent, Style style = TabBaseStyle);
  virtual ~TabBase();

  virtual Font* getFont();
  virtual void setFont(Font* font);

  virtual Rect getLayoutBounds();

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

  Tab(Widget* parent, Style style = TabStyle);
  virtual ~Tab();

  TabPage* getPage(int pageIndex);

protected:

  virtual void onPageChange(Event& ev);

};

//////////////////////////////////////////////////////////////////////
// TabPage

class TabPageClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName("Vaca.TabPage"); }
};

/**
 * A page for an automatic Tab. It's like a Panel.
 */
class VACA_DLL TabPage : public Register<TabPageClass>, public Panel
{
public:

  TabPage(const String& text, Tab* parent, Style style = TabPageStyle);
  virtual ~TabPage();
  
};

} // namespace Vaca

#endif
