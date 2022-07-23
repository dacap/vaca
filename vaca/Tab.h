// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TAB_H
#define VACA_TAB_H

#include "vaca/Widget.h"
#include "vaca/Font.h"
#include "vaca/WidgetClass.h"
#include "vaca/Register.h"

namespace vaca {

// ======================================================================
// TabBase

/**
   Widget that contains tabs to switch between them. This class
   doesn't controls pages automatically, it's useful when you want a
   special behaviour for the tabs (see @c Tabs example). If you want
   to put one widget in the each page, and swap them when the user
   change the pages, you should use the Tab widget.

   @see Tab
*/
class VACA_DLL TabBase : public Widget
{
  Font m_userFont;
  Font m_tabFont;

public:

  struct VACA_DLL Styles {
    static constexpr Style Default = Widget::Styles::Default;
  };

  TabBase(Widget* parent, Style style = Styles::Default);
  virtual ~TabBase();

  virtual Font getFont() const;
  virtual void setFont(Font font);

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

  // Signals
//   Signal<void(Event&)> PageChanging;
  Signal<void(Event&)> PageChange; ///< @see onPageChange

protected:
  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onLayout(LayoutEvent& ev);

  // Reflected notifications
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);

  // New events
//   virtual void onPageChanging(Event& ev);
  virtual void onPageChange(Event& ev);

private:

  void updateFont();

};

// ======================================================================
// Tab

/**
   This class automatically controls the visibility of each page (TabPage).

   You don't need to setup a layout manager for this widget, because
   it uses the ClientLayout manager to arrange its TabPage(s).
*/
class VACA_DLL Tab : public TabBase
{
public:

  struct VACA_DLL Styles {
    static constexpr Style Default = TabBase::Styles::Default;
  };

  Tab(Widget* parent, Style style = Styles::Default);
  virtual ~Tab();

  TabPage* getPage(int pageIndex);

protected:

  virtual void onPageChange(Event& ev);

};

/**
   Represents the Win32 class used by TabPage.
*/
class TabPageClass : public WidgetClass
{
public:
  static WidgetClassName getClassName()
  { return WidgetClassName(L"Vaca.TabPage"); }
};

/**
   A page for an automatic Tab.
*/
class VACA_DLL TabPage : public Register<TabPageClass>, public Widget
{
  int m_index;

public:

  struct VACA_DLL Styles {
    static constexpr Style Default = Widget::Styles::Default;
  };

  TabPage(const String& text, Tab* parent, Style style = Styles::Default);
  virtual ~TabPage();

  virtual String getText() const;
  virtual void setText(const String& str);

  int getPageIndex();

};

} // namespace vaca

#endif // VACA_TAB_H
