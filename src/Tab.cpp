// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

#include "stdvaca.h"
#include "Vaca/Tab.h"
#include "Vaca/Font.h"
#include "Vaca/Point.h"
#include "Vaca/Debug.h"
#include "Vaca/Event.h"
#include "Vaca/ClientLayout.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// TabBase

/**
 * Creates a new tab control. A tab is a set of pages, each page has a
 * group of widgets, so you can navigate through the pages.
 */
TabBase::TabBase(Widget *parent, Style style)
  : Widget(WC_TABCONTROL, parent, style)
{
  mUserFont = NULL;
}

TabBase::~TabBase()
{
}

Font &TabBase::getFont()
{
  return mUserFont != NULL ? *mUserFont: Widget::getFont();
}

void TabBase::setFont(Font &font)
{
  mUserFont = &font;
  updateFont();
}

/**
 * Returns the client rectangle area of the Tab, it's the area inside
 * the pages.
 *
 * @warning It isn't like Win32 GetClientRect, because here we
 *          returns the area inside the pages.  It's like to call
 *          Win32 TabCtrl_AdjustRect.
 */
Rect TabBase::getClientBounds()
{
  RECT rc;
  assert(getHWND() != NULL);

  rc = getBounds();
  TabCtrl_AdjustRect(getHWND(), FALSE, &rc);
  return Rect(&rc).offset(-getBounds().getOrigin());
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

    mTabFont.assign(&lf);
    Widget::setFont(mTabFont);
  }

  // layout children
  layout();
}

Side TabBase::getSide()
{
  int style = getStyle().regular;

  if ((style & TCS_VERTICAL) != 0) {
    if ((style & TCS_RIGHT) != 0)
      return RightSide;
    else
      return LeftSide;
  }
  else {
    if ((style & TCS_BOTTOM) != 0)
      return BottomSide;
    else
      return TopSide;
  }
}

/**
 * Sets the side of the pages.
 *
 * @warning You can't use Side::Left and Side::Right sides in a Tab
 *          with isMultiline() == false. This routine will set the Tab
 *          in a multiline style automatically if you try this.
 * 
 * @see setMultiline()
 */
void TabBase::setSide(Side side)
{
  int style = 0;

  removeStyle(Style(TCS_VERTICAL | TCS_RIGHT | TCS_BOTTOM, 0));

  switch (side) {
    case TopSide:    style = 0; break;
    case BottomSide: style = TCS_BOTTOM; break;
    // TCS_VERTICAL needs TCS_MULTILINE
    case LeftSide:   style = TCS_VERTICAL | TCS_MULTILINE; break;
    case RightSide:  style = TCS_VERTICAL | TCS_RIGHT | TCS_MULTILINE; break;
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

int TabBase::addPage(const String &text)
{
  return insertPage(-1, text);
}

int TabBase::insertPage(int pageIndex, const String &text)
{
  assert(getHWND() != NULL);

  TCITEM tci;

  tci.mask = TCIF_TEXT;
  tci.pszText = const_cast<TCHAR *>(text.c_str());
  tci.cchTextMax = text.size();

  if (pageIndex < 0)
    pageIndex = getPageCount();
  
  return TabCtrl_InsertItem(getHWND(), pageIndex, &tci);
}

void TabBase::removePage(int pageIndex)
{
  assert(getHWND() != NULL);
  TabCtrl_DeleteItem(getHWND(), pageIndex);
}

int TabBase::getPageCount()
{
  assert(getHWND() != NULL);
  return TabCtrl_GetItemCount(getHWND());
}

int TabBase::getRowCount()
{
  assert(getHWND() != NULL);
  return TabCtrl_GetRowCount(getHWND());
}

int TabBase::getActivePage()
{
  assert(getHWND() != NULL);
  return TabCtrl_GetCurSel(getHWND());
}

void TabBase::setActivePage(int pageIndex)
{
  assert(getHWND() != NULL);
  TabCtrl_SetCurSel(getHWND(), pageIndex);
}

String TabBase::getPageText(int pageIndex)
{
  assert(getHWND() != NULL);

  TCITEM tci;

  tci.mask = TCIF_TEXT;
  tci.pszText = new Character[1024];
  tci.cchTextMax = 1024;

  if (TabCtrl_GetItem(getHWND(), pageIndex, &tci) != FALSE) {
    String text(tci.pszText);
    delete tci.pszText;
    return text;
  }
  else
    return String();
}

// void TabBase::setPadding(Size padding)
// {
//   assert(getHWND() != NULL);
//   TabCtrl_SetPadding(getHWND(), padding.w, padding.h);
// }

/**
 * @warning This function couldn't work for Tab with getSide() != Side::Top.
 */
Size TabBase::getNonClientSize()
{
  assert(getHWND() != NULL);

  Rect clientRect(0, 0, 1, 1);
  RECT nonClientRect = clientRect;

  TabCtrl_AdjustRect(getHWND(), TRUE, &nonClientRect);

  return Rect(&nonClientRect).getSize() - clientRect.getSize();
}

Size TabBase::preferredSize()
{
  return getNonClientSize() + Widget::preferredSize();
}

Size TabBase::preferredSize(const Size &fitIn)
{
  Size ncSize(getNonClientSize());

  return
    ncSize +
    Widget::preferredSize(Size(VACA_MAX(0, fitIn.w - ncSize.w),
			       VACA_MAX(0, fitIn.h - ncSize.h)));
}

// /**
//  * TCN_SELCHANGING
//  */
// void TabBase::onPageChanging(Event &ev)
// {
//   PageChanging(ev);
// }

/**
 * TCN_SELCHANGE
 */
void TabBase::onPageChange(Event &ev)
{
  PageChange(ev);
}

bool TabBase::onNotify(LPNMHDR lpnmhdr, LRESULT &lResult)
{
  switch (lpnmhdr->code) {

    case TCN_SELCHANGE: {
      Event ev(this);
      onPageChange(ev);
      break;
    }

  }

  return false;
}

//////////////////////////////////////////////////////////////////////
// Tab

Tab::Tab(Widget *parent, Style style)
  : TabBase(parent, style)
{
  setLayout(new ClientLayout);
}

Tab::~Tab()
{
}

TabPage *Tab::getPage(int pageIndex)
{
  assert(pageIndex >= 0 && pageIndex < getPageCount());
  
  return dynamic_cast<TabPage *>(getChildren().at(pageIndex));
}

void Tab::onPageChange(Event &ev)
{
  TabBase::onPageChange(ev);

  Container pages = getChildren();
  int pageIndex = 0;
  int selectedPage = getActivePage();

  for (Container::iterator it = pages.begin();
       it != pages.end();
       ++it, ++pageIndex) {
    TabPage *page = dynamic_cast<TabPage *>(*it);

    assert(page != NULL);

    page->setVisible(pageIndex == selectedPage);
  }

  layout();
}

//////////////////////////////////////////////////////////////////////
// TabPage

TabPage::TabPage(const String &text, Tab *parent, Style style)
  : Panel(TabPageClass::getClassName(), parent, style)
{
  setText(text);

  int pageIndex = parent->addPage(text);

  // only the first inserted page is visible
  if (pageIndex > 0)
    setVisible(false);
}

TabPage::~TabPage()
{
}
