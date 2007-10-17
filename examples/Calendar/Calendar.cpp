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

#include "Vaca/Vaca.h"

#include <ctime>

using namespace Vaca;

const String monthNames[] = {
  "January", "February", "March", "April",
  "May", "June", "July", "August",
  "September", "October", "November", "December"
};

const String dayNames[] = { "S", "M", "T", "W", "T", "F", "S" };

//////////////////////////////////////////////////////////////////////
// Easy Date

class Date
{
  std::time_t t;
  struct std::tm tm;

public:
  Date() {
    t = std::time(NULL);
    tm = *std::localtime(&t);
  }
  
  int getDay() const { return tm.tm_mday; }	  // 1-31
  int getMonth() const { return tm.tm_mon+1; }	  // 1-12
  int getYear() const { return tm.tm_year+1900; } // 1900-2038
  int getDayOfWeek() const { return tm.tm_wday; }   // 0-6

  String getMonthName() const { return monthNames[tm.tm_mon]; }

  void setDay(int day) { tm.tm_mday = day; t=std::mktime(&tm); } // 1-31

  void prevDay() { tm.tm_mday++; t=std::mktime(&tm); }
  void nextDay() { tm.tm_mday++; t=std::mktime(&tm); }

  void prevMonth() { tm.tm_mon--; t=std::mktime(&tm); }
  void nextMonth() { tm.tm_mon++; t=std::mktime(&tm); }

  bool operator==(const Date &date) const
  {
    return
      getYear() == date.getYear() &&
      getMonth() == date.getMonth() &&
      getDay() == date.getDay();
  }

};

//////////////////////////////////////////////////////////////////////
// The calendar

class MainFrame : public Frame
{
  Date mVisibleDate;
  Button mPrevMonth;
  Button mNextMonth;
  Font mFontTitle;
  Font mFontContent;
  Font mFontBottom;
  MenuBar mMenuBar;
  Menu mCalendarMenu;
  Menu mOptionsMenu;
  
public:

  MainFrame()
    : Frame("Calendar", NULL,
	    FrameStyle +
	    ClipChildrenStyle) // the clip-children style avoid to
			       // paint above the buttons
    , mPrevMonth("<", this)
    , mNextMonth(">", this)
    , mFontTitle("Verdana", 12)
    , mFontContent("Verdana", 8)
    , mFontBottom("Verdana", 8, Font::Style::Bold)
    , mCalendarMenu("&Calendar")
    , mOptionsMenu("&Options")
  {
    initializeMenuBar();
    setMenuBar(&mMenuBar);
    setSize(getNonClientSize()+Size(156, 180));

    mPrevMonth.Action.connect(Bind(&MainFrame::onPrevMonth, this));
    mNextMonth.Action.connect(Bind(&MainFrame::onNextMonth, this));
  }

  // arrange the prev-next month buttons
  virtual void layout()
  {
    ScreenGraphics g;
    g.setFont(mFontTitle);
    int titleHeight = g.measureString(" ").h;

    mPrevMonth.setBounds(Rect(0, 0, 16, titleHeight));
    mNextMonth.setBounds(Rect(getClientBounds().w-16, 0, 16, titleHeight));
  }

protected:

  // before to change the size
  virtual void onResizing(int edge, Rect &rc)
  {
    Frame::onResizing(edge, rc);
    
    // the size must be multiple of 7
    if (edge == WMSZ_RIGHT ||
	edge == WMSZ_TOPRIGHT ||
	edge == WMSZ_BOTTOMRIGHT) {
      rc.w = 3 + ((rc.w-3) / 7) * 7;
    }
    else if (edge == WMSZ_LEFT ||
	     edge == WMSZ_TOPLEFT ||
	     edge == WMSZ_BOTTOMLEFT) {
      int w = rc.w;
      rc.w = 3 + ((rc.w-3) / 7) * 7;
      rc.x += w-rc.w;
    }
  }

  // after the size is changed
  virtual void onResize(const Size &sz)
  {
    Frame::onResize(sz);

    // when the window is resized, repaint the client area (without
    // erasing the background, we use double-buffering)
    invalidate(false);
  }

  // the paint the calendar
  virtual void onPaint(Graphics &gDest)
  {
    Size size = getClientBounds().getSize();
    Image image(gDest, size); // Create the image for double-buffering
    Graphics &g(image.getGraphics()); // Get the Graphics to draw in the image
    Rect rc = Rect(size);	      // Area where we can draw
    
    // paint border
    g.draw3dRect(rc, Color::Gray, Color::White);
    rc.shrink(1);

    // measure title size
    g.setFont(mFontTitle);
    String monthName(mVisibleDate.getMonthName());
    Size titleSize = g.measureString(monthName);

    // measure bottom size
    g.setFont(mFontBottom);
    String yearString(String::fromInt(mVisibleDate.getYear(), 10, 4));
    Size bottomSize = g.measureString(yearString);
    int bottom = bottomSize.h+4;

    // paint title background
    g.setFont(mFontTitle);
    g.fillGradientRect(Rect(rc.x, rc.y, rc.w, titleSize.h),
		       Color(0, 150, 170),
		       Color(0, 180, 190),
		       Vertical);

    // paint title text
    g.setColor(Color(255, 255, 255));
    g.drawString(monthName, rc.x+rc.w/2-titleSize.w/2, 2);

    // paint content background
    g.fillGradientRect(Rect(rc.x, rc.y+titleSize.h, rc.w, rc.h-titleSize.h),
		       Color::White,
		       Color(220, 230, 240),
		       Vertical);

    // paint bottom line
    g.setColor(Color(0, 0, 180));
    g.setFont(mFontBottom);
    g.drawString(yearString, rc.x+rc.w/2-bottomSize.w/2, rc.y+rc.h-bottomSize.h);

    // paint content
    rc = Rect(rc.x, rc.y+titleSize.h, rc.w, rc.h-titleSize.h-bottomSize.h);
    Size boxSize(rc.w/7, rc.h/7);

    Date currentDate;		// today
    Date iterator = mVisibleDate;
    iterator.setDay(1);		// go to first day of the month

    bool start = false;
    bool end = false;

    g.setFont(mFontContent);

    // through rows
    for (int j=0; j<7; j++) {
      // through columns
      for (int i=0; i<7; i++) {
	// a box in the table
	Rect box(Point(rc.x+i*boxSize.w, rc.y+j*boxSize.h), boxSize+1);
	box.shrink(1);

	// paint headers (week day)
	if (j == 0) {
	  // paint the header background
	  g.fillGradientRect(box,
			     Color(190, 190, 190),
			     Color(220, 220, 220),
			     Vertical);

	  g.setColor(Color(255, 255, 255));
	  g.fillRect(box.x+1, box.y+1, box.w-2, 1);

	  // paint the header text
	  String dayString(dayNames[i]);
	  Size daySize = g.measureString(dayString);

	  g.setColor(Color(0, 0, 0));
	  g.drawString(dayString, box.x+box.w/2-daySize.w/2, box.y+box.h/2-daySize.h/2);

	  continue;
	}

	// start painting days?
	if (!start && i == iterator.getDayOfWeek())
	  start = true;

	// paint the day box
	if (start && !end) {
	  // is this day today?
	  bool isToday = iterator == currentDate;

	  // paint the day background
	  g.fillGradientRect(box,
			     isToday ? Color(255, 255, 0): Color(220, 220, 230),
			     isToday ? Color(255, 255, 255): Color(255, 255, 255),
			     Vertical);

	  g.setColor(Color(255, 255, 255));
	  g.fillRect(box.x+1, box.y+1, box.w-2, 1);

	  // paint the day text
	  String dayString(String::fromInt(iterator.getDay()));
	  Size daySize = g.measureString(dayString);

	  g.setColor(Color(0, 0, 0));
	  g.drawString(dayString, box.x+box.w/2-daySize.w/2, box.y+box.h/2-daySize.h/2);

	  // go to next day
	  iterator.nextDay();

	  // the month changes?
	  if (iterator.getMonth() != mVisibleDate.getMonth())
	    end = true;
	}
      }
    }

    // draw the image on "gDest"
    gDest.drawImage(image, getClientBounds().getOrigin());
  }

private:

  void initializeMenuBar()
  {
    MenuItem *menuItem;

    menuItem = mCalendarMenu.add("E&xit");
    menuItem->Action.connect(Bind(&MainFrame::dispose, this));

    menuItem = mOptionsMenu.add("&Change Title Font");
    menuItem->Action.connect(Bind(&MainFrame::onChangeTitleFont, this));

    menuItem = mOptionsMenu.add("&Change Content Font");
    menuItem->Action.connect(Bind(&MainFrame::onChangeContentFont, this));

    menuItem = mOptionsMenu.add("&Change Bottom Font");
    menuItem->Action.connect(Bind(&MainFrame::onChangeBottomFont, this));

    mMenuBar.add(&mCalendarMenu);
    mMenuBar.add(&mOptionsMenu);
  }

  void onChangeFont(Font &font)
  {
    FontDialog dialog(font, this);
    if (dialog.doModal()) {
      layout();
      invalidate(false);
    }
  }

  void onChangeTitleFont  () { onChangeFont(mFontTitle);   }
  void onChangeContentFont() { onChangeFont(mFontContent); }
  void onChangeBottomFont () { onChangeFont(mFontBottom);  }

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

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
