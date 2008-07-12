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

#include <algorithm>
#include <time.h>

#include <Vaca/Vaca.hpp>
#include "grid.hpp"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Algorithms

template<typename InputIterator, typename BinaryOperation>
void
compare_with_all(InputIterator first, InputIterator last,
		 BinaryOperation binary_op)
{
  for ( ; first != last; ++first) {
    InputIterator second = first;
    for (++second; second != last; ++second)
      binary_op(*first, *second);
  }
}

template<typename InputIterator, typename Predicate>
bool
exist(InputIterator first, InputIterator last, Predicate pred)
{
  for ( ; first != last; ++first)
    if (pred(*first))
      return true;
  return false;
}

//////////////////////////////////////////////////////////////////////
// constants

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

  InsertSeedDialog(Widget* parent)
    : Dialog("Game Seed", parent)
    , mTop(this)
    , mBottom(this)
    , mSeedLabel("Seed:", &mTop)
    , mSeedEdit(String::fromInt(rand()), &mTop)
    , mOk("OK", &mBottom)
    , mCancel("Cancel", &mBottom)
  {
    setLayout(new BoxLayout(Orientation::Vertical, true));
    mTop.setLayout(new BoxLayout(Orientation::Horizontal, false, 0));
    mBottom.setLayout(new BoxLayout(Orientation::Horizontal, true, 0));
    mSeedEdit.setConstraint(new BoxConstraint(true));

    mSeedEdit.requestFocus();
    mSeedEdit.selectAll();
    mOk.Action.connect(Bind(&InsertSeedDialog::defaultOkAction, this));
    mCancel.Action.connect(Bind(&InsertSeedDialog::defaultCancelAction, this));

    mOk.setDefault(true);

    setSize(getPreferredSize());
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
public:	// MSVC needs it to be public because grid<cell> needs to access to it
  ////////////////////////////////////////
  // a cell
  struct cell {
    int digit;
    bool given;
    bool warning;
    std::vector<int> history;
    cell() {
      digit = 0;
      given = false;
      warning = false;
    }
  };

private:  
  ////////////////////////////////////////
  // a predicates and functors

  struct is_digit {
    int digit;
    is_digit(int digit) : digit(digit) { }
    bool operator()(cell &c) { return digit == c.digit; }
  };

  struct set_warning {
    void operator()(cell& c) {
      c.warning = false;
    }
    void operator()(cell& c1, cell& c2) {
      if (&c1 != &c2 &&
	  c1.digit != 0 &&
	  c1.digit == c2.digit) {
	c1.warning = true;
	c2.warning = true;
      }
    }
  };

  struct set_given_to_true {
    void operator()(cell& c) {
      c.given = true;
    }
  };

  struct if_given_is_true {
    bool operator()(const cell& c) {
      return c.given;
    }
  };

  ////////////////////////////////////////
  // types

  typedef grid<cell>::iterator grid_iterator;
  typedef grid<cell>::box_iterator box_iterator;
  typedef grid<cell>::col_iterator col_iterator;
  typedef grid<cell>::row_iterator row_iterator;

  ////////////////////////////////////////
  // members

  grid<cell> m_puzzle;
  grid_iterator m_hotCell;
  Font m_font;			     // font for the digits
  Font m_fontSmall;		     // font for the digits of the history
  Font m_fontBold;		     // font for the warnings and for same-digits

public:

  // create the sudoku window
  MainFrame()
    : Frame("Sudoku", NULL, FrameStyle
			    - MaximizableFrameStyle // can't be maximized
			    - ResizableFrameStyle)  // can't change the size
    , m_font("Verdana", 10)
    , m_fontSmall("Verdana", 7)
    , m_fontBold("Verdana", 10, FontStyle::Bold)
  {
    m_hotCell = m_puzzle.end();

    setBgColor(Color::White);
    setMenuBar(createMenuBar());
    setSize(getNonClientSize()+Size(32*9, 32*9));
    center();

    setDoubleBuffered(true);

    onNew();
  }

protected:

  // paint the client area
  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();

    // paint the 9x9 grid
    for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
      Rect cellBounds = getCellBounds(it);
      Color color;

      // cell background color
      if (it->warning)
	color = Color::Red;
      else if (it->given)
	color = Color(240, 240, 240);
      else if (m_hotCell == it)
	color = Color::Yellow;
      else
	color = Color::White;

      // paint cell background
      Brush brush(color);
      g.fillRect(brush, cellBounds);

      // paint text inside the cell
      if (it->digit != 0 || !it->history.empty()) {
	// warning
	if (it->warning) {
	  g.setColor(Color::White);
	  g.setFont(&m_fontBold);
	}
	// hot or normal
	else {
	  g.setColor(Color::Black);
	  g.setFont(m_hotCell != m_puzzle.end() &&
		    it->digit == m_hotCell->digit ? &m_fontBold: // hot
						   &m_font); // normal
	}

	if (it->digit != 0) {
	  String text = String::fromInt(it->digit);
	  Size textSize = g.measureString(text);
	  g.drawString(text,
		       cellBounds.x + (cellBounds.w - textSize.w)/2,
		       cellBounds.y + (cellBounds.h - textSize.h)/2);
	}

	if (!it->history.empty()) {
	  int c = 0;

	  g.setFont(&m_fontSmall);
	  if (!it->warning)
	    g.setColor(Color(190, 190, 190));

	  for (std::vector<int>::iterator it2=it->history.begin();
	       it2!=it->history.end(); ++it2) {
	    String text = String::fromInt(*it2);
	    Size textSize = g.measureString(text);

	    g.drawString(text,
			 cellBounds.x + historyOffsets[c].x - textSize.w/2,
			 cellBounds.y + historyOffsets[c].y - textSize.h/2);

	    c++;
	  }
	}
      }
    }
	
    // draw horizontal and vertical lines to split 3x3 regions and cells
    Pen blackPen(Color::Black);
    Pen grayPen(Color::Gray);
    
    for (int i=0; i<=2; i++) {
      if (i > 0) {
	g.drawRect(blackPen, 0, i*rc.h/3, rc.w, 2);
	g.drawRect(blackPen, i*rc.w/3, 0, 2, rc.h);
      }

      for (int j=1; j<=2; j++) {
	g.drawRect(grayPen, 0, i*rc.h/3+j*rc.h/9, rc.w, 1);
	g.drawRect(grayPen, i*rc.w/3+j*rc.w/9, 0, 1, rc.h);
      }
    }
  }

  // when the mouse move through the client area, change the current
  // hot cell
  virtual void onMouseMove(MouseEvent &ev)
  {
    grid_iterator oldHot = m_hotCell;
    m_hotCell = m_puzzle.end();

    for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
      if (getCellBounds(it).contains(ev.getPoint())) {
	m_hotCell = it;
	break;
      }
    }

    if (m_hotCell != oldHot)
      invalidate(true);
  }
  
  // if the mouse leave the client area, remove the highlight
  virtual void onMouseLeave()
  {
    if (m_hotCell != m_puzzle.end()) {
      invalidate(true);
      m_hotCell = m_puzzle.end();
    }
  }

  // push into or pop from the history an element
  virtual void onMouseDown(MouseEvent &ev)
  {
    if ((m_hotCell != m_puzzle.end()) &&
	(!m_hotCell->given)) {
      // pop an element from history
      if (ev.getButton() == MouseButtons::Right) {
	if (!m_hotCell->history.empty()) {
	  m_hotCell->digit = m_hotCell->history.back();
	  m_hotCell->history.pop_back();
	}
	else
	  m_hotCell->digit = 0;
      }
      // remove all
      else if (ev.getButton() == MouseButtons::Middle) {
	m_hotCell->history.clear();
      }
      // push a digit into history
      else {
	// push the current entry into history
	pushIntoHistory(*m_hotCell);

	// clear the digit
	m_hotCell->digit = 0;
      }

      updateWarnings();
      checkWin();
      invalidate(true);
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
    if ((digit >= 0) && (m_hotCell != m_puzzle.end()) && (!m_hotCell->given)) {
      // push the current entry into history
      if (digit != 0)
	pushIntoHistory(*m_hotCell);

      // replace the digit in the table
      m_hotCell->digit = digit;

      // remove the new selected entry from the history
      remove_from_container(m_hotCell->history,
			    m_hotCell->digit);

      // check warning & win
      updateWarnings();
      checkWin();

      // redraw
      invalidate(true);

      ev.consume();
    }
//     // fill history
//     else if (keyCode == Keys::H) {
//       // fill history
//       for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
// 	if (!it->given) {
// 	  it->history.clear();

// 	  for (int digit=1; digit<=9; ++digit) {
// 	    if (it->digit == digit)
// 	      continue;
	      
// 	    if (!exist(m_puzzle.col_begin(it.col()), m_puzzle.col_end(it.col()), is_digit(digit)) &&
// 		!exist(m_puzzle.row_begin(it.row()), m_puzzle.row_end(it.row()), is_digit(digit)) &&
// 		!exist(m_puzzle.box_begin(it.box()), m_puzzle.box_end(it.box()), is_digit(digit)))
// 	      it->history.push_back(digit);
// 	  }
// 	}
//       }
//       // help
//       for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
// 	if (!it->given && it->digit == 0) {
// 	  if (it->history.size() == 1) {
// 	    it->digit = it->history.front();
// 	    it->history.clear();
// 	  }
// 	}
//       }
//       invalidate(true);
//     }
  }

private:

  MenuBar* createMenuBar()
  {
    Menu* gameMenu = new Menu("&Game");
    gameMenu->add("&New\tCtrl+Shift+N", Keys::Control | Keys::Shift | Keys::N)
      ->Action.connect(Bind(&MainFrame::onNew, this));
    gameMenu->add("New with &seed\tCtrl+N", Keys::Control | Keys::N)
      ->Action.connect(Bind(&MainFrame::onNewWithSeed, this));
    gameMenu->addSeparator();
    gameMenu->add("&Exit")->Action.connect(Bind(&MainFrame::setVisible, this, false));

    MenuBar* menuBar = new MenuBar;
    menuBar->add(gameMenu);
    menuBar->add("&Help")->Action.connect(Bind(&MainFrame::onHelp, this));

    return menuBar;
  }
  
  //////////////////////////////////////////////////////////////////////
  // signal bindings

  void onNew()
  {
    generateGame(rand());

//     for (int c=0; c<0xffff; ++c) {
//       generateGame(c);
//       for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
// 	if (it->given) {
// 	  printf("%d", it->digit);
// 	}
// 	else {
// 	  printf(".");
// 	}
//       }
//       printf("   vaca (%d)\n", c);
//       fflush(stdout);
//     }
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

  void pushIntoHistory(cell& cell)
  {
    // push the current entry into history
    if ((cell.digit != 0) &&
	(std::find(cell.history.begin(),
		   cell.history.end(),
		   cell.digit) == cell.history.end())) {
      // history too big?
      if (cell.history.size() == 9)
	cell.history.erase(cell.history.begin());

      // put the new element
      cell.history.push_back(cell.digit);
    }
  }

  Rect getCellBounds(grid_iterator it)
  {
    Rect rc = getClientBounds();
    return Rect(rc.x + it.col()*rc.w/9,
		rc.y + it.row()*rc.h/9,
		rc.w/9+1, rc.h/9+1);
  }

  void updateWarnings()
  {
    // clear warnings
    std::for_each(m_puzzle.begin(), m_puzzle.end(), set_warning());
      
    // nine boxes, rows, and columns
    for (int c=0; c<9; c++) {
      compare_with_all(m_puzzle.box_begin(c), m_puzzle.box_end(c), set_warning());
      compare_with_all(m_puzzle.col_begin(c), m_puzzle.col_end(c), set_warning());
      compare_with_all(m_puzzle.row_begin(c), m_puzzle.row_end(c), set_warning());
    }
  }

  void checkWin()
  {
    for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it)
      if (it->digit == 0 || it->warning)
	return;

    msgBox("You win!!!", "Congratulations", MB_OK);

    // set all to given
    std::for_each(m_puzzle.begin(), m_puzzle.end(), set_given_to_true());
    invalidate(false);
  }

  void generateGame(int gameNumber)
  {
    srand(gameNumber);
    setText("Sudoku (Game " + String::fromInt(gameNumber) + ")");

    // no hot cell
    m_hotCell = m_puzzle.end();

    // reset the grid
    for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
      it->digit = 0;
      it->warning = false;
      it->given = false;
      it->history.clear();
    }

    // fill all boxes with all digits
    for (int digit=1; digit<=9; digit++)
      for (int box=0; box<9; box++)
	fillBoxWithDigit(box, digit);

    grid<cell> puzzle;

    // now we must create "the givens" (the exposed digits)
    do {
      // make a copy of the generated puzzle
      puzzle = m_puzzle;

      // make a vector to known where is each digit in the grid
      std::vector<grid_iterator> digits[9];
      for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it)
	if (it->digit != 0)
	  digits[it->digit-1].push_back(it);
    
      std::vector<int> givens_for_digit(9, 0);
      for (int i=0; i<31; i++) {
	int digit = rand()%9;
	while (true) {
	  if (givens_for_digit[digit] < 4) {
	    givens_for_digit[digit]++;
	    break;
	  }
	  digit = (digit+1)%9;
	}
      }

      for (int i=0; i<9; i++) {
	for (int j=0; j<givens_for_digit[i]; j++) {
	  grid_iterator it = digits[i][rand() % digits[i].size()];
	  remove_from_container(digits[i], it);
	  it->given = true;
	}
      }

      // set to 0 non-given cells
      for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it)
	if (!it->given)
	  it->digit = 0;

      // is the sudoku is not soluble, regenerate...
    } while (!isSoluble(puzzle));

    // set to 0 non-given cells
    for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it)
      if (!it->given)
	it->digit = 0;

    m_puzzle = puzzle;

    updateWarnings();
    invalidate(true);
  }
  
  int getGivensCountInBox(int box)
  {
    return std::count_if(m_puzzle.box_begin(box), m_puzzle.box_end(box),
			 if_given_is_true());
  }
  
  // put a digit in its 3x3 box
  void fillBoxWithDigit(int box, int digit)
  {
    int cellCount;		// current cell collisions
    int bestCount = -1;		// how many collisions has that cell
    grid_iterator bestCell;	// cell where are less collisions
    std::vector<grid_iterator> chances;

    for (box_iterator it = m_puzzle.box_begin(box);
	 it != m_puzzle.box_end(box); ++it) {
      if (it->digit == 0) {
	cellCount = countDigitCollisionsInRowAndColumn(it, digit);
	if (cellCount == 0)
	  chances.push_back(it);

	if (bestCount < 0 ||
	    bestCount > cellCount) {
	  bestCount = cellCount;
	  bestCell = it;
	}
      }
    }

    // excelent, we found a good place to put the digit
    if (!chances.empty()) {
      // put the digit in a random place
      chances[rand() % chances.size()]->digit = digit;
    }
    // bad news, we must swap cells to avoid the collision
    else {
      // put the digit
      bestCell->digit = digit;

      // swap cells
      fixCellCollisions(bestCell);
    }
  }
  
  int countDigitCollisionsInRowAndColumn(grid_iterator it, int digit)
  {
    int count =
      std::count_if(m_puzzle.col_begin(it.col()), m_puzzle.col_end(it.col()), is_digit(digit)) +
      std::count_if(m_puzzle.row_begin(it.row()), m_puzzle.row_end(it.row()), is_digit(digit));
    return count > 0 && it->digit == digit ? count-1: count;
  }

  // try to swap cells to fix the collision with the cell at (x,y)
  void fixCellCollisions(grid_iterator cell)
  {
    int col = cell.col();
    int row = cell.row();

    // look up 9 cells in the row that cross at "cell" position
    for (row_iterator it = m_puzzle.row_begin(row); it != m_puzzle.row_end(row); ++it) {
      if (it != cell && it->digit == cell->digit) {
	// find other row to swap
	for (int q=(row/3)*3; q<(row/3)*3+3; q++) {
 	  if (q != row) {
	    // make this cell temporary blank
	    int old_digit = it->digit;
	    it->digit = 0;

	    grid_iterator swapable = m_puzzle.at(it.col(), q);

	    // check if I move the cell in the y-row to the cell in
	    // the q-row, there aren't a collision
	    if (countDigitCollisionsInRowAndColumn(swapable, old_digit) == 0) {
	      // anyway, swap the cells
	      it->digit = swapable->digit;
	      swapable->digit = old_digit;

	      // well, now we should check if this movement doesn't
	      // make another warning...
	      if (it->digit != 0 &&
		  countDigitCollisionsInRowAndColumn(it, it->digit) > 0)
		fixCellCollisions(it);

	      break;
	    }
	    else
	      it->digit = old_digit;
	  }
	}
      }
    }

    for (col_iterator it = m_puzzle.col_begin(col); it != m_puzzle.col_end(col); ++it) {
      if (it != cell && it->digit == cell->digit) {
	// find other row to swap
	for (int q=(col/3)*3; q<(col/3)*3+3; q++) {
 	  if (q != col) {
	    // make this cell temporary blank
	    int old_digit = it->digit;
	    it->digit = 0;

	    grid_iterator swapable = m_puzzle.at(q, it.row());

	    // check if I move the cell in the y-row to the cell in
	    // the q-row, there aren't a collision
	    if (countDigitCollisionsInRowAndColumn(swapable, old_digit) == 0) {
	      // anyway, swap the cells
	      it->digit = swapable->digit;
	      swapable->digit = old_digit;

	      // well, now we should check if this movement doesn't
	      // make another warning...
	      if (it->digit != 0 &&
		  countDigitCollisionsInRowAndColumn(it, it->digit) > 0)
		fixCellCollisions(it);

	      break;
	    }
	    else
	      it->digit = old_digit;
	  }
	}
      }
    }
  }

  // basic Sudoku solver
  static bool isSoluble(grid<cell> puzzle)
  {
    bool continue_working;
    bool all_filled;

    do {
      for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it) {
	if (it->given)
	  continue;

	it->history.clear();

	for (int digit=1; digit<=9; ++digit) {
	  if (it->digit == digit)
	    continue;
	      
	  if (!exist(puzzle.col_begin(it.col()), puzzle.col_end(it.col()), is_digit(digit)) &&
	      !exist(puzzle.row_begin(it.row()), puzzle.row_end(it.row()), is_digit(digit)) &&
	      !exist(puzzle.box_begin(it.box()), puzzle.box_end(it.box()), is_digit(digit)))
	    it->history.push_back(digit);
	}
      }

      all_filled = true;
      continue_working = false;
      for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it) {
	if (!it->given && it->digit == 0) {
	  if (it->history.size() == 1) {
	    it->digit = it->history.front();
	    it->history.clear();
	    continue_working = true;
	  }
	  else
	    all_filled = false;
	}
      }
    } while (continue_working);

    return all_filled;
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  srand(static_cast<unsigned int>(time(NULL)));

  Example* app(new Example);
  app->run();
  delete app;
  return 0;
}
