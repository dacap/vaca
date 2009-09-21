// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#include "Vaca/Bix.h"
#include "Vaca/Point.h"
#include "Vaca/ParseException.h"
#include "Vaca/Widget.h"

#include <stack>
#include <cassert>
#include <algorithm>

using namespace Vaca;

#define MAIN_BIX_DEFAULT_BORDER		4
#define BIX_DEFAULT_BORDER		0
#define BIX_DEFAULT_CHILD_SPACING	4

/**
   @internal Internal element
*/
struct Bix::Element
{
  virtual ~Element() { }
  virtual int getFlags() = 0;
  virtual bool isLayoutFree() = 0;
  virtual Size getPreferredSize(const Size& fitIn) = 0;
  virtual void setBounds(Bix* bix, const Rect& rc) = 0;
};

/**
   @internal Internal matrix element
*/
struct Bix::SubBixElement : public Bix::Element
{
  Bix* bix;
  SubBixElement(Bix* b) { bix=b; }
  virtual ~SubBixElement() { delete bix; }
  virtual int getFlags() { return bix->m_flags; };
  virtual bool isLayoutFree() { return false; }
  virtual Size getPreferredSize(const Size& fitIn) { return bix->getPreferredSize(fitIn); }
  virtual void setBounds(Bix* parentBix, const Rect& rc) { bix->layout(parentBix, rc); }
};

/**
   @internal Internal widget element
*/
struct Bix::WidgetElement : public Bix::Element
{
  Widget* widget;
  int flags;
  WidgetElement(Widget* w, int f) {
    assert(w != NULL);
    widget=w;
    flags=f;
  }
  virtual ~WidgetElement() { }
  virtual int getFlags() { return flags; };
  virtual bool isLayoutFree() { return widget->isLayoutFree(); }
  virtual Size getPreferredSize(const Size& fitIn) { return widget->getPreferredSize(fitIn); }
  virtual void setBounds(Bix* parentBix, const Rect& rc) { parentBix->moveWidget(widget, rc); }
};

/**
   @internal Internal matrix to arrange elements
*/
struct Bix::Matrix
{
  int cols, rows;
  Element*** elem; // matrix of pointers "typeof(elem[y][x]) = Element *"
  Size** size;
  bool* col_fill;
  bool* row_fill;

  Matrix(int c, int r) {
    cols = c;
    rows = r;

    elem = new Element**[rows];
    size = new Size*[rows];
    for (int y=0; y<rows; ++y) {
      elem[y] = new Element*[cols];
      size[y] = new Size[cols];
      for (int x=0; x<cols; ++x)
	elem[y][x] = NULL;
    }

    col_fill = new bool[cols];
    for (int x=0; x<cols; ++x)
      col_fill[x] = false;

    row_fill = new bool[rows];
    for (int y=0; y<rows; ++y)
      row_fill[y] = false;

  }

  virtual ~Matrix() {
    for (int y=0; y<rows; ++y) {
      delete[] elem[y];
      delete[] size[y];
    }
    delete[] elem;
    delete[] size;

    delete[] col_fill;
    delete[] row_fill;
  }

  void setElementAt(int x, int y, Element* e) {
    elem[y][x] = e;
    if (e->getFlags() & BixFillX) col_fill[x] = true;
    if (e->getFlags() & BixFillY) row_fill[y] = true;
  }


  int getColFillsCount()
  {
    register int x, count = 0;

    for (x=0; x<cols; ++x)
      if (col_fill[x])
	++count;

    return count;
  }

  int getRowFillsCount()
  {
    register int y, count = 0;

    for (y=0; y<rows; ++y)
      if (row_fill[y])
	++count;

    return count;
  }

  void calcCellsSize(const Size& fitIn)
  {
    register int x, y;

    // get the preferred size of each element in the matrix
    for (y=0; y<rows; ++y)
      for (x=0; x<cols; ++x)
	if (elem[y][x] != NULL)
	  size[y][x] = elem[y][x]->getPreferredSize(fitIn);

    // fill the first row with the maximum width of each column
    for (x=0; x<cols; ++x)
      for (y=1; y<rows; ++y)
	if (size[0][x].w < size[y][x].w)
	  size[0][x].w = size[y][x].w;

    // fill the first column with the maximum height of each row
    for (y=0; y<rows; ++y)
      for (x=1; x<cols; ++x)
	if (size[y][0].h < size[y][x].h)
	  size[y][0].h = size[y][x].h;
  }

};

// ======================================================================
// Bix

Bix::Bix(int flags, int matrixColumns)
{
  m_flags = flags;
  m_cols = matrixColumns;
  m_border = BIX_DEFAULT_BORDER;
  m_childSpacing = BIX_DEFAULT_CHILD_SPACING;
}

Bix::~Bix()
{
  for (Elements::iterator it=m_elements.begin(); it!=m_elements.end(); ++it)
    delete *it;

  m_elements.clear();
}

/**
   Returns true if it's a horizontal Bix (row vector).
*/
bool Bix::isRow()
{
  return (m_flags & BixTypeMask) == BixRow;
}

/**
   Returns true if it's a vertical Bix (column vector).
*/
bool Bix::isCol()
{
  return (m_flags & BixTypeMask) == BixCol;
}

/**
   Returns true if this Bix is a matrix.
*/
bool Bix::isMat()
{
  return (m_flags & BixTypeMask) == BixMat;
}

bool Bix::isEvenX()
{
  return (m_flags & BixEvenX) == BixEvenX;
}

bool Bix::isEvenY()
{
  return (m_flags & BixEvenY) == BixEvenY;
}

bool Bix::isEven()
{
  return (m_flags & BixEven) == BixEven;
}

bool Bix::isFillX()
{
  return (m_flags & BixFillX) == BixFillX;
}

bool Bix::isFillY()
{
  return (m_flags & BixFillY) == BixFillY;
}

bool Bix::isFill()
{
  return (m_flags & BixFill) == BixFill;
}

int Bix::getBorder()
{
  return m_border;
}

void Bix::setBorder(int border)
{
  m_border = border;
}

/**
   Returns the space between each child.
*/
int Bix::getChildSpacing()
{
  return m_childSpacing;
}

/**
   Sets the space between each child.

   @param childSpacing
   	Spacing between one child and another (specified
   	in pixels)
*/
void Bix::setChildSpacing(int childSpacing)
{
  m_childSpacing = childSpacing;
}

int Bix::getMatrixColumns()
{
  return m_cols;
}

void Bix::setMatrixColumns(int matrixColumns)
{
  m_cols = matrixColumns;
}

/**
   Adds a subbix (a child) with the specified flags.

   @param flags
   	TODO docme

   @param matrixColumns
   	TODO docme

   @warning The returned Bix can't be deleted (it's automatically
	    deleted by the parent bix).
*/
Bix* Bix::add(int flags, int matrixColumns)
{
  Bix* subbix = new Bix(flags, matrixColumns);
  m_elements.push_back(new SubBixElement(subbix));
  return subbix;
}

void Bix::add(Widget* child, int flags)
{
  m_elements.push_back(new WidgetElement(child, flags));
}

void Bix::remove(Bix* subbix)
{
  Elements::iterator it;

  for (it = m_elements.begin(); it != m_elements.end(); ++it) {
    SubBixElement* element = dynamic_cast<SubBixElement*>(*it);
    if (element != NULL &&
	element->bix == subbix) {
      remove_from_container(m_elements, element);
      return;
    }
  }

  // the "child" doesn't exist
  assert(false);
}

void Bix::remove(Widget* child)
{
  Elements::iterator it;

  for (it = m_elements.begin(); it != m_elements.end(); ++it) {
    WidgetElement* element = dynamic_cast<WidgetElement*>(*it);
    if (element != NULL &&
	element->widget == child) {
      remove_from_container(m_elements, element);
      return;
    }
  }

  // the "child" doesn't exist
  assert(false);
}

Size Bix::getPreferredSize(Widget* parent, WidgetList& widgets, const Size& fitIn)
{
  return getPreferredSize(fitIn);
}

void Bix::layout(Widget* parent, WidgetList& widgets, const Rect& rc)
{
  beginMovement(widgets);
  layout(this, rc);
  endMovement();
}

Size Bix::getPreferredSize(const Size& fitIn)
{
  Size matDim = getMatrixDimension();
  if (matDim.w > 0 && matDim.h > 0) {
    Matrix mat(matDim.w, matDim.h);

    fillMatrix(mat);
    mat.calcCellsSize(fitIn);

    return getPreferredSize(mat);
  }
  else
    return Size(0, 0);
}

Size Bix::getPreferredSize(Matrix& mat)
{
  Size sz;

  // X axis
  if (isEvenX()) {
    int max_w = 0;
    for (int x=0; x<mat.cols; ++x) {
      int w = mat.size[0][x].w;
      max_w = max_value(max_w, w);
    }
    sz.w = max_w*mat.cols;
  }
  else {
    for (int x=0; x<mat.cols; ++x)
      sz.w += mat.size[0][x].w;
  }

  // Y axis
  if (isEvenY()) {
    int max_h = 0;
    for (int y=0; y<mat.rows; ++y) {
      int h = mat.size[0][y].h;
      max_h = max_value(max_h, h);
    }
    sz.h = max_h*mat.rows;
  }
  else {
    for (int y=0; y<mat.rows; ++y)
      sz.h += mat.size[y][0].h;
  }

  sz.w += m_border*2 + m_childSpacing*(mat.cols-1);
  sz.h += m_border*2 + m_childSpacing*(mat.rows-1);

  return sz;
}

void Bix::layout(Bix* parentBix, const Rect& rc)
{
  Size matDim = getMatrixDimension();
  if (matDim.w > 0 && matDim.h > 0) {
    Matrix mat(matDim.w, matDim.h);

    fillMatrix(mat);
    mat.calcCellsSize(Size(0, 0));

    // distribute size
    register int x, y;

    // X axis
    if (isEvenX()) {
      int remainderWidth = rc.w - m_border*2 - m_childSpacing*(mat.cols-1);
      int elemWidth = remainderWidth / mat.cols;

      for (x=0; x<mat.cols; ++x) {
	if (x == mat.cols-1)
	  mat.size[0][x].w = remainderWidth;
	else {
	  mat.size[0][x].w = elemWidth;
	  remainderWidth -= elemWidth;
	}
      }
    }
    else {
      int colFills = mat.getColFillsCount();
      if (colFills > 0) {
	Size pref = getPreferredSize(mat);

	int remainderWidth = rc.w - pref.w;
	int extraElemWidth = remainderWidth / colFills;

	for (x=0; x<mat.cols; ++x) {
	  if (mat.col_fill[x]) {
	    if (x == mat.cols-1)
	      mat.size[0][x].w += remainderWidth;
	    else {
	      mat.size[0][x].w += extraElemWidth;
	      remainderWidth -= extraElemWidth;
	    }
	  }
	}
      }
    }

    // Y axis
    if (isEvenY()) {
      int remainderHeight = rc.h - m_border*2 - m_childSpacing*(mat.rows-1);
      int elemHeight = remainderHeight / mat.rows;

      for (y=0; y<mat.rows; ++y) {
	if (y == mat.rows-1)
	  mat.size[y][0].h = remainderHeight;
	else {
	  mat.size[y][0].h = elemHeight;
	  remainderHeight -= elemHeight;
	}
      }
    }
    else {
      int rowFills = mat.getRowFillsCount();
      if (rowFills > 0) {
	Size pref = getPreferredSize(mat);

	int remainderHeight = rc.h - pref.h;
	int extraElemHeight = remainderHeight / rowFills;

	for (y=0; y<mat.rows; ++y) {
	  if (mat.row_fill[y]) {
	    if (y == mat.rows-1)
	      mat.size[y][0].h += remainderHeight;
	    else {
	      mat.size[y][0].h += extraElemHeight;
	      remainderHeight -= extraElemHeight;
	    }
	  }
	}
      }
    }

    // setup child bounds
    Point pt(0, rc.y+m_border);

    for (y=0; y<mat.rows; ++y) {
      pt.x = rc.x+m_border;
      for (x=0; x<mat.cols; ++x) {
	if (mat.elem[y][x] != NULL)
	  mat.elem[y][x]->setBounds(parentBix,
				    Rect(pt, Size(mat.size[0][x].w,
						  mat.size[y][0].h)));

	pt.x += mat.size[0][x].w+m_childSpacing;
      }
      pt.y += mat.size[y][0].h+m_childSpacing;
    }
  }
}

Size Bix::getMatrixDimension()
{
  int cols = 0, rows = 0;

  switch (m_flags & BixTypeMask) {

    // a row
    case BixRow: {
      cols = 0;
      rows = 1;

      for (Elements::iterator it=m_elements.begin(); it!=m_elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  cols++;
      }
      break;
    }

    // a column
    case BixCol: {
      cols = 1;
      rows = 0;

      for (Elements::iterator it=m_elements.begin(); it!=m_elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  rows++;
      }
      break;
    }

    // a matrix
    case BixMat: {
      cols = m_cols;
      rows = 0;

      int x = 0;

      for (Elements::iterator it=m_elements.begin(); it!=m_elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  ++x;

	if (x == cols) {
	  x = 0;
	  ++rows;
	}
      }

      if (x > 0) {
	++rows;
      }
      break;
    }
  }

  return Size(cols, rows);
}

void Bix::fillMatrix(Matrix& mat)
{
  switch (m_flags & BixTypeMask) {

    // a row
    case BixRow: {
      int x = 0;

      for (Elements::iterator it=m_elements.begin(); it!=m_elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  mat.setElementAt(x++, 0, *it);
      }

      mat.row_fill[0] = true;
      break;
    }

    // a column
    case BixCol: {
      int y = 0;

      for (Elements::iterator it=m_elements.begin(); it!=m_elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  mat.setElementAt(0, y++, *it);
      }

      mat.col_fill[0] = true;
      break;
    }

    // a matrix
    case BixMat: {
      int x = 0, y = 0;

      for (Elements::iterator it=m_elements.begin(); it!=m_elements.end(); ++it) {
	if (!(*it)->isLayoutFree()) {
	  mat.setElementAt(x, y, *it);
	  ++x;
	}

	if (x == mat.cols) {
	  x = 0;
	  ++y;
	}
      }
      break;
    }
  }
}

// ==================================================================
//                         String Parser
// ==================================================================

/**
   @brief Creates a complete Bix with a specific formatted-string and
	  a list of widgets.

   Format of the string:
   @li "X[...]"    Creates a row-vector to distribute components in horizontal way, each one separated by comma (,).
   @li "Y[...]"    Creates a column-vector to distribute components in vertical way.
   @li "XY[...]"   Creates a matrix to distribute components in a matricial way (each column separated with ',' and each row with ';').
   @li "%"         Gets a widget from the @c ... paramenters.
   @li "f..."      Activates the BixFill flag for the next element (e.g.: "fX[...]" or "f%").
   @li "fx..."     Activates the BixFillX flag for the next element.
   @li "fy..."     Activates the BixFillY flag for the next element.
   @li "e..."      Activates the BixEven flag for the next element (e.g.: "eY[...]" or "e%").
   @li "ex..."     Activates the BixEvenX flag for the next element.
   @li "ey..."     Activates the BixEvenY flag for the next element.

   Example:
   @code
   Dialog dlg("Test");
   Label nameL("Username:", &dlg);
   Label passL("Password:", &dlg);
   TextEdit name("", &dlg);
   TextEdit pass("", &dlg, TextEdit::Styles::Password);
   Button ok("OK", &dlg);
   Button cancel("Cancel", &dlg);

   name.setPreferredSize(128, name.getPreferredSize().h);
   pass.setPreferredSize(128, pass.getPreferredSize().h);

   // See explanation below
   dlg.setLayout(Bix::parse("Y[XY[%,f%;%,f%],X[fX[],eX[%,%]]]",
			    &nameL, &name,
			    &passL, &pass,
			    &ok, &cancel));

   dlg.setSize(dlg.getPreferredSize());
   dlg.setVisible(true);
   @endcode
   In this case the @a fmt = @c "Y[XY[%,f%;%,f%],X[fX[],eX[%,%]]]", which
   means:
   @li the first "Y[...]" is a column, so the next two elements ("XY[...],X[...]")
       will be arranged one below the other.
   @li then "XY[%,f%;%,f%]" is a grid of 2x2, where each '%' is a reference
       to the next widget in the ... arguments (in this case @c nameL,
       @c name, @c passL, and @c pass)
   @li "X[fX[],eX[%,%]]" is a row with two elements, the first one is
       a dummy filler "fX[]", that "eats" the left-side available space,
       then "eX[%,%]" is a sub-row that arranges two widgets (@c ok, @c cancel)
       with same width and height ('e' means BixEven), so both buttons will
       have the same size.

   @throw ParseException
     Thrown when the syntax of the string @a fmt is ill-formed.
*/
Bix* Bix::parse(const Char* fmt, ...)
{
#define PARSE_ASSERT(condition, error)				\
  if (!(condition))						\
    throw ParseException(error, n_line, n_column, fmt-p);

#define NEW_BIX(flags)							\
  if (mainBix == NULL)							\
    mainBix = newBix = new Bix(flags | fill);				\
  else									\
    newBix = bixes.top()->add(flags | fill);				\
  bixes.push(newBix);							\
  columns.push(new int(0));

#define ADVANCE()				\
  ++n_column, ++p

  // ----------------------------------------

  Bix* mainBix = NULL;		// first Bix created
  Bix* newBix = NULL;		// current new Bix
  std::stack<Bix*> bixes;	// current stack of bixes
  std::stack<int*> columns;	// columns for bixes
  int fill = 0;			// want to fill next widget/bix
  bool expectClose = false;	// true is the next token must be a comma or a closing-parenthesis
  va_list ap;
  int n_line = 1, n_column = 0;

  va_start(ap, fmt);
  try {
    const Char* p;
    for (p=fmt; *p; ADVANCE()) {
      // is not a space character...
      if (!isspace(*p)) {
	// expect a close?
	if (expectClose) {
	  PARSE_ASSERT((*p == L',' || *p == L';' || *p == L']'), L"',' or ';' or ']' expected");
	  expectClose = false;
	}

	switch (*p) {

	  // widget
	  case L'%':
	    PARSE_ASSERT(!bixes.empty(), L"Bix expected before '%'");

	    (*columns.top())++;

	    bixes.top()->add(va_arg(ap, Widget*), fill);

	    expectClose = true;
	    fill = 0;
	    break;

	    // fill
	  case L'f':
	    if (p[1] == L'x') {
	      fill = BixFillX;
	      ADVANCE();
	    }
	    else if (p[1] == L'y') {
	      fill = BixFillY;
	      ADVANCE();
	    }
	    else {
	      fill = BixFill;
	    }
	    break;

	    // even
	  case L'e':
	    if (p[1] == L'x') {
	      fill = BixEvenX;
	      ADVANCE();
	    }
	    else if (p[1] == L'y') {
	      fill = BixEvenY;
	      ADVANCE();
	    }
	    else {
	      fill = BixEven;
	    }
	    break;

	    // row or matrix
	  case L'X':
	    if (mainBix != NULL)
	      (*columns.top())++;

	    // matrix
	    if (p[1] == L'Y') {
	      PARSE_ASSERT(p[2] == L'[', L"'[' expected after 'XY' to open the matrix");
	      NEW_BIX(BixMat);
	      p += 2;
	    }
	    // row
	    else {
	      PARSE_ASSERT(p[1] == L'[', L"'[' expected after 'X' to open the row");
	      NEW_BIX(BixRow);
	      ADVANCE();
	    }
	    fill = 0;
	    break;

	    // column
	  case L'Y':
	    if (mainBix != NULL)
	      (*columns.top())++;

	    PARSE_ASSERT(p[1] == L'[', L"'[' expected after 'Y' to open the column");
	    NEW_BIX(BixCol);
	    ADVANCE();

	    fill = 0;
	    break;

	    // pop a bix from the stack
	  case L']':
	    bixes.pop();

	    delete columns.top();
	    columns.pop();

	    expectClose = true;
	    break;

	  case L',':
	    PARSE_ASSERT(!bixes.empty(), L"Bix expected before ','");
	    break;

	    // row separator
	  case L';':
	    PARSE_ASSERT(!bixes.empty(), L"Bix expected before ';'");

	    bixes.top()->setMatrixColumns(*columns.top());
	    (*columns.top()) = 0;
	    break;

	    // do nothing
	  case L' ':
	    break;
	}
      }
      else if (*p == L'\n') {
	++n_line;
	n_column = 0;
      }
    }
    PARSE_ASSERT(bixes.empty(), L"']' expected to close Bixes before end of string");
  }
  catch (...) {
    delete mainBix;

    while (!columns.empty()) {
      delete columns.top();
      columns.pop();
    }

    va_end(ap);
    throw;
  }
  va_end(ap);

  mainBix->setBorder(MAIN_BIX_DEFAULT_BORDER);
  return mainBix;
}
