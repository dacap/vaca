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
#include "Vaca/BoxLayout.h"
#include "Vaca/BoxConstraint.h"
#include "Vaca/Size.h"
#include "Vaca/Debug.h"

using namespace Vaca;

// auxiliar function to known if a widget is expansive
static bool WidgetIsExpansive(Widget *widget) 
{
  Constraint *constraint = widget->getConstraint();
  if (constraint == NULL)
    return false;

  BoxConstraint *boxConstraint =
    dynamic_cast<BoxConstraint *>(constraint);
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
  mOrientation = orientation;
  mHomogeneous = homogeneous;
  mBorder = borderSize;
  mChildSpacing = childSpacing;
}

bool BoxLayout::isHorizontal()
{
  return mOrientation == Horizontal;
}

bool BoxLayout::isVertical()
{
  return mOrientation == Vertical;
}

bool BoxLayout::isHomogeneous()
{
  return mHomogeneous;
}

Size BoxLayout::preferredSize(Widget *parent, Widget::Container &widgets, const Size &fitIn)
{
#define GET_CHILD_SIZE(w, h)			\
  {						\
    if (isHomogeneous())			\
      sz.w = VACA_MAX(sz.w, pref.w);		\
    else					\
      sz.w += pref.w;				\
    sz.h = VACA_MAX(sz.h, pref.h);		\
  }

#define FINAL_SIZE(w)				\
  {						\
    if (isHomogeneous())			\
      sz.w *= childCount;			\
    sz.w += mChildSpacing * (childCount-1);	\
  }

  int childCount = 0;
  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget *widget = *it;
    if (!widget->isLayoutFree())
      childCount++;
  }

  Size sz(0, 0);
  Size _fitIn(VACA_MAX(0, fitIn.w-mBorder*2),
	      VACA_MAX(0, fitIn.h-mBorder*2));

  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget *widget = *it;

    if (widget->isLayoutFree())
      continue;

    Size pref = widget->preferredSize(_fitIn);

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

  sz.w += mBorder*2;
  sz.h += mBorder*2;

  // VACA_TRACE("BoxLayout::preferredSize(%d, %d);\n", sz.w, sz.h);

  return sz;
}

void BoxLayout::layout(Widget *parent, Widget::Container &widgets, const Rect &rc)
{
#define FIXUP(x, y, w, h)						\
  {									\
    if (childCount > 0) {						\
      x = rc.x+mBorder;							\
      y = rc.y+mBorder;							\
      h = VACA_MAX(1, rc.h - mBorder*2);				\
									\
      if (isHomogeneous()) {						\
	width = (rc.w							\
		 - mBorder*2						\
		 - mChildSpacing * (childCount - 1));			\
	extra = width / childCount;					\
      }									\
      else if (expandCount > 0) {					\
	width = rc.w - pref.w;						\
									\
	for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) { \
	  Widget *widget = *it;						\
	  if (widget->isLayoutFree())					\
	    continue;							\
									\
	  if (!WidgetIsExpansive(widget)) {				\
	    Size fitIn;							\
	    fitIn.w = 0;						\
	    fitIn.h = h;						\
	    pref = widget->preferredSize(fitIn);			\
	    prefDiff = widget->preferredSize();				\
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
	Widget *widget = *it;						\
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
	  pref = widget->preferredSize(fitIn);				\
									\
	  child_width = pref.w;/* + child->padding * 2; */		\
									\
	  if (WidgetIsExpansive(widget)) {				\
	    prefDiff = widget->preferredSize();				\
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
	w = VACA_MAX(1, child_width/*  - child->padding * 2 */);	\
									\
	if (isHorizontal())						\
	  cpos = Rect(x, y, w, h);					\
	else								\
	  cpos = Rect(y, x, h, w);					\
									\
	moveWidget(widget, cpos);					\
	/* x = x + child->padding; */					\
	x += child_width + mChildSpacing;				\
      }									\
    }									\
  }

  int childCount = 0;
  int expandCount = 0;

  for (Widget::Container::iterator it=widgets.begin(); it!=widgets.end(); ++it) {
    Widget *widget = *it;
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

  pref = preferredSize(parent, widgets, Size(0, 0)); // fitIn doesn't matter
//   pref = preferredSize(parent, widgets,
// 		       isHorizontal() ? Size(0, VACA_MAX(0, rc.h-mBorder)):
// 					Size(VACA_MAX(0, rc.w-mBorder), 0));

  if (isHorizontal()) {
    FIXUP(x, y, w, h);
  }
  else {
    FIXUP(y, x, h, w);
  }

  endMovement();
}
