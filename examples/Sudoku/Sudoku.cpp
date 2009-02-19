// Vaca - Visual Application Components Abstraction
// Copyright (C) 2005-2009 David Capello
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

#include <algorithm>
#include <time.h>

#include <Vaca/Vaca.h>
#include "grid.h"

using namespace Vaca;

enum {
  ID_GAME_NEW = 1000,
  ID_GAME_NEW_WITH_SEED,
  ID_GAME_USE_HELPER,
  ID_GAME_EXIT,
  ID_HELP,
};

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
// Dialog to insert the game seed

class InsertSeedDialog : public Dialog
{
  Panel m_top;
  Panel m_bottom;
  Label m_seedLabel;
  TextEdit m_seedEdit;
  Button m_ok;
  Button m_cancel;

public:

  InsertSeedDialog(Widget* parent)
    : Dialog(L"Game Seed", parent)
    , m_top(this)
    , m_bottom(this)
    , m_seedLabel(L"Seed:", &m_top)
    , m_seedEdit(convert_to<String>(rand()), &m_top)
    , m_ok(L"OK", IDOK, &m_bottom)
    , m_cancel(L"Cancel", IDCANCEL, &m_bottom)
  {
    setLayout(new BoxLayout(Orientation::Vertical, true));
    m_top.setLayout(new BoxLayout(Orientation::Horizontal, false, 0));
    m_bottom.setLayout(new BoxLayout(Orientation::Horizontal, true, 0));
    m_seedEdit.setConstraint(new BoxConstraint(true));

    m_seedEdit.requestFocus();
    m_seedEdit.selectAll();

    setSize(getPreferredSize());
    center();
  }

  int getSeed() {
    return convert_to<int>(m_seedEdit.getText());
  }

};

//////////////////////////////////////////////////////////////////////
// Help Dialog

class HelpDialog : public Dialog
{
  RichEdit m_help;
  Button m_close;

public:

  HelpDialog(Widget* parent)
    : Dialog(L"Help", parent)
    , m_help(L"", this, RichEdit::Styles::Default
			+ RichEdit::Styles::ReadOnly
			- Widget::Styles::HorizontalScroll
			- Widget::Styles::ClientEdge)
    , m_close(L"Close", IDOK, this)
  {
    setLayout(Bix::parse(L"Y[f%,X[fX[],%,fX[]]]", &m_help, &m_close));

    m_close.setDefault(true);

    m_help.requestFocus();
    m_help.setBgColor(getBgColor());
    m_help.setAutoUrlDetect(true);
    m_help.setText(L"The objective is to fill a 9\x00D7\x0039 grid so that each column, "
		   L"each row, and each of the nine 3\x00D7\x0033 boxes (also called "
		   L"blocks or regions) contains the digits from 1 to 9 only one time each. [http://en.wikipedia.org/wiki/Sudoku]");

    setSize(Size(400, 128));
    center();
  }

};

//////////////////////////////////////////////////////////////////////
//

class CellEdit : public TextEdit
{
public:

  CellEdit(int maxChars, Font font, Widget* parent)
    : TextEdit(L"", parent, TextEdit::Styles::Default
			    - Widget::Styles::Visible
			    - Widget::Styles::ClientEdge)
  {
    setFont(font);
    setTextLimit(maxChars);
  }

  void setDigit(int digit)
  {
    setText(digit == 0 ? L"": convert_to<String>(digit));
    selectAll();
  }

  int getDigit()
  {
    return getTextLength() == 1 ? convert_to<int>(getText()): 0;
  }

  void setCandidates(const std::vector<int>& candidates)
  {
    String result;
    for (int i=0; i<candidates.size(); ++i)
      result += convert_to<String>(candidates[i]);
    setText(result);
    selectRange(getTextLength(), getTextLength());
  }

  std::vector<int> getCandidates() const
  {
    String text = getText();
    std::vector<int> result;
    for (int i=0; i<text.size(); ++i)
      result.push_back(text[i] - L'0');
    return result;
  }

  int getFontHeight()
  {
    ScreenGraphics g;
    g.setFont(getFont());
    return g.measureString(L" ").h;
  }
  
  Signal0<void> Enter;
  
protected:
  virtual void onKeyDown(KeyEvent& ev)
  {
    if ((ev.getCharCode() != 0) &&
	!(ev.getCharCode() >= '0' && ev.getCharCode() <= '9') &&
	(ev.getCharCode() >= ' ')) {
      ev.consume();
    }
    else if (ev.getKeyCode() == Keys::Enter) {
      Enter();
      ev.consume();
    }
    else
      TextEdit::onKeyDown(ev);
  }
  virtual void onLostFocus(Event& ev)
  {
    TextEdit::onLostFocus(ev);
    Enter();
  }
};

//////////////////////////////////////////////////////////////////////
// The Sudoku window

class MainFrame : public Frame
		, public CommandsClient
{
public:	// MSVC needs it to be public because grid<cell> needs to access to it
  ////////////////////////////////////////
  // a cell
  struct cell {
    int digit;
    bool given;
    bool warning;
    std::vector<int> candidates;
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
  grid_iterator m_editingCell;
  Font m_font;			     // font for the digits
  Font m_fontSmall;		     // font for the digits of candidates
  Font m_fontBold;		     // font for the warnings and for same-digits
  CellEdit m_valueEdit;
  CellEdit m_candidatesEdit;
  bool m_done;
  bool m_useHelper;

public:

  // create the sudoku window
  MainFrame()
    : Frame(L"Sudoku", NULL, Frame::Styles::Default
			     - Frame::Styles::Maximizable // can't be maximized
			     - Frame::Styles::Resizable)  // can't change the size
    , m_font(L"Verdana", 10)
    , m_fontSmall(L"Verdana", 7)
    , m_fontBold(L"Verdana", 10, FontStyle::Bold)
    , m_valueEdit(1, m_font, this)
    , m_candidatesEdit(10, m_fontSmall, this)
  {
    m_hotCell = m_puzzle.end();
    m_editingCell = m_puzzle.end();
    m_useHelper = false;

    setBgColor(Color::White);
    setMenuBar(createMenuBar());

    // commands
    SignalCommand* useHelperCmd;
    addCommand(new SignalCommand(ID_GAME_NEW, &MainFrame::onNew, this));
    addCommand(new SignalCommand(ID_GAME_NEW_WITH_SEED, &MainFrame::onNewWithSeed, this));
    addCommand(useHelperCmd =
	       new SignalCommand(ID_GAME_USE_HELPER, &MainFrame::onUseHelper, this));
    addCommand(new SignalCommand(ID_GAME_EXIT, Bind(&MainFrame::setVisible, this, false)));
    addCommand(new SignalCommand(ID_HELP, &MainFrame::onHelp, this));

    useHelperCmd->Checked.connect(Bind(&MainFrame::isUseHelper, this));

    // signals
    m_valueEdit.Enter.connect(Bind(&MainFrame::onCellEnter, this));
    m_valueEdit.Change.connect(Bind(&MainFrame::onCellChange, this));
    m_candidatesEdit.Enter.connect(Bind(&MainFrame::onCellEnter, this));
    m_candidatesEdit.Change.connect(Bind(&MainFrame::onCellChange, this));

    setDoubleBuffered(true);
    setSize(getNonClientSize()+Size(40*9, 40*9));
    center();

    onNew();
  }

protected:

  // paint the client area
  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();

    int hotDigit = 0;
    if (m_hotCell != m_puzzle.end())
      hotDigit = m_hotCell->digit;
    else if (m_editingCell != m_puzzle.end())
      hotDigit = m_editingCell->digit;

    // paint the 9x9 grid
    for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
      Rect cellBounds = getCellBounds(it);
      Color color;

      // cell background color
      if (it == m_editingCell)
	color = Color::White;
      else if (it->warning)
	color = Color::Red;
      else if (it->given)
	color = Color(240, 240, 240);
      else
	color = Color::White;

      // paint cell background
      Brush brush(color);
      g.fillRect(brush, cellBounds);

      // paint helper
      if (m_useHelper && hotDigit != 0 && !it->warning) {
	Brush b2(Color(255, 255, 200));

	if (exist(m_puzzle.box_begin(it.box()), m_puzzle.box_end(it.box()), is_digit(hotDigit)))
	  g.fillRect(b2, cellBounds);
	else {
	  if (exist(m_puzzle.col_begin(it.col()), m_puzzle.col_end(it.col()), is_digit(hotDigit)))
	    g.fillRect(b2, Rect(cellBounds.x+cellBounds.w/4,
				cellBounds.y,
				cellBounds.w/2,
				cellBounds.h));

	  if (exist(m_puzzle.row_begin(it.row()), m_puzzle.row_end(it.row()), is_digit(hotDigit)))
	    g.fillRect(b2, Rect(cellBounds.x,
				cellBounds.y+cellBounds.h/4,
				cellBounds.w,
				cellBounds.h/2));
	}
      }

      // paint text inside the cell
      if (it->digit != 0 || !it->candidates.empty()) {
	Color textColor;

	// editing
	if (it == m_editingCell) {
	  color = Color::Black;
	  g.setFont(m_font);
	}
	// warning
	else if (it->warning) {
	  textColor = Color::White;
	  g.setFont(m_fontBold);
	}
	// hot or normal
	else {
	  textColor = Color::Black;
	  g.setFont(it->digit == hotDigit ? m_fontBold: m_font);
	}

	if (it->digit != 0) {
	  String text = convert_to<String>(it->digit);
	  Size textSize = g.measureString(text);
	  g.drawString(text, textColor,
		       cellBounds.x + (cellBounds.w - textSize.w)/2,
		       cellBounds.y + (cellBounds.h - textSize.h)/2);
	}

	if (!it->candidates.empty() &&
	    (it != m_editingCell || !m_candidatesEdit.isVisible())) {
	  g.setFont(m_fontSmall);
	  if (!it->warning)
	    textColor = Color(190, 190, 190);

	  Point offset(1, 1);

	  for (int i=0; i<it->candidates.size(); ++i) {
	    String text = convert_to<String>(it->candidates[i]);
	    Size textSize = g.measureString(text);

	    g.drawString(text, textColor, cellBounds.getOrigin()+offset);

	    if (i == 4) {
	      offset.x = 2;
	      offset.y = cellBounds.h - textSize.h - 1;
	    }
	    else {
	      offset.x += textSize.w;
	    }
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

  virtual void onMouseDown(MouseEvent &ev)
  {
    if ((m_hotCell != m_puzzle.end()) &&
	(!m_hotCell->given)) {

      // restore the digit of the cell that was edited
      if (m_editingCell != m_puzzle.end())
	onCellEnter();

      // prepare the cell editor for the selected cell
      m_editingCell = m_hotCell;

      if (ev.getPoint().y > getCellBounds(m_editingCell).y+8) {
	Rect rc = getCellBounds(m_editingCell);

	m_valueEdit.setDigit(m_editingCell->digit);
	m_valueEdit.setBounds(Rect(rc.x+rc.w/3, rc.y+rc.h/3-2,
				   rc.w/3, m_valueEdit.getFontHeight()));
	m_valueEdit.setVisible(true);
	m_valueEdit.requestFocus();
      }
      else {
	Rect rc = getCellBounds(m_editingCell).shrink(2);
	rc.h = m_candidatesEdit.getFontHeight();

	m_candidatesEdit.setCandidates(m_editingCell->candidates);
	m_candidatesEdit.setBounds(rc);
	m_candidatesEdit.setVisible(true);
	m_candidatesEdit.requestFocus();
      }

      invalidate(true);
    }
  }

private:

  MenuBar* createMenuBar()
  {
    MenuBar* menuBar = new MenuBar;
    Menu* gameMenu = new Menu(L"&Game");

    gameMenu->add(L"&New\tCtrl+Shift+N", ID_GAME_NEW, Keys::Control | Keys::Shift | Keys::N);
    gameMenu->add(L"New with &seed\tCtrl+N", ID_GAME_NEW_WITH_SEED, Keys::Control | Keys::N);
    gameMenu->addSeparator();
    gameMenu->add(L"Use &Helper", ID_GAME_USE_HELPER);
    gameMenu->addSeparator();
    gameMenu->add(L"&Exit", ID_GAME_EXIT);

    menuBar->add(gameMenu);
    menuBar->add(L"&Help", ID_HELP);
    return menuBar;
  }
  
  //////////////////////////////////////////////////////////////////////
  // signal bindings

  void onNew()
  {
    noEdit();

    generateGame(rand());
  }

  void onNewWithSeed()
  {
    noEdit();

    // create the dialog
    InsertSeedDialog dlg(this);

    // show the dialog, and if the user press ok...
    if (dlg.doModal())
      // start a new game with the specified seed
      generateGame(dlg.getSeed());
  }

  void onUseHelper()
  {
    m_useHelper = !m_useHelper;
  }

  void onHelp()
  {
    HelpDialog dlg(this);
    dlg.doModal();
  }

  void onCellEnter()
  {
    onCellChange();
    noEdit();
  }

  void onCellChange()
  {
    assert(m_editingCell != m_puzzle.end());

    if (m_valueEdit.isVisible()) {
      m_editingCell->digit = m_valueEdit.getDigit();
    }
    else if (m_candidatesEdit.isVisible()) {
      m_editingCell->candidates = m_candidatesEdit.getCandidates();
    }

    updateWarnings();
    checkWin();
    invalidate(true);
  }

  //////////////////////////////////////////////////////////////////////

  bool isUseHelper()
  {
    return m_useHelper;
  }

  void noEdit()
  {
    m_valueEdit.setVisible(false);
    m_candidatesEdit.setVisible(false);
    m_editingCell = m_puzzle.end();

    requestFocus();
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
    if (m_done)
      return;

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
    if (m_done)
      return;

    for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
      if (it->digit == 0 || it->warning)
	return;
    }

    m_done = true;
    noEdit();

    MsgBox::show(this, L"Congratulations", L"You win!!!");

    // set all to given
    std::for_each(m_puzzle.begin(), m_puzzle.end(), set_given_to_true());
    invalidate(false);
  }

  void generateGame(int gameNumber)
  {
    m_done = false;

    srand(gameNumber);

    // no hot cell
    m_hotCell = m_puzzle.end();

    // reset the grid
    for (grid_iterator it = m_puzzle.begin(); it != m_puzzle.end(); ++it) {
      it->digit = 0;
      it->warning = false;
      it->given = false;
      it->candidates.clear();
    }

    // fill all boxes with all digits
    for (int digit=1; digit<=9; digit++)
      for (int box=0; box<9; box++)
	fillBoxWithDigit(box, digit);

    // first: all givens
    grid<cell> puzzle = m_puzzle;
    for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it)
      it->given = true;

    // make a vector to known where is each digit in the grid
    std::vector<grid_iterator> digits[9];
    for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it)
      digits[it->digit-1].push_back(it);

    int givens_of_digit[9];
    for (int i=0; i<9; ++i)
      givens_of_digit[i] = 9;

    // now we must create "the givens" (the exposed digits)
    for (int i=81; i>=17; --i) {
      int tries, maxTries = 100-i;

      grid<cell> bak_puzzle = puzzle;

      for (tries=0; tries<maxTries; ++tries) {
	// hide one "given"
	grid_iterator it;
	{
	  int digit = rand()%9;
	  while (givens_of_digit[digit] < i/9)
	    digit = (digit+1) % 9;

	  int pos = rand()%9;
	  do {
	    it = digits[digit][pos];
	    pos = (pos+1)%9;
	  } while (!it->given);
	}

	int given_digit = it->digit;
	givens_of_digit[given_digit-1]--;

	it->given = false;
	it->digit = 0;

	if (isSoluble(puzzle))
	  break;

	givens_of_digit[given_digit-1]++;
	puzzle = bak_puzzle;
      }

      if (tries == maxTries)
	break;

      // is the sudoku is not soluble, regenerate...
    }

    // set to 0 non-given cells
    for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it)
      if (!it->given)
	it->digit = 0;

    m_puzzle = puzzle;

    setText(format_string(L"Sudoku (Game %d with %d clues)",
			  gameNumber, getGivenCount()));

    updateWarnings();
    invalidate(true);
  }
  
  int getGivenCount()
  {
    return std::count_if(m_puzzle.begin(), m_puzzle.end(), if_given_is_true());
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

    // fill possible candidates of every cell
    for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it) {
      if (it->given)
	continue;

      it->candidates.clear();

      for (int digit=1; digit<=9; ++digit) {
	if (it->digit == digit)
	  continue;

	if (!isInScope(puzzle, it, digit))
	  it->candidates.push_back(digit);
      }
    }

    // now we can consume candidates one by one (only if there are
    // cells with just one candidate)
    do {
      all_filled = true;
      continue_working = false;

      for (grid_iterator it = puzzle.begin(); it != puzzle.end(); ++it) {
	if (!it->given && it->digit == 0) {
	  if (it->candidates.size() == 1) {
	    it->digit = it->candidates.front();
	    it->candidates.clear();

	    clearCandidateFromScope(puzzle, it);
	    continue_working = true;
	  }
	  else
	    all_filled = false;
	}
      }
    } while (continue_working);

    return all_filled;
  }

  static bool isInScope(grid<cell>& puzzle, grid_iterator it, int digit)
  {
    return
      exist(puzzle.col_begin(it.col()), puzzle.col_end(it.col()), is_digit(digit)) ||
      exist(puzzle.row_begin(it.row()), puzzle.row_end(it.row()), is_digit(digit)) ||
      exist(puzzle.box_begin(it.box()), puzzle.box_end(it.box()), is_digit(digit));
  }

  static void clearCandidateFromScope(grid<cell>& puzzle, grid_iterator pos)
  {
    for (box_iterator it=puzzle.box_begin(pos.box()); it!=puzzle.box_end(pos.box()); ++it)
      remove_from_container(it->candidates, pos->digit);

    for (col_iterator it=puzzle.col_begin(pos.col()); it!=puzzle.col_end(pos.col()); ++it)
      remove_from_container(it->candidates, pos->digit);

    for (row_iterator it=puzzle.row_begin(pos.row()); it!=puzzle.row_end(pos.row()); ++it)
      remove_from_container(it->candidates, pos->digit);
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  srand(static_cast<unsigned int>(time(NULL)));

  Application app;
  MainFrame frm;
  frm.setVisible(true);
  app.run();
  return 0;
}
