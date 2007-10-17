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

#include <time.h>

#include "Vaca/Vaca.h"
#include "table.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// constants

const Point NullCell = Point(-1, -1);

const Point boxOffset[] = {
  Point(0, 0), Point(3, 0), Point(6, 0),
  Point(0, 3), Point(3, 3), Point(6, 3),
  Point(0, 6), Point(3, 6), Point(6, 6)
};

const Point historyOffsets[] = {
  Point(32*0/3+32/3/2, 32*0/3+32/3/2),
  Point(32*1/3+32/3/2, 32*0/3+32/3/2),
  Point(32*2/3+32/3/2, 32*0/3+32/3/2),

  Point(32*0/3+32/3/2, 32*1/3+32/3/2),
  Point(32*2/3+32/3/2, 32*1/3+32/3/2),

  Point(32*0/3+32/3/2, 32*2/3+32/3/2),
  Point(32*1/3+32/3/2, 32*2/3+32/3/2),
  Point(32*2/3+32/3/2, 32*2/3+32/3/2),
};

//////////////////////////////////////////////////////////////////////
// Dialog to insert the game seed

class InsertSeedDialog : public Dialog
{
  Panel mTop;
  Panel mBottom;
  Label mSeedLabel;
  Edit mSeedEdit;
  Button mOk;
  Button mCancel;

public:

  InsertSeedDialog(Widget *parent)
    : Dialog("Game Seed", parent)
    , mTop(this)
    , mBottom(this)
    , mSeedLabel("Seed:", &mTop)
    , mSeedEdit(String::fromInt(rand()), &mTop)
    , mOk("OK", &mBottom)
    , mCancel("Cancel", &mBottom)
  {
    setLayout(new BoxLayout(Vertical, true));
    mTop.setLayout(new BoxLayout(Horizontal, false, 0));
    mBottom.setLayout(new BoxLayout(Horizontal, true, 0));
    mSeedEdit.setConstraint(new BoxConstraint(true));

    mSeedEdit.focus();
    mSeedEdit.selectAll();
    mOk.Action.connect(Bind(&InsertSeedDialog::defaultOkAction, this));
    mCancel.Action.connect(Bind(&InsertSeedDialog::defaultCancelAction, this));

    mOk.setDefault(true);

    setSize(preferredSize());
    center();
  }

  int getSeed() {
    return mSeedEdit.getText().parseInt();
  }

};

//////////////////////////////////////////////////////////////////////
// The Sudoku window

class MainFrame : public Frame
{
  table<int> mTable;		     // digit for each cell
  table<bool> mExposed;              // is the cell exposed (visible) from the beginning?
  table<bool> mWarning;              // is the cell in warning? (has collisions with other cell)
  table<std::vector<int> > mHistory; // history of digits for each cell
  Point mHighlighted;		     // current highlighted cell
  Font mFont;			     // font for the digits
  Font mFontSmall;		     // font for the digits of the history
  Font mFontBold;		     // font for the warnings and for same-digits
  MenuBar mMenuBar;		     // the menu bar
  Menu mGameMenu;		     // the game menu

public:

  // create the sudoku window
  MainFrame()
    : Frame("Sudoku", NULL, FrameStyle
			    - MaximizableFrameStyle // can't be maximized
			    - ResizableFrameStyle)  // can't change the size
    , mFont("Verdana", 10)
    , mFontSmall("Verdana", 7)
    , mFontBold("Verdana", 10, Font::Style::Bold)
    , mGameMenu("&Game")
  {
    mGameMenu.add("&New\tCtrl+Shift+N", Keys::Control | Keys::Shift | Keys::N)
      ->Action.connect(Bind(&MainFrame::onNew, this));
    mGameMenu.add("New with &seed\tCtrl+N", Keys::Control | Keys::N)
      ->Action.connect(Bind(&MainFrame::onNewWithSeed, this));
    mGameMenu.addSeparator();
    mGameMenu.add("&Exit")->Action.connect(Bind(&MainFrame::dispose, this));
    mMenuBar.add(&mGameMenu);
    mMenuBar.add("&Help")->Action.connect(Bind(&MainFrame::onHelp, this));

    setBgColor(Color::White);
    setMenuBar(&mMenuBar);
    setSize(getNonClientSize()+Size(32*9, 32*9));
    center();

    onNew();
  }

protected:

  // paint the client area
  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();
    Point pt;

    // draw horizontal and vertical lines
    for (int i=0; i<=2; i++) {
      g.setColor(Color::Black);
      if (i > 0) {
	g.drawRect(0, i*rc.h/3, rc.w, 2);
	g.drawRect(i*rc.w/3, 0, 2, rc.h);
      }
      g.setColor(Color::Gray);
      for (int j=1; j<=2; j++) {
	g.drawRect(0, i*rc.h/3+j*rc.h/9, rc.w, 1);
	g.drawRect(i*rc.w/3+j*rc.w/9, 0, 1, rc.h);
      }
    }

    for (pt.y=0; pt.y<9; pt.y++) {
      for (pt.x=0; pt.x<9; pt.x++) {
	Rect cell = getCellRect(pt);
	Color color;

	// cell background color
	if (mWarning[pt] && mExposed[pt])
	  color = Color::Black;
	else if (mWarning[pt])
	  color = Color::Red;
	else if (mExposed[pt])
	  color = Color(240, 240, 240);
	else if (mHighlighted == pt)
	  color = Color::Yellow;
	else
	  color = Color::White;

	// paint cell background
	g.setColor(color);
	g.fillRect(cell);

	if (mTable[pt] != 0 || !mHistory[pt].empty()) {
	  // normal
	  if (!mWarning[pt]) {
	    g.setColor(Color::Black);
	    g.setFont(mHighlighted != NullCell &&
		      mTable[mHighlighted] == mTable[pt] ? mFontBold:
							   mFont);

	  }
	  // warning
	  else {
	    g.setColor(Color::White);
	    g.setFont(mFontBold);
	  }

	  if (mTable[pt] != 0) {
	    String text = String::fromInt(mTable[pt]);
	    Size textSize = g.measureString(text);
	    g.drawString(text,
			 cell.x + (cell.w - textSize.w)/2,
			 cell.y + (cell.h - textSize.h)/2);
	  }

	  // history
	  if (!mHistory[pt].empty()) {
	    int c = 0;

	    g.setFont(mFontSmall);
	    g.setColor(Color(190, 190, 190));

	    for (std::vector<int>::iterator it=mHistory[pt].begin();
		 it!=mHistory[pt].end();
		 ++it) {
	      String text = String::fromInt(*it);
	      Size textSize = g.measureString(text);

	      g.drawString(text,
			   cell.x + historyOffsets[c].x - textSize.w/2,
			   cell.y + historyOffsets[c].y - textSize.h/2);

	      c++;
	    }
	  }
	}
      }
    }
  }

  // when the mouse move through the client area, change the current
  // highlighted cell
  virtual void onMouseMove(MouseEvent &ev)
  {
    Point pt, oldCell = mHighlighted;
    Rect rc = getClientBounds();

    mHighlighted = NullCell;

    for (pt.y=0; pt.y<9; pt.y++)
      for (pt.x=0; pt.x<9; pt.x++)
	if (getCellRect(pt).contains(ev.getPoint())) {
	  mHighlighted = pt;
	  break;
	}

    if (mHighlighted != oldCell) {
      invalidateCellsWithDigit(oldCell);
      invalidateCellsWithDigit(mHighlighted);
    }
  }

  // if the mouse leave the client area, remove the highlight
  virtual void onMouseLeave()
  {
    if (mHighlighted != NullCell) {
      invalidateCellsWithDigit(mHighlighted);
      mHighlighted = NullCell;
    }
  }

  // convert double-click to a single-click
  virtual void onDoubleClick(MouseEvent &ev)
  {
    onMouseDown(ev);
  }

  // push into or pop from the history an element
  virtual void onMouseDown(MouseEvent &ev)
  {
    if ((mHighlighted != NullCell) &&
	(!mExposed[mHighlighted])) {
      Point pt = mHighlighted;

      invalidateCellsWithDigit(pt);

      // pop an element from history
      if (ev.getButton() == MouseButtons::Right) {
	if (!mHistory[pt].empty()) {
	  mTable[pt] = mHistory[pt].back();
	  mHistory[pt].pop_back();
	}
	else
	  mTable[pt] = 0;
      }
      // remove all
      else if (ev.getButton() == MouseButtons::Middle) {
	mHistory[pt].clear();
      }
      // push a digit into history
      else {
	// push the current entry into history
	pushIntoHistory(pt);

	// clear the digit
	mTable[pt] = 0;
      }

      invalidateCellsWithDigit(pt);
      invalidateWarnings();
      checkWin();
    }
  }

  // change current selected cell digit
  virtual void onKeyDown(KeyEvent &ev)
  {
    Keys::Type keyCode = ev.getKeyCode();
    int digit =
      (keyCode >= Keys::D0 && keyCode <= Keys::D9) ? keyCode - Keys::D0:
      ((keyCode >= Keys::NumPad0 && keyCode <= Keys::NumPad9) ? keyCode - Keys::NumPad0:
								-1);
    if ((mHighlighted != NullCell) && (digit >= 0) &&
	(!mExposed[mHighlighted])) {
      Point pt = mHighlighted;

      invalidateCellsWithDigit(pt);

      // push the current entry into history
      if (digit != 0)
	pushIntoHistory(pt);

      // replace the digit in the table
      mTable[pt] = digit;

      // remove the new selected entry from the history
      remove_element_from_container(mHistory[pt], mTable[pt]);

      // redraw
      invalidateCellsWithDigit(pt);
      invalidateWarnings();
      checkWin();

      ev.consume();
    }
  }

private:

  //////////////////////////////////////////////////////////////////////
  // signal bindings

  void onNew()
  {
    generateGame(rand());
  }

  void onNewWithSeed()
  {
    // create the dialog
    InsertSeedDialog dlg(this);

    // show the dialog, and if the user press ok...
    if (dlg.doModal())
      // start a new game with the specified seed
      generateGame(dlg.getSeed());
  }

  void onHelp()
  {
    msgBox("Every 3x3 box, every column, and every row can't repeat the same digit.\n"
	   "Use the number from 1 to 9 to add a digit, and the 0 to remove it.\n"
	   "You can use the left mouse button to push the digit to the history,\n"
	   "the right button to pop a digit from the history, and the middle button\n"
	   "to remove the entire history.\n",
	   "Help", MB_OK);
  }

  //////////////////////////////////////////////////////////////////////
  // logic

  void pushIntoHistory(Point pt)
  {
    // push the current entry into history
    if ((mTable[pt] != 0) &&
	(std::find(mHistory[pt].begin(),
		   mHistory[pt].end(),
		   mTable[pt]) == mHistory[pt].end())) {
      // history too big?
      if (mHistory[pt].size() == 8)
	mHistory[pt].erase(mHistory[pt].begin());

      // put the new element
      mHistory[pt].push_back(mTable[pt]);
    }
  }

  Rect getCellRect(const Point &pt)
  {
    if (pt != NullCell) {
      Rect rc = getClientBounds();
      int dx = 1+((pt.x>0) && (pt.x%3)==0?1:0);
      int dy = 1+((pt.y>0) && (pt.y%3)==0?1:0);
      return Rect(rc.x+pt.x*rc.w/9+dx,
		  rc.y+pt.y*rc.h/9+dy, rc.w/9-dx, rc.h/9-dy);
    }
    else
      return Rect();
  }

  void invalidateCellRect(const Point &pt)
  {
    if (pt != NullCell)
      invalidate(getCellRect(pt), false);
  }

  void invalidateCellsWithDigit(Point point)
  {
    if (point != NullCell) {
      Point pt;

      for (pt.y=0; pt.y<9; pt.y++)
	for (pt.x=0; pt.x<9; pt.x++)
	  if (mTable[pt] == mTable[point])
	    invalidateCellRect(pt);
    }
  }

  void invalidateWarnings()
  {
    table<bool> oldWarning = mWarning;
    Point pt;

    checkWarnings();

    // invalidate cells where warning state changes
    for (pt.y=0; pt.y<9; pt.y++)
      for (pt.x=0; pt.x<9; pt.x++)
	if (oldWarning[pt] != mWarning[pt])
	  invalidateCellRect(pt);
  }

  void checkWarnings()
  {
    int c, x, y, u, v;

    // clear warnings
    mWarning.reset(false);

    // nine boxes, nine rows, nine columns
    for (c=0; c<9; c++) {
      // check a box
      for (y=boxOffset[c].y; y<boxOffset[c].y+3; y++) {
	for (x=boxOffset[c].x; x<boxOffset[c].x+3; x++) {
	  for (v=boxOffset[c].y; v<boxOffset[c].y+3; v++) {
	    for (u=boxOffset[c].x; u<boxOffset[c].x+3; u++) {
	      if (mTable[v][u] != 0 &&
		  y != v && x != u && mTable[v][u] == mTable[y][x]) {
		mWarning[v][u] = true;
		mWarning[y][x] = true;
	      }
	    }
	  }
	}
      }

      // check a row
      for (x=0; x<9; x++) {
	for (u=0; u<9; u++) {
	  if (mTable[c][u] != 0 &&
	      x != u && mTable[c][u] == mTable[c][x]) {
	    mWarning[c][u] = true;
	    mWarning[c][x] = true;
	  }
	}
      }

      // check a column
      for (y=0; y<9; y++) {
	for (v=0; v<9; v++) {
	  if (mTable[v][c] != 0 &&
	      y != v && mTable[v][c] == mTable[y][c]) {
	    mWarning[v][c] = true;
	    mWarning[y][c] = true;
	  }
	}
      }
    }
  }

  void checkWin()
  {
    int x, y;

    for (y=0; y<9; y++)
      for (x=0; x<9; x++)
	if (mTable[y][x] == 0 || mWarning[y][x])
	  return;

    msgBox("You win!!!",
	   "Congratulations",
	   MB_OK);

    mExposed.reset(true);
    invalidate(false);
  }

  void generateGame(int gameNumber)
  {
    srand(gameNumber);
    setText("Sudoku (Game " + String::fromInt(gameNumber) + ")");

    mHighlighted = NullCell;

    // clear the table and the history
    mTable.reset(0);
    for (table<std::vector<int> >::iterator it=mHistory.begin(); it!=mHistory.end(); ++it)
      it->clear();

    // fill all boxes with all digits
    int i, j;
    for (i=0; i<9; i++)
      for (j=0; j<9; j++)
	fillBoxWithDigit(boxOffset[j].x, boxOffset[j].y, i+1);

    // make blank the cells that the player should fill
    std::vector<Point> digits[9];
    Point pt;
    for (pt.y=0; pt.y<9; pt.y++)
      for (pt.x=0; pt.x<9; pt.x++)
	if (mTable[pt] != 0)
	  digits[mTable[pt]-1].push_back(pt);

    // leave 30 digits exposed
    mExposed.reset(false);
    int lucky[9];
    for (i=0; i<9; i++) lucky[i] = 3;
    for (i=0; i<3; i++) lucky[rand()%9]++;
    for (i=0; i<9; i++) {
      for (j=0; j<lucky[i]; j++) {
	do {
	  // select a cell to expose
	  pt = digits[i][rand() % digits[i].size()];
	  // check if this box is too exposed (maximum of 4 exposed
	  // cells per box)
	} while (getExposedCellsInBox(pt) >= 4);

	remove_element_from_container(digits[i], pt);

	mExposed[pt] = true;
      }
    }

    // set to 0 non-exposed cells
    for (i=0; i<9; i++)
      for (j=0; j<9; j++)
	if (!mExposed[i][j])
	  mTable[i][j] = 0;

    // check warnings
    checkWarnings();
    invalidate(true);
  }

  int getExposedCellsInBox(Point point)
  {
    Point pt;
    int exposedCount = 0;
    for (pt.y=(point.y/3)*3; pt.y<(point.y/3)*3+3; pt.y++) {
      for (pt.x=(point.x/3)*3; pt.x<(point.x/3)*3+3; pt.x++) {
	if (mExposed[pt])
	  exposedCount++;
      }
    }
    return exposedCount;
  }

  // put a digit in its 3x3 box
  void fillBoxWithDigit(int u, int v, int digit)
  {
    int c, x, y;
    int values[9];		// values of the 3x3 box
    bool blanks[9];		// blank places to put a digit
    bool posibles[9];		// places where the digit don't generate warnings
    std::vector<int> chances;	// vector with the true-indexes of "posibles"
    int cellCount;		// current cell collisions
    int bestCell;		// cell where are less collisions
    int bestCount = -1;		// how many collisions has that cell
    
    c = 0;
    for (y=0; y<3; y++)
      for (x=0; x<3; x++) {
	values[c] = mTable[v+y][u+x];
	blanks[c] = (values[c] == 0);
	if (blanks[c]) {
	  cellCount = countDigitCollisionsInRowAndColumn(u+x, v+y, digit);
	  posibles[c] = (cellCount == 0);

	  if (bestCount < 0 ||
	      bestCount > cellCount) {
	    bestCount = cellCount;
	    bestCell = c;
	  }
	}
	else
	  posibles[c] = false;
	c++;
      }

    // chances
    for (c=0; c<9; c++)
      if (posibles[c])
	chances.push_back(c);

    // excelent, we found a good place to put the digit
    if (!chances.empty()) {
      // put the digit
      c = chances[rand() % chances.size()];
      mTable[v+(c/3)][u+(c%3)] = digit;
    }
    // bad news, we must swap cells to avoid the collision
    else {
      // put the digit
      c = bestCell;
      mTable[v+(c/3)][u+(c%3)] = digit;

      // swap cells
      swapCellToFixDigitCollision(u+(c%3), v+(c/3), digit);
    }
  }

  int countDigitCollisionsInRowAndColumn(int x, int y, int digit)
  {
    int collisions = 0;
    for (int c=0; c<9; c++) {
      if (mTable[y][c] == digit) collisions++;
      if (mTable[c][x] == digit) collisions++;
    }
    return collisions;
  }

  // try to swap cells to fix the collision with the cell at (x,y)
  void swapCellToFixDigitCollision(int x, int y, int digit)
  {
    // look up 9 cells in the row and the column that cross at (x,y) position
    for (int c=0; c<9; c++) {
      // check if the c-column in the y-row (cy-cell), has the same
      // digit that the xy-cell (remember to avoid x == c)
      if (x != c && mTable[y][c] == digit) {
	// find other row to swap
	for (int q=(y/3)*3; q<(y/3)*3+3; q++) {
	  // the q-row can't be at the same row as (x,y) (the y-row)
 	  if (q != y) {
	    mTable[y][c] = 0;	// make this cell temporary blank

	    // check if I move the cell in the y-row to the cell in
	    // the q-row, there aren't a collision
	    if (countDigitCollisionsInRowAndColumn(c, q, digit) == 0) {
	      // anyway, swap the cells
	      mTable[y][c] = mTable[q][c];
	      mTable[q][c] = digit;

	      // well, now we should check if this movement doesn't
	      // make another warning...
	      if (mTable[y][c] &&
		  countDigitCollisionsInRowAndColumn(c, y, mTable[y][c]) > 0)
		swapCellToFixDigitCollision(c, y, mTable[y][c]);

	      break;
	    }
	    // restore the old value
	    else {
	      mTable[y][c] = digit;
	    }
	  }
	}
      }
      // now for the column (is symmetrical to the row)
      if (y != c && mTable[c][x] == digit) {
	for (int q=(x/3)*3; q<(x/3)*3+3; q++) {
	  if (q != x) {
	    mTable[c][x] = 0;

	    if (countDigitCollisionsInRowAndColumn(q, c, digit) == 0) {
	      mTable[c][x] = mTable[c][q];
	      mTable[c][q] = digit;

	      if (mTable[c][x] &&
		  countDigitCollisionsInRowAndColumn(x, c, mTable[c][x]) > 0)
		swapCellToFixDigitCollision(x, c, mTable[c][x]);

	      break;
	    }
	    else {
	      mTable[c][x] = digit;
	    }
	  }
	}
      }
    }
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
  srand(static_cast<unsigned int>(time(NULL)));

  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
