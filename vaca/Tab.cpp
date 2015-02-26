// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Tab.h"
#include "vaca/Font.h"
#include "vaca/Point.h"
#include "vaca/Debug.h"
#include "vaca/Event.h"
#include "vaca/ClientLayout.h"
#include "vaca/PreferredSizeEvent.h"
#include "vaca/LayoutEvent.h"
#include "vaca/win32.h"

using namespace vaca;

// ======================================================================
// TabBase

/**
   Creates a new tab control. A tab is a set of pages, each page has a
   group of widgets, so you can navigate through the pages.
*/
TabBase::TabBase(Widget* parent, Style style)
  : Widget(WidgetClassName(WC_TABCONTROL), parent, style)
{
  m_userFont = getFont();
  m_tabFont = getFont();
}

TabBase::~TabBase()
{
}

Font TabBase::getFont() const
{
  return m_userFont;
}

void TabBase::setFont(Font font)
{
  m_userFont = font;
  updateFont();
}

void TabBase::updateFont()
{
  LOGFONT lf;
  if (getFont().getLogFont(&lf)) {
    int style = getStyle().regular;

    if ((style & TCS_VERTICAL) != 0) {
      if ((style & TCS_RIGHT) != 0) {
	lf.lfEscapement = 2700;
	lf.lfOrientation = 2700;
      }
      else {
	lf.lfEscapement = 900;
	lf.lfOrientation = 900;
      }
    }
    else {
      lf.lfEscapement = 0;
      lf.lfOrientation = 0;
    }

    m_tabFont = Font(&lf);
    Widget::setFont(m_tabFont);
  }

  // layout children
  layout();
}

Side TabBase::getSide()
{
  int style = getStyle().regular;

  if ((style & TCS_VERTICAL) != 0) {
    if ((style & TCS_RIGHT) != 0)
      return Side::Right;
    else
      return Side::Left;
  }
  else {
    if ((style & TCS_BOTTOM) != 0)
      return Side::Bottom;
    else
      return Side::Top;
  }
}

/**
   Sets the side of the pages.

   @warning You can't use LeftSide and RightSide sides in a Tab
	    with isMultiline() == false. This routine will set the Tab
	    in a multiline style automatically if you try this.

   @see #setMultiline
*/
void TabBase::setSide(Side side)
{
  int style = 0;

  removeStyle(Style(TCS_VERTICAL | TCS_RIGHT | TCS_BOTTOM, 0));

  switch (side) {
    case Side::Top:    style = 0; break;
    case Side::Bottom: style = TCS_BOTTOM; break;
    // TCS_VERTICAL needs TCS_MULTILINE
    case Side::Left:   style = TCS_VERTICAL | TCS_MULTILINE; break;
    case Side::Right:  style = TCS_VERTICAL | TCS_RIGHT | TCS_MULTILINE; break;
  }

  addStyle(Style(style, 0));
  updateFont();
}

bool TabBase::isMultiline()
{
  return (getStyle().regular & TCS_MULTILINE) != 0;
}

void TabBase::setMultiline(bool state)
{
  int style = getStyle().regular;

  // is already in the specified state?
  if (((style & TCS_MULTILINE) != 0) == state)
    return;

  // we can't use TCS_MULTILINE if we are using TCS_VERTICAL
  if (!state && ((style & TCS_VERTICAL) != 0))
    return;

  // set TCS_MULTILINE style
  if (state)
    addStyle(Style(TCS_MULTILINE, 0));
  else
    removeStyle(Style(TCS_MULTILINE, 0));

  // layout children
  layout();
}

int TabBase::addPage(const String& text)
{
  return insertPage(-1, text);
}

int TabBase::insertPage(int pageIndex, const String& text)
{
  assert(::IsWindow(getHandle()));

  TCITEM tci;

  tci.mask = TCIF_TEXT;
  tci.pszText = (Char*)text.c_str();
  tci.cchTextMax = text.size();

  if (pageIndex < 0)
    pageIndex = getPageCount();

  return TabCtrl_InsertItem(getHandle(), pageIndex, &tci);
}

void TabBase::removePage(int pageIndex)
{
  assert(::IsWindow(getHandle()));
  TabCtrl_DeleteItem(getHandle(), pageIndex);
}

int TabBase::getPageCount()
{
  assert(::IsWindow(getHandle()));
  return TabCtrl_GetItemCount(getHandle());
}

int TabBase::getRowCount()
{
  assert(::IsWindow(getHandle()));
  return TabCtrl_GetRowCount(getHandle());
}

int TabBase::getActivePage()
{
  assert(::IsWindow(getHandle()));
  return TabCtrl_GetCurSel(getHandle());
}

void TabBase::setActivePage(int pageIndex)
{
  assert(::IsWindow(getHandle()));
  TabCtrl_SetCurSel(getHandle(), pageIndex);
}

String TabBase::getPageText(int pageIndex)
{
  assert(::IsWindow(getHandle()));

  TCITEM tci;

  tci.mask = TCIF_TEXT;
  tci.pszText = new Char[1024];
  tci.cchTextMax = 1024;

  if (TabCtrl_GetItem(getHandle(), pageIndex, &tci) != FALSE) {
    String text(tci.pszText);
    delete[] tci.pszText;
    return text;
  }
  else {
    delete[] tci.pszText;
    return String();
  }
}

void TabBase::setPageText(int pageIndex, const String& text)
{
  assert(::IsWindow(getHandle()));

  TCITEM tci;

  tci.mask = TCIF_TEXT;
  tci.pszText = new Char[1024];
  tci.cchTextMax = 1024;

  copy_string_to(text, tci.pszText, tci.cchTextMax);

  TabCtrl_SetItem(getHandle(), pageIndex, &tci);
}

// void TabBase::setPadding(Size padding)
// {
//   assert(::IsWindow(getHandle()));
//   TabCtrl_SetPadding(getHandle(), padding.w, padding.h);
// }

/**
   @warning This function couldn't work for Tab with getSide() != TopSide.
*/
Size TabBase::getNonClientSize()
{
  assert(::IsWindow(getHandle()));

  Rect clientRect(0, 0, 1, 1);
  RECT nonClientRect = convert_to<RECT>(clientRect);

  TabCtrl_AdjustRect(getHandle(), TRUE, &nonClientRect);

  return convert_to<Rect>(nonClientRect).getSize() - clientRect.getSize();
}

// /// TCN_SELCHANGING
// ///
// void TabBase::onPageChanging(Event& ev)
// {
//   PageChanging(ev);
// }

/**
   Event generated when the user select a new page. Use
   getActivePage() to known which page was selected.

   TCN_SELCHANGE
*/
void TabBase::onPageChange(Event& ev)
{
  PageChange(ev);
}

/**
   Adds space for the non-client size.

   @see getNonClientSize
*/
void TabBase::onPreferredSize(PreferredSizeEvent& ev)
{
  Size ncSize = getNonClientSize();

  if (ev.fitInWidth() || ev.fitInHeight()) {
    ev.setPreferredSize(max_value(0, ev.fitInWidth() - ncSize.w),
			max_value(0, ev.fitInHeight() - ncSize.h));
  }

  Widget::onPreferredSize(ev);
  ev.setPreferredSize(ev.getPreferredSize() + ncSize);
}

/**
   Adjust the area where the pages should be positioned.

   @win32
     It uses @msdn{TabCtrl_AdjustRect} to known how to adjust the bounds.
   @endwin32
*/
void TabBase::onLayout(LayoutEvent& ev)
{
  RECT rc;
  assert(::IsWindow(getHandle()));

  rc = convert_to<RECT>(ev.getBounds());
  TabCtrl_AdjustRect(getHandle(), FALSE, &rc);
  ev.setBounds(convert_to<Rect>(rc).offset(-getBounds().getOrigin()));

  // Call base implementation
  Widget::onLayout(ev);
}

bool TabBase::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  if (Widget::onReflectedNotify(lpnmhdr, lResult))
    return true;

  switch (lpnmhdr->code) {

    case TCN_SELCHANGE: {
      Event ev(this);
      onPageChange(ev);
      break;
    }

  }

  return false;
}

// ======================================================================
// Tab

Tab::Tab(Widget* parent, Style style)
  : TabBase(parent, style)
{
  setLayout(new ClientLayout);
}

Tab::~Tab()
{
}

/**
   Returns the page in the specified index.

   @param pageIndex
     Zero-based index of the page to be returned.
*/
TabPage* Tab::getPage(int pageIndex)
{
  assert(pageIndex >= 0 && pageIndex < getPageCount());

  return dynamic_cast<TabPage*>(getChildren().at(pageIndex));
}

void Tab::onPageChange(Event& ev)
{
  TabBase::onPageChange(ev);

  WidgetList pages = getChildren();
  int pageIndex = 0;
  int selectedPage = getActivePage();

  for (WidgetList::iterator
	 it = pages.begin(); it != pages.end(); ++it, ++pageIndex) {
    TabPage* page = dynamic_cast<TabPage*>(*it);

    assert(page != NULL);

    page->setVisible(pageIndex == selectedPage);
  }

  layout();
}

// ======================================================================
// TabPage

TabPage::TabPage(const String& text, Tab* parent, Style style)
  : Widget(TabPageClass::getClassName(), parent, style)
{
  setText(text);

  m_index = parent->addPage(text);

  // only the first inserted page is visible
  if (m_index > 0)
    setVisible(false);
}

TabPage::~TabPage()
{
}

String TabPage::getText() const
{
  return static_cast<Tab*>(getParent())->getPageText(m_index);
}

void TabPage::setText(const String& str)
{
  static_cast<Tab*>(getParent())->setPageText(m_index, str);
}

int TabPage::getPageIndex()
{
  return m_index;
}
