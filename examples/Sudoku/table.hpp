// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#ifndef TABLE_H
#define TABLE_H

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Special structure to handle tables of 9x9

template<typename T>
class table
{
  T mValues[9*9];
  T *mStart;
  T *mFinish;

public:
  class iterator {
    T *mPtr;
  public:
    iterator(T *start) {
      mPtr = start;
    }
    iterator(const iterator &it) {
      mPtr = it.mPtr;
    }
    iterator & operator=(const iterator &it) {
      mPtr = it.mPtr;
      return *this;
    }
    iterator & operator++() {
      mPtr++;
      return *this;
    }
    bool operator==(const iterator &other) { return (mPtr == other.mPtr); }
    bool operator!=(const iterator &other) { return (mPtr != other.mPtr); }
    T& operator*()  { return *mPtr; }
    T *operator->() { return &(operator*()); }
  };

public:
  table() { }

  iterator begin() {
    return iterator(&mValues[0]);
  }

  iterator end() {
    return iterator(&mValues[9*9]);
  }

  void reset(const T &defValue) {
    for (iterator it=begin(); it!=end(); ++it)
      *it = defValue;
  }

  T &get(int x, int y) {
    assert(x >= 0 && x < 9 && y >= 0 && y < 9);
    return mValues[y*9+x];
  }

  void put(int x, int y, const T &value) {
    assert(x >= 0 && x < 9 && y >= 0 && y < 9);
    mValues[y*9+x] = value;
  }

  // table[row][column]
  T* operator[](const int &index) {
    assert(index >= 0 && index < 9);
    return &mValues[index*9];
  }

  // table[point]
  T &operator[](const Point &index) {
    return get(index.x, index.y);
  }

  // table = table_copy
  table<T> &operator=(const table<T> &copy) {
    for (iterator it1=begin(), it2=copy.begin();
	 it1!=end() && it2!=end();
	 ++it1, ++it2)
      *it1 = *it2;
    return *this;
  }
  
};

#endif
