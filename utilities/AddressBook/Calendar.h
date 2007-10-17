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

#ifndef ADDRESSBOOK_CALENDAR_H
#define ADDRESSBOOK_CALENDAR_H

#include "Date.h"

using namespace Vaca;

class Calendar : public Panel
{
  static void noop() { }
  
  Date mVisibleDate;
  LinkLabel mPrevMonth;
  LinkLabel mNextMonth;
  Font mFontTitle;
  Font mFontContent;
  Font mFontBold;
  Point mHotBox;
  Date mHotDate;

public:

  boost::signal<void (const Date &date)> Action;
  boost::signal<void (const Date &date, bool &hasReminder, bool &isBirthday)> GetDateInfo;

  Calendar(Widget *parent)
    : Panel(parent)
    , mPrevMonth("Previous", this)
    , mNextMonth("Next", this)
    , mFontTitle("Verdana", 12, Font::Style::Bold)
    , mFontContent("Verdana", 8)
    , mFontBold("Verdana", 8, Font::Style::Bold)
  {
    mHotBox = Point(-1, -1);

    setBgColor(Color::White);
    mPrevMonth.setBgColor(Color::White);
    mNextMonth.setBgColor(Color::White);
    
    mPrevMonth.Action.connect(Bind(&Calendar::onPrevMonth, this));
    mNextMonth.Action.connect(Bind(&Calendar::onNextMonth, this));

    // use double-buffering technique for this Widget when onPaint()
    // event is received
    setDoubleBuffered(true);
    setPreferredSize(Size(156, 180));
  }

  // arrange the prev-next month buttons
  virtual void layout()
  {
    Rect rc = getClientBounds();
      
    ScreenGraphics g;
    g.setFont(&mFontTitle);
    int titleHeight = g.measureString(" ").h;

    Size sz;

    sz = mPrevMonth.getPreferredSize();
    mPrevMonth.setBounds(rc.x, rc.y, sz.w, titleHeight);

    sz = mNextMonth.getPreferredSize();
    mNextMonth.setBounds(rc.x+rc.w-sz.w, rc.y, sz.w, titleHeight);
  }

protected:

  // after the size is changed
  virtual void onResize(const Size &sz)
  {
    Panel::onResize(sz);

    // when the window is resized, repaint the client area
    invalidate(true);
  }

  // the paint the calendar
  virtual void onPaint(Graphics &g)
  {
    Size boxSize;
    Rect rc = getClientBounds();
    Rect subrc = getContentBounds(boxSize);

    // measure title size
    g.setFont(&mFontTitle);
    String monthName(mVisibleDate.getMonthName());
    Size titleSize = g.measureString(monthName);

    // measure bottom size
    g.setFont(&mFontBold);
    String yearString(String::fromInt(mVisibleDate.getYear(), 10, 4));
    Size bottomSize = g.measureString(yearString);
    int bottom = bottomSize.h+4;

    // paint content background
    Brush brush(Color(135, 250, 187));
    g.fillRoundRect(brush, subrc, Size(16, 16));
    g.fillGradientRect(subrc.x, subrc.y+subrc.h/2,
		       subrc.w, subrc.h-subrc.h/2,
		       brush.getColor(),
		       Color::White,
		       Vertical);

    // paint bottom line (year)
    g.setColor(Color::Black);
    g.setFont(&mFontBold);
    g.drawString(yearString,
		 subrc.x+subrc.w/2-bottomSize.w/2,
		 subrc.y+subrc.h-bottomSize.h-1);

    // paint header background
    brush.setColor(Color::White);
    g.fillRoundRect(brush,
		    Rect(subrc.x+1, subrc.y+1, subrc.w-2, boxSize.h-2),
		    Size(16, 16));

    // paint headers and days text
    forEachBox(Bind(&Calendar::paintHeader, this, &g, _1, _3, _4),
	       Bind(&Calendar::paintDay, this, &g, _1, _2, _3, _4));

    // paint title text
    g.setColor(Color::Black);
    g.setFont(&mFontTitle);
    g.drawString(monthName, rc.x+rc.w/2-titleSize.w/2, 2);
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    Panel::onMouseMove(ev);

    Size boxSize;
    Rect subrc = getContentBounds(boxSize);

    Point oldHotBox = mHotBox;
    mHotBox = Point(-1, -1);

    forEachBox(boost::bind(Calendar::noop),
	       Bind(&Calendar::changeHot, this, ev.getPoint(), _1, _2, _3, _4));

    if (oldHotBox != mHotBox)
      invalidate(true);
  }

  virtual void onMouseLeave()
  {
    Panel::onMouseLeave();

    if (mHotBox != Point(-1, -1)) {
      mHotBox = Point(-1, -1);
      invalidate(true);
    }
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    Panel::onMouseDown(ev);

    if (mHotBox != Point(-1, -1)) {
      acquireCapture();
      invalidate(true);
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    Panel::onMouseUp(ev);

    if (hasCapture()) {
      releaseCapture();
      invalidate(true);

      // fire the Action signal
      if (mHotBox != Point(-1, -1))
	Action(mHotDate);
    }
  }
  
private:

  Rect getContentBounds(Size &boxSize)
  {
    Rect rc = getClientBounds();

    // measure title size
    ScreenGraphics g;
    g.setFont(&mFontTitle);
    String monthName(mVisibleDate.getMonthName());
    Size titleSize = g.measureString(monthName);

    // paint content
    Rect subrc(rc.x, rc.y+titleSize.h, rc.w, rc.h-titleSize.h);
    boxSize = Size(subrc.w/7, subrc.h/7);
    return subrc;
  }

  template<typename HeaderFunctor, typename DayFunctor>
  void forEachBox(HeaderFunctor headerFunctor, DayFunctor dayFunctor)
  {
    Size boxSize;
    Rect subrc = getContentBounds(boxSize);

    // paint content
    Date currentDate;		// today
    Date iterator = mVisibleDate;
    iterator.setDay(1);		// go to first day of the month

    bool start = false;
    bool end = false;

    // through rows
    for (int i=0; i<7; i++) {
      // through columns
      for (int j=0; j<7; j++) {
	// a box in the table
	Rect box(Point(subrc.x+j*boxSize.w, subrc.y+i*boxSize.h), boxSize+1);
	box.shrink(1);

	// paint headers (week day)
	if (i == 0) {
	  headerFunctor(box, iterator, i, j);
	  continue;
	}

	// start painting days?
	if (!start && j == iterator.getDayOfWeek())
	  start = true;

	// paint the day box
	if (start && !end) {
	  dayFunctor(box, iterator, i, j);
	  
	  // go to next day
	  iterator.nextDay();

	  // the month changes?
	  if (iterator.getMonth() != mVisibleDate.getMonth())
	    end = true;
	}
      }
    }
  }
  
  void onPrevMonth()
  {
    mVisibleDate.prevMonth();
    invalidate(false);
  }

  void onNextMonth()
  {
    mVisibleDate.nextMonth();
    invalidate(false);
  }

  //////////////////////////////////////////////////////////////////////
  // functors

  void paintBoxBase(Graphics *g, const Rect &box, int i, int j,
		    const String &text, const Date *date)
  {
    bool isHot = mHotBox == Point(i, j);
    bool isToday = false;
    bool hasReminder = false;
    bool isBirthday = false;

    // fill date information
    if (date != NULL) {
      isToday = *date == Date();
      GetDateInfo(*date, hasReminder, isBirthday);
    }

    // has reminder?
    if (hasReminder) {
      Pen pen(Color(190, 190, 120));
      g->drawEllipse(pen, Rect(box).shrink(1));
    }

    // is hot?
    if (isHot) {
      Brush brush(hasCapture() ? Color::White: Color(180, 180, 200));
      g->fillRoundRect(brush, box, Size(8, 8));
    }

    // is today?
    if (isToday) {
      Pen pen(Color(0, 0, 200));
      g->drawRoundRect(pen, box, Size(8, 8));
    }

    // paint the text
    Size textSize = g->measureString(text);

    g->setColor(isHot && !hasCapture() ? Color::White: Color::Black);
    g->setFont(date == NULL ? &mFontBold: &mFontContent);
    g->drawString(text, box.getCenter() - Point(textSize/2));
  }

  void paintHeader(Graphics *g, const Rect &box, int i, int j)
  {
    String dayFirstLetter;
    dayFirstLetter.push_back(weekdayNames[j][0]);
    paintBoxBase(g, box, i, j, dayFirstLetter, NULL);
  }

  void paintDay(Graphics *g, const Rect &box, const Date &date, int i, int j)
  {
    paintBoxBase(g, box, i, j, String::fromInt(date.getDay()), &date);
  }

  void changeHot(Point pt, const Rect &box, const Date &date, int i, int j)
  {
    if (box.contains(pt)) {
      mHotBox = Point(i, j);
      mHotDate = date;
    }
  }

};

#endif
