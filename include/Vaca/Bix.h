// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
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

#ifndef VACA_BIX_H
#define VACA_BIX_H

#include "Vaca/base.h"
#include "Vaca/Layout.h"

namespace Vaca {

enum {
  // flags: type
  BixTypeMask = 3,
  BixRow = 1,			// row vector (horizontal, x axis)
  BixCol = 2,			// column vector (vertical, y axis)
  BixMat = 3,			// matrix (both axis)
  // flags: modifiers
  BixEvenX = 4,
  BixEvenY = 8,
  BixEven = BixEvenX | BixEvenY,
  BixFillX = 16,
  BixFillY = 32,
  BixFill = BixFillX | BixFillY,
};

/**
   A Bix is a <em>layout manager</em> which allows you to arrange widgets
   in a matricial way or horizontally/vertically (like a row/column vector
   respectively).

   All matrices and vectors can be inner one inside the other.
*/
class VACA_DLL Bix : public Layout
{

  struct Element;
  struct SubBixElement;
  struct WidgetElement;
  struct Matrix;

  typedef std::vector<Element*> Elements;

  int m_flags;
  int m_cols;
  int m_border;
  int m_childSpacing;
  Elements m_elements;

public:

  Bix(int flags, int matrixColumns = 0);
  virtual ~Bix();

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

  int getMatrixColumns();
  void setMatrixColumns(int matrixColumns);

  Bix* add(int flags, int matrixColumns = 0);
  void add(Widget* child, int flags = 0);

  void remove(Bix* subbix);
  void remove(Widget* child);

  virtual Size getPreferredSize(Widget* parent, WidgetList& widgets, const Size& fitIn);

  static Bix* parse(const Char* fmt, ...);

protected:

  virtual void layout(Widget* parent, WidgetList& widgets, const Rect& rc);

private:

  Size getPreferredSize(const Size& fitIn);
  Size getPreferredSize(Matrix& mat);
  void layout(Bix* parentBix, const Rect& rc);

  Size getMatrixDimension();
  void fillMatrix(Matrix& mat);

};

} // namespace Vaca

#endif // VACA_BIX_H
