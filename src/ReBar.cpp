// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, 2009, Jie Zhang, David Capello
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


#include "Vaca/ReBar.h"
#include "Vaca/Event.h"
#include "Vaca/ImageList.h"
#include "Vaca/WidgetClass.h"
#include "Vaca/LayoutEvent.h"
#include "Vaca/PreferredSizeEvent.h"
#include "Vaca/win32.h"

// Warning:
//   You must define WINVER & _WIN32_WINNT version, otherwise,
//   addBand will raise an exception!!!
#ifndef WINVER
#error You have to define WINVER to compile ReBar
#endif
#ifndef _WIN32_WINNT
#error You have to define _WIN32_WINNT to compile ReBar
#endif
#ifndef _WIN32_IE
#error You have to define _WIN32_IE to compile ReBar
#endif

using namespace Vaca;

// ======================================================================
// ReBarBand class

ReBarBand::ReBarBand(ReBar* parent, int index)
  : m_rebar(parent)
  , m_index(index)
{
}

ReBarBand::ReBarBand(const ReBarBand& band)
{
  m_rebar = band.m_rebar;
  m_index = band.m_index;
}

ReBarBand::~ReBarBand()
{
}

ReBarBandStyle ReBarBand::getStyle() const
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_STYLE;

  getBand(&rbbi);

  return ReBarBandStyle(rbbi.fStyle);
}

void ReBarBand::setStyle(ReBarBandStyle style)
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_STYLE;
  rbbi.fStyle = style;

  setBand(&rbbi);
}

void ReBarBand::getColors(Color& fg, Color& bg) const
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_COLORS;
	
  getBand(&rbbi);

  fg = convert_to<Color>(rbbi.clrFore);
  bg = convert_to<Color>(rbbi.clrBack);
}

void ReBarBand::setColors(Color fg, Color bg)
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_COLORS;

  rbbi.clrBack = convert_to<COLORREF>(bg);
  rbbi.clrFore = convert_to<COLORREF>(fg);

  setBand(&rbbi);
}

String ReBarBand::getText() const
{
  REBARBANDINFO rbbi;
  String buf;
  buf.reserve(1024);

  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_TEXT;
  rbbi.lpText = &buf[0];
  rbbi.cch    = buf.capacity();

  getBand(&rbbi);

  return buf;
}

void ReBarBand::setText(const String& text)
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_TEXT;
  rbbi.cch    = text.length();
  rbbi.lpText = (LPTSTR)text.c_str();

  setBand(&rbbi);
}

int ReBarBand::getImageIndex() const
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_IMAGE;

  getBand(&rbbi);

  return rbbi.iImage;
}

void ReBarBand::setImageIndex(int index)
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_IMAGE;
  rbbi.iImage = index;

  setBand(&rbbi);
}

Widget* ReBarBand::getChild() const
{
  REBARBANDINFO rbbi;
  rbbi.cbSize = sizeof(REBARBANDINFO);
  rbbi.fMask  = RBBIM_CHILD;
	
  getBand(&rbbi);

  return Widget::fromHandle(rbbi.hwndChild);
}

void ReBarBand::setChild(Widget* widget)
{
  assert(widget != NULL);

  Size sz = widget->getPreferredSize();

  REBARBANDINFO rbbi;
  rbbi.cbSize     = sizeof(REBARBANDINFO);
  rbbi.fMask      = RBBIM_CHILD | RBBIM_CHILDSIZE;
  rbbi.hwndChild  = widget->getHandle();
  rbbi.cx         = sz.w;
  rbbi.cxMinChild = sz.w;
  rbbi.cyMinChild = sz.h;

  setBand(&rbbi);
}

void ReBarBand::setBand(REBARBANDINFO* rbbi)
{
  m_rebar->sendMessage(RB_SETBANDINFO, (WPARAM)m_index, (LPARAM)rbbi);
}

void ReBarBand::getBand(REBARBANDINFO* rbbi) const
{
  m_rebar->sendMessage(RB_GETBANDINFO, (WPARAM)m_index, (LPARAM)rbbi);
}

// ======================================================================

ReBar::ReBar(Widget* parent, Style style)
  : Widget(WidgetClassName(REBARCLASSNAME), parent, style)
{
  REBARINFO rbi;
  rbi.cbSize = sizeof(REBARINFO);
  rbi.fMask  = 0;

  // set parent window, send notification to it
  sendMessage(RB_SETPARENT, (WPARAM)parent->getHandle(), (LPARAM)0);

  if (!sendMessage(RB_SETBARINFO, (WPARAM)0, (LPARAM)&rbi))
    throw ReBarException(L"ReBar Error: Failed to initialize rebar!");
}

ReBar::~ReBar()
{
  // TODO remove children in bands
}

bool ReBar::isLayoutFree() const
{
  return true;
}

void ReBar::setImageList(const ImageList& imageList)
{
  REBARINFO rbi;
  rbi.cbSize = sizeof(REBARINFO);
  rbi.fMask  = RBIM_IMAGELIST;
  rbi.himl   = imageList.getHandle();

  if (!sendMessage(RB_SETBARINFO, (WPARAM)0, (LPARAM)&rbi))
    throw ReBarException(L"ReBar Error: Failed to initialize rebar!");

  // make a reference to the image list (remember that
  // ImageLists are shared pointers)
  m_imageList = imageList;
}

int ReBar::getBandCount()
{
  return (int)sendMessage(RB_GETBANDCOUNT, (WPARAM)0, (LPARAM)0);
}

int ReBar::getBarHeight()
{
  return (int)sendMessage(RB_GETBARHEIGHT, (WPARAM)0, (LPARAM)0);
}

ReBarBand ReBar::addBand(Widget* item, ReBarBandStyle style, int position)
{
  Size sz = item->getPreferredSize();

  REBARBANDINFO rbbi;
  rbbi.cbSize     = sizeof(REBARBANDINFO);
  rbbi.fMask      = RBBIM_STYLE | RBBIM_CHILD | RBBIM_SIZE | RBBIM_CHILDSIZE;
  rbbi.fStyle     = style;
  rbbi.hwndChild  = item->getHandle();
  rbbi.cx         = sz.w;
  rbbi.cxMinChild = sz.w;
  rbbi.cyMinChild = sz.h;

  if (!sendMessage(RB_INSERTBAND, (WPARAM)position, (LPARAM)&rbbi))
    throw ReBarException(L"ReBar Error: Failed to append ReBar Band!");

  return ReBarBand(this, (position < 0) ? getBandCount() : position);
}

ReBarBand ReBar::getBand(int index)
{
  if (index < 0 || index >= getBandCount())
    throw ReBarException(L"ReBar Error: Invalid index of the Band!");

  return ReBarBand(this, index);
}

void ReBar::removeBand(int index)
{
  if (!sendMessage(RB_DELETEBAND, (WPARAM)index, (LPARAM)0))
    throw ReBarException(L"ReBar Error: Failed to delete ReBar Band!");
}

Color ReBar::getBgColor()
{
  return convert_to<Color>((COLORREF)sendMessage(RB_GETBKCOLOR, (WPARAM)0, (LPARAM)0));
}

Color ReBar::getFgColor()
{
  return convert_to<Color>((COLORREF)sendMessage(RB_GETTEXTCOLOR, (WPARAM)0, (LPARAM)0));
}

void ReBar::setBgColor(const Color& color)
{
  sendMessage(RB_SETBKCOLOR, (WPARAM)0, (LPARAM)convert_to<COLORREF>(color));
}

void ReBar::setFgColor(const Color& color)
{
  sendMessage(RB_SETTEXTCOLOR, (WPARAM)0, (LPARAM)convert_to<COLORREF>(color));
}

Rect ReBar::getBandRect(int index)
{
  RECT rc;
  sendMessage(RB_GETRECT, (WPARAM)index, (LPARAM)&rc);
  return Rect(&rc);
}

int ReBar::getRowCount()
{
  return (int)sendMessage(RB_GETROWCOUNT, (WPARAM)0, (LPARAM)0);
}

int ReBar::getRowHeight(int iRow)
{
  return (int)sendMessage(RB_GETROWCOUNT, (WPARAM)iRow, (LPARAM)0);
}

void ReBar::showBand(int index, bool show)
{
  sendMessage(RB_SHOWBAND, (WPARAM)index, (LPARAM)show);
}

void ReBar::maximizeBand(int index, bool ideal)
{
  sendMessage(RB_MAXIMIZEBAND, (WPARAM)index, (LPARAM)false);
}

void ReBar::minimizeBand(int index)
{
  sendMessage(RB_MINIMIZEBAND, (WPARAM)index, (LPARAM)0);
}

void ReBar::onPreferredSize(PreferredSizeEvent& ev)
{
  ev.setPreferredSize(0, getBarHeight());
}

void ReBar::onLayout(LayoutEvent& ev)
{
  Rect rc = ev.getBounds();
  Size pref = getPreferredSize();

  // set the size of the rebar (top of parent)
  setBounds(Rect(rc.x, rc.y, rc.w, pref.h));

  // remove the top part of the rectangle for other parent's children
  rc.y += pref.h;
  rc.h -= pref.h;
  ev.setBounds(rc);

  Widget::onLayout(ev);
}

bool ReBar::onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult)
{
  if (Widget::onReflectedNotify(lpnmhdr, lResult))
    return true;

  switch (lpnmhdr->code) {

    case RBN_AUTOSIZE:
      Event ev(this);
      onAutoSize(ev);
      return true;

  }

  return false;
}

void ReBar::onAutoSize(Event& ev)
{
  // Relayout parent
  if (getParent())
    getParent()->layout();

  AutoSize(ev);
}
