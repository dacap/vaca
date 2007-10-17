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

#include <Vaca/Vaca.h>

using namespace Vaca;

#if 0

//////////////////////////////////////////////////////////////////////

class Bix : public Layout
{

  struct Element;
  struct SubBixElement;
  struct WidgetElement;
  struct Matrix;

  typedef std::vector<Element *> Elements;

  int mFlags;
  int mCols;
  int mBorder;
  int mChildSpacing;
  Elements elements;
  
public:

  enum {
    // flags: type
    TypeMask = 3,
    Row = 1,			// row vector (horizontal, x axis)
    Col = 2,			// column vector (vertical, y axis)
    Mat = 3,			// matrix (both axis)
    // flags: modifiers
    EvenX = 4,
    EvenY = 8,
    Even = EvenX | EvenY,
    FillX = 16,
    FillY = 32,
    Fill = FillX | FillY,
  };

  Bix(Widget *container, int flags, int matrixColumns = 0);

  bool isRow();
  bool isCol();
  bool isMat();
  bool isEvenX();
  bool isEvenY();
  bool isEven();
  bool isFillX();
  bool isFillY();
  bool isFill();

  int getBorder();
  void setBorder(int border);

  int getChildSpacing();
  void setChildSpacing(int childSpacing);

  Bix *addBix(int flags, int matrixColumns = 0);
  void add(Widget *child, int flags = 0);
  
  virtual Size getPreferredSize(Widget *parent, Widget::Container &widgets, const Size &fitIn);

protected:

  virtual void layout(Widget *parent, Widget::Container &widgets, const Rect &rc);

private:
  
  Size getPreferredSize(const Size &fitIn);
  Size getPreferredSize(Matrix &mat);
  void layout(const Rect &rc);
  
  Size getMatrixDimension();
  void fillMatrix(Matrix &mat);
  void calcCellsSize(Matrix &mat, const Size &fitIn);
  int getColFillsCount(Matrix &mat);
  int getRowFillsCount(Matrix &mat);

};

//////////////////////////////////////////////////////////////////////

struct Bix::Element
{
  virtual int getFlags() = 0;
  virtual bool isLayoutFree() = 0;
  virtual Size getPreferredSize(const Size &fitIn) = 0;
  virtual void setBounds(const Rect &rc) = 0;
};

struct Bix::SubBixElement : public Bix::Element
{
  Bix *bix;
  SubBixElement(Bix *b) { bix=b; }
  virtual int getFlags() { return bix->mFlags; };
  virtual bool isLayoutFree() { return false; }
  virtual Size getPreferredSize(const Size &fitIn) { return bix->getPreferredSize(fitIn); }
  virtual void setBounds(const Rect &rc) { bix->layout(rc); }
};

struct Bix::WidgetElement : public Bix::Element
{
  Widget *widget;
  int flags;
  WidgetElement(Widget *w, int f) { widget=w; flags=f; }
  virtual int getFlags() { return flags; };
  virtual bool isLayoutFree() { return widget->isLayoutFree(); }
  virtual Size getPreferredSize(const Size &fitIn) { return widget->getPreferredSize(fitIn); }
  virtual void setBounds(const Rect &rc) { widget->setBounds(rc); }
};

struct Bix::Matrix
{
  int cols, rows;
  Element ***elem; // matrix of pointers "typeof(elem[y][x]) = Element *"
  Size **size;
  bool *col_fill;
  bool *row_fill;

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

  void setElementAt(int x, int y, Element *e) {
    elem[y][x] = e;
    if (e->getFlags() & Bix::FillX) col_fill[x] = true;
    if (e->getFlags() & Bix::FillY) row_fill[y] = true;
  }
};

//////////////////////////////////////////////////////////////////////
// Bix

Bix::Bix(Widget *container, int flags, int matrixColumns)
{
  if (container != NULL)
    container->setLayout(this);
  
  mFlags = flags;
  mCols = matrixColumns;
  mBorder = container != NULL ? 4: 0;
  mChildSpacing = 4;
}

bool Bix::isRow()
{
  return (mFlags & Bix::TypeMask) == Bix::Row;
}

bool Bix::isCol()
{
  return (mFlags & Bix::TypeMask) == Bix::Col;
}

bool Bix::isMat()
{
  return (mFlags & Bix::TypeMask) == Bix::Mat;
}

bool Bix::isEvenX()
{
  return (mFlags & Bix::EvenX) == Bix::EvenX;
}

bool Bix::isEvenY()
{
  return (mFlags & Bix::EvenY) == Bix::EvenY;
}

bool Bix::isEven()
{
  return (mFlags & Bix::Even) == Bix::Even;
}

bool Bix::isFillX()
{
  return (mFlags & Bix::FillX) == Bix::FillX;
}

bool Bix::isFillY()
{
  return (mFlags & Bix::FillY) == Bix::FillY;
}

bool Bix::isFill()
{
  return (mFlags & Bix::Fill) == Bix::Fill;
}

int Bix::getBorder()
{
  return mBorder;
}

void Bix::setBorder(int border)
{
  mBorder = border;
}

int Bix::getChildSpacing()
{
  return mChildSpacing;
}

void Bix::setChildSpacing(int childSpacing)
{
  mChildSpacing = childSpacing;
}

Bix *Bix::addBix(int flags, int matrixColumns)
{
  Bix *subbix = new Bix(NULL, flags, matrixColumns);
  elements.push_back(new SubBixElement(subbix));
  return subbix;
}

void Bix::add(Widget *child, int flags)
{
  elements.push_back(new WidgetElement(child, flags));
}

Size Bix::getPreferredSize(Widget *parent, Widget::Container &widgets, const Size &fitIn)
{
  return getPreferredSize(fitIn);
}

void Bix::layout(Widget *parent, Widget::Container &widgets, const Rect &rc)
{
  layout(rc);
}

Size Bix::getPreferredSize(const Size &fitIn)
{
  Size matDim = getMatrixDimension();
  if (matDim.w > 0 && matDim.h > 0) {
    Matrix mat(matDim.w, matDim.h);
    
    fillMatrix(mat);
    calcCellsSize(mat, fitIn);

    return getPreferredSize(mat);
  }
  else
    return Size(0, 0);
}

Size Bix::getPreferredSize(Matrix &mat)
{
  Size sz;

  for (int x=0; x<mat.cols; ++x) sz.w += mat.size[0][x].w;
  for (int y=0; y<mat.rows; ++y) sz.h += mat.size[y][0].h;

  sz.w += mBorder*2 + mChildSpacing*(mat.cols-1);
  sz.h += mBorder*2 + mChildSpacing*(mat.rows-1);

  return sz;
}

void Bix::layout(const Rect &rc)
{
  Size matDim = getMatrixDimension();
  if (matDim.w > 0 && matDim.h > 0) {
    Matrix mat(matDim.w, matDim.h);

    fillMatrix(mat);
    calcCellsSize(mat, Size(0, 0));

    // distribute size
    register int x, y;

    // X axis
    if (isEvenX()) {
      int remainderWidth = rc.w - mBorder*2 - mChildSpacing*(mat.cols-1);
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
      int colFills = getColFillsCount(mat);
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
      int remainderHeight = rc.h - mBorder*2 - mChildSpacing*(mat.rows-1);
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
      int rowFills = getRowFillsCount(mat);
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
    Point pt(0, rc.y+mBorder);

    for (y=0; y<mat.rows; ++y) {
      pt.x = rc.x+mBorder;
      for (x=0; x<mat.cols; ++x) {
	if (mat.elem[y][x] != NULL)
	  mat.elem[y][x]->setBounds(Rect(pt, Size(mat.size[0][x].w,
						  mat.size[y][0].h)));

	pt.x += mat.size[0][x].w+mChildSpacing;
      }
      pt.y += mat.size[y][0].h+mChildSpacing;
    }
  }
}

Size Bix::getMatrixDimension()
{
  int cols = 0, rows = 0;

  switch (mFlags & TypeMask) {

    // a row
    case Bix::Row: {
      cols = 0;
      rows = 1;

      for (Elements::iterator it=elements.begin(); it!=elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  cols++;
      }
      break;
    }

    // a column
    case Bix::Col: {
      cols = 1;
      rows = 0;

      for (Elements::iterator it=elements.begin(); it!=elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  rows++;
      }
      break;
    }

    // a matrix
    case Bix::Mat: {
      cols = mCols;
      rows = 0;

      int x = 0;
      bool freeRow = true;

      for (Elements::iterator it=elements.begin(); it!=elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  freeRow = false;

	if (++x == cols) {
	  x = 0;
	  if (!freeRow) {
	    ++rows;
	    freeRow = true;
	  }
	}
      }
      break;
    }
  }

  return Size(cols, rows);
}

void Bix::fillMatrix(Matrix &mat)
{
  switch (mFlags & TypeMask) {

    // a row
    case Bix::Row: {
      int x = 0;
      
      for (Elements::iterator it=elements.begin(); it!=elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  mat.setElementAt(x++, 0, *it);
      }

      mat.row_fill[0] = true;
      break;
    }

    // a column
    case Bix::Col: {
      int y = 0;

      for (Elements::iterator it=elements.begin(); it!=elements.end(); ++it) {
	if (!(*it)->isLayoutFree())
	  mat.setElementAt(0, y++, *it);
      }

      mat.col_fill[0] = true;
      break;
    }

    // a matrix
    case Bix::Mat: {
      int x = 0, y = 0;
      bool freeRow = true;

      for (Elements::iterator it=elements.begin(); it!=elements.end(); ++it) {
	if (!(*it)->isLayoutFree()) {
	  mat.setElementAt(x, y, *it);
	  freeRow = false;
	}

	if (++x == mat.cols) {
	  x = 0;
	  if (!freeRow) {
	    ++y;
	    freeRow = true;
	  }
	}
      }
      break;
    }
  }
}

void Bix::calcCellsSize(Matrix &mat, const Size &fitIn)
{
  register int x, y;
  
  // get the preferred size of each element in the matrix
  for (y=0; y<mat.rows; ++y)
    for (x=0; x<mat.cols; ++x)
      if (mat.elem[y][x] != NULL)
	mat.size[y][x] = mat.elem[y][x]->getPreferredSize(fitIn);

  // fill the first row with the maximum width of each column
  for (x=0; x<mat.cols; ++x)
    for (y=1; y<mat.rows; ++y)
      if (mat.size[0][x].w < mat.size[y][x].w)
	mat.size[0][x].w = mat.size[y][x].w;

  // fill the first column with the maximum height of each row
  for (y=0; y<mat.rows; ++y)
    for (x=1; x<mat.cols; ++x)
      if (mat.size[y][0].h < mat.size[y][x].h)
	mat.size[y][0].h = mat.size[y][x].h;
}

int Bix::getColFillsCount(Matrix &mat)
{
  register int x, count = 0;

  for (x=0; x<mat.cols; ++x)
    if (mat.col_fill[x])
      ++count;

  return count;
}

int Bix::getRowFillsCount(Matrix &mat)
{
  register int y, count = 0;

  for (y=0; y<mat.rows; ++y)
    if (mat.row_fill[y])
      ++count;

  return count;
}

#endif

namespace Bix {

// enum Modifiers {
//   None  = 0,
//   FillX = 1,
//   FillY = 2,
//   EvenX = 4,
//   EvenY = 8,
//   Fill  = FillX | FillY,
//   Even  = EvenX | EvenY,
// };

// struct BixElement;

// BixElement &fillX(Widget &widget);

// class BixLayout
// {
// private:
//   Modifiers mModifiers;

// public:
//   BixLayout();
//   virtual ~BixLayout();

//   BixLayout &operator<<(BixElement &elem);
//   BixLayout &operator<<(Widget &widget);
//   BixLayout &operator<<(BixElement &elem);
// };

// class Column : public BixLayout
// {
// public:
//   virtual ~Column();
// };

// class Matrix : public BixLayout
// {
// public:
//   Matrix(int columns);
//   virtual ~Matrix();
// };

// class Row : public BixLayout
// {
// public:
//   Row();
//   virtual ~Row();
// };

}

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Label mFirstNameLabel;
  Edit mFirstNameEdit;
  Label mLastNameLabel;
  Edit mLastNameEdit;
//   Separator mSeparator;
  Button mOK;
  Button mCancel;
  
public:

  MainFrame()
    : Frame("Bixes")
    , mFirstNameLabel("First Name:", this)
    , mFirstNameEdit("", this)
    , mLastNameLabel("Last Name:", this)
    , mLastNameEdit("", this)
//     , mSeparator(this)
    , mOK("OK", this)
    , mCancel("Cancel", this)
  {
/*
    // prepare layout
    Bix *col = new Bix(this, Bix::Col);
    Bix *mat = col->addBix(Bix::Mat, 2);

    mat->add(&mFirstNameLabel);
    mat->add(&mFirstNameEdit, Bix::FillX);

    mat->add(&mLastNameLabel);
    mat->add(&mLastNameEdit, Bix::FillX);

//     bix->add(&mSeparator);

    Bix *row = col->addBix(Bix::Row);
    row->addBix(Bix::FillX);
    row->add(&mOK);
    row->add(&mCancel);

*/
    // --------------------

        // prepare layout
//     Bix col(Bix::Col);
//     Bix mat(Bix::Mat, 2);
//     Bix row(Bix::Row);

//     col << mat << row;
//     mat << &mFirstNameLabel << Bix::FillX << &mFirstNameEdit;
//     mat << &mLastNameLabel << Bix::FillX << &mLastNameEdit;
//     row << Bix(Bix::FillX) << &mOK << &mCancel;

//     setLayout(col.createLayout());

    // --------------------

    
    // prepare layout

    {
      using namespace Bix;

      Column column(Set
		    << Matrix(2,
			      Set
			      << mFirstNameLabel << fillX(mFirstNameEdit)
			      << mLastNameLabel << fillX(mLastNameEdit))
		    << Row(Set
			   << Filler << Row(Set << evenX(mOK) << evenX(mCancel))));

      setLayout(column.createLayout());
    }

    // --------------------

    setSize(getPreferredSize());
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
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
