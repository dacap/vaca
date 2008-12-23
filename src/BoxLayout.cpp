// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

#include "Vaca/BoxLayout.h"
#include "Vaca/BoxConstraint.h"
#include "Vaca/Size.h"
#include "Vaca/Debug.h"

using namespace Vaca;

// auxiliar function to known if a widget is expansive
static bool WidgetIsExpansive(Widget* widget) 
{
  Constraint* constraint = widget->getConstraint();
  if (constraint == NULL)
    return false;

  BoxConstraint* boxConstraint =
    dynamic_cast<BoxConstraint*>(constraint);
  assert(boxConstraint != NULL);
  return boxConstraint->isExpansive();
}

//////////////////////////////////////////////////////////////////////
// BoxLayout

BoxLayout::BoxLayout(Orientation orientation,
		     bool homogeneous,
		     int borderSize,
		     int childSpacing)
{
  m_orientation = orientation;
  m_homogeneous = homogeneous;
  m_border = borderSize;
  m_childSpacing = childSpacing;
}

bool BoxLayout::isHorizontal()
{
  return m_orientation == Orientation::Horizontal;
}

bool BoxLayout::isVertical()
{
  return m_orientation == Orientation::Vertical;
}

bool BoxLayout::isHomogeneous()
{
  return m_homogeneous;
}

int BoxLayout::getBorder()
{
  return m_border;
}

void BoxLayout::setBorder(int border)
{
  m_border = border;
}

int BoxLayout::getChildSpacing()
{
  return m_childSpacing;
}

void BoxLayout::setChildSpacing(int childSpacing)
{
  m_childSpacing = childSpacing;
}

Size BoxLayout::getPreferredSize(Widget* parent, Widget::Container& widgets, const Size& fitIn)
{
#define GET_CHILD_SIZE(w, h)			\
  {						\
    if (isHomogeneous())			\
      sz.w = max_value(sz.w, pref.w);		\
    else					\
      sz.w += pref.w;				\
    sz.h = max_value(sz.h, pref.h);		\
  }

#define FINAL_SIZE(w)				\
  {						\
    if (isHomogeneous())			\
      sz.w *= childCount;			\
    sz.w += m_childSpacing * (childCount-1);	\
  }

  int childCount = 0;
  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget* widget = *it;
    if (!widget->isLayoutFree())
      childCount++;
  }

  Size sz(0, 0);
  Size _fitIn(max_value(0, fitIn.w-m_border*2),
	      max_value(0, fitIn.h-m_border*2));

  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget* widget = *it;

    if (widget->isLayoutFree())
      continue;

    Size pref = widget->getPreferredSize(_fitIn);

    if (isHorizontal()) {
      GET_CHILD_SIZE(w, h);
    }
    else {
      GET_CHILD_SIZE(h, w);
    }
  }

  if (childCount > 0) {
    if (isHorizontal()) {
      FINAL_SIZE(w);
    }
    else {
      FINAL_SIZE(h);
    }
  }

  sz.w += m_border*2;
  sz.h += m_border*2;

  // VACA_TRACE("BoxLayout::getPreferredSize(%d, %d);\n", sz.w, sz.h);

  return sz;
}

void BoxLayout::layout(Widget* parent, Widget::Container& widgets, const Rect& rc)
{
#define FIXUP(x, y, w, h)						\
  {									\
    if (childCount > 0) {						\
      x = rc.x+m_border;						\
      y = rc.y+m_border;						\
      h = max_value(1, rc.h - m_border*2);				\
									\
      if (isHomogeneous()) {						\
	width = (rc.w							\
		 - m_border*2						\
		 - m_childSpacing * (childCount - 1));			\
	extra = width / childCount;					\
      }									\
      else if (expandCount > 0) {					\
	width = rc.w - pref.w;						\
									\
	for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) { \
	  Widget* widget = *it;						\
	  if (widget->isLayoutFree())					\
	    continue;							\
									\
	  if (!WidgetIsExpansive(widget)) {				\
	    Size fitIn;							\
	    fitIn.w = 0;						\
	    fitIn.h = h;						\
	    pref = widget->getPreferredSize(fitIn);			\
	    prefDiff = widget->getPreferredSize();			\
	    width -= pref.w - prefDiff.w;				\
	  }								\
	}								\
									\
	extra = width / expandCount;					\
      }									\
      else {								\
	width = 0;							\
	extra = 0;							\
      }									\
									\
      for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) { \
	Widget* widget = *it;						\
									\
	if (widget->isLayoutFree())					\
	  continue;							\
									\
	if (isHomogeneous()) {						\
	  if (childCount == 1)						\
	    child_width = width;					\
	  else								\
	    child_width = extra;					\
									\
	  childCount -= 1;						\
	  width -= extra;						\
	}								\
	else {								\
	  Size fitIn;							\
	  fitIn.w = 0;							\
	  fitIn.h = h;							\
	  pref = widget->getPreferredSize(fitIn);			\
									\
	  child_width = pref.w;						\
									\
	  if (WidgetIsExpansive(widget)) {				\
	    prefDiff = widget->getPreferredSize();			\
	    child_width -= pref.w - prefDiff.w;				\
									\
	    if (expandCount == 1)					\
	      child_width += width;					\
	    else							\
	      child_width += extra;					\
									\
	    expandCount -= 1;						\
	    width -= extra;						\
	  }								\
	}								\
									\
	w = max_value(1, child_width);					\
									\
	if (isHorizontal())						\
	  cpos = Rect(x, y, w, h);					\
	else								\
	  cpos = Rect(y, x, h, w);					\
									\
	moveWidget(widget, cpos);					\
	x += child_width + m_childSpacing;				\
      }									\
    }									\
  }

  int childCount = 0;
  int expandCount = 0;

  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget* widget = *it;
    if (!widget->isLayoutFree()) {
      childCount++;
      if (WidgetIsExpansive(widget))
	expandCount++;
    }
  }

  Rect cpos;
  Size pref, prefDiff;
  int extra, width, child_width;
  int x, y, w, h;

  beginMovement(widgets);

  pref = getPreferredSize(parent, widgets, Size(0, 0)); // fitIn doesn't matter
//   pref = preferredSize(parent, widgets,
// 		       isHorizontal() ? Size(0, max_value(0, rc.h-m_border)):
// 					Size(max_value(0, rc.w-m_border), 0));

  if (isHorizontal()) {
    FIXUP(x, y, w, h);
  }
  else {
    FIXUP(y, x, h, w);
  }

  endMovement();
}
