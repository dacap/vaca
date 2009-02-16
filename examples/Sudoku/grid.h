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

#ifndef GRID_H
#define GRID_H

#include <iterator>

//////////////////////////////////////////////////////////////////////

template<class T, class MovementPolicy>
class base_grid_iterator : public std::iterator<std::bidirectional_iterator_tag, T>
{
  template<class, class> friend class base_grid_iterator;

  T* m_values;
  int m_col, m_row;
  MovementPolicy m_movement;

public:
  // template<class, class> friend class base_grid_iterator;
    
  base_grid_iterator()
    : m_values(0)
    , m_col(-1)
    , m_row(-1)
    , m_movement(-1, -1)
  { }

  explicit base_grid_iterator(T *p, int col, int row)
    : m_values(p)
    , m_col(col)
    , m_row(row)
    , m_movement(col, row)
  { }

  template<class T2, class OtherMovementPolicy>
  base_grid_iterator(const base_grid_iterator<T2, OtherMovementPolicy> &other)
    : m_values(other.m_values)
    , m_col(other.m_col)
    , m_row(other.m_row)
    , m_movement(other.m_col, other.m_row)
  { }

  int col() const { return m_col; }
  int row() const { return m_row; }
  int box() const { return (m_row/3)*3 + m_col/3; }

  T& operator*() const { return dereference(); }
  T* operator->() const { return &dereference(); }
  base_grid_iterator& operator++() {
    increment();
    return *this;
  }
  base_grid_iterator operator++(int) {
    base_grid_iterator i = *this;
    increment();
    return i;
  }
  base_grid_iterator& operator--() {
    decrement();
    return *this;
  }
  base_grid_iterator operator--(int) {
    base_grid_iterator i = *this;
    decrement();
    return i;
  }
  bool operator==(const base_grid_iterator& i) const { return equal(i); }
  bool operator!=(const base_grid_iterator& i) const { return !equal(i); }

private:
  void increment() { m_movement.increment(m_col, m_row); }
  void decrement() { m_movement.decrement(m_col, m_row); }
  T& dereference() const { return m_values[m_row*9 + m_col]; }
  bool equal(base_grid_iterator const& other) const {
    return (this->m_col == other.m_col &&
	    this->m_row == other.m_row);
  }
};

//////////////////////////////////////////////////////////////////////

class scan_movement
{
public:
  scan_movement(int col, int row) { }
  void increment(int &col, int &row)
  { col++;
    if (col == 9)
      row++, col = 0; }
  void decrement(int &col, int &row)
  { col--;
    if (col == -1)
      row--, col = 8; }
};

//////////////////////////////////////////////////////////////////////

class box_movement
{
  int m_start_col;
public:
  box_movement(int col, int row) : m_start_col((col/3)*3) { }
  void increment(int &col, int &row)
  { col++;
    if (col == m_start_col+3)
      row++, col = m_start_col; }
  void decrement(int &col, int &row)
  { col--;
    if (col == m_start_col-1)
      row--, col = m_start_col+2; }
};

//////////////////////////////////////////////////////////////////////

class col_movement
{
public:
  col_movement(int col, int row) { }
  void increment(int &col, int &row) { row++; }
  void decrement(int &col, int &row) { row--; }
};

//////////////////////////////////////////////////////////////////////

class row_movement
{
public:
  row_movement(int col, int row) { }
  void increment(int &col, int &row) { col++; }
  void decrement(int &col, int &row) { col--; }
};

//////////////////////////////////////////////////////////////////////

template<class T>
class grid
{
  T values[9*9];
  
public:
  typedef base_grid_iterator<T, scan_movement> iterator;
  typedef base_grid_iterator<T, box_movement> box_iterator;
  typedef base_grid_iterator<T, col_movement> col_iterator;
  typedef base_grid_iterator<T, row_movement> row_iterator;

  grid() { }

  iterator begin() { return at(0, 0); }
  iterator end() { return at(0, 9); }

  box_iterator box_begin(int box) { return at(3*(box%3), 3*(box/3)); }
  box_iterator box_end(int box) { return at(3*(box%3), 3*(box/3)+3); }

  col_iterator col_begin(int col) { return at(col, 0); }
  col_iterator col_end(int col) { return at(col, 9); }

  row_iterator row_begin(int row) { return at(0, row); }
  row_iterator row_end(int row) { return at(9, row); }

  iterator at(int col, int row)
  {
    return iterator(values, col, row);
  }

  void reset(const T &default_value)
  {
    std::fill(begin(), end(), default_value);
  }

  grid<T> &operator=(grid<T> &copy)
  {
    std::copy(copy.begin(), copy.end(), this->begin());
    return *this;
  }
  
};

#endif // GRID_H
