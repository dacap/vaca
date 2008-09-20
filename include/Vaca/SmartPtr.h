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

#ifndef VACA_SMARTPTR_H
#define VACA_SMARTPTR_H

#include "Vaca/base.h"

namespace Vaca {

/**
 * A pointer which maintains reference counting and
 * automatically deletes the pointed object when it is
 * no longer referenced.
 *
 * What is a smart pointer? It is a class that wraps a pointer to a
 * dynamically allocated object. Various smart pointers can share
 * the same object counting references for that object. When the
 * last smart pointer is destroyed, the object is automatically
 * deleted.
 *
 * It's used internally by Vaca to wrap classes that can't be
 * derivable and handle graphics resources (like Brush, Pen, Image,
 * Icon, etc.).
 *
 * @tparam T Must be of Referenceable type, because Referenceable has
 *           the reference counter.
 */
template<class T>
class SmartPtr
{
  T* m_ptr;
public:
  SmartPtr() {
    m_ptr = NULL;
  }
  explicit SmartPtr(T* ptr) {
    m_ptr = ptr;
    ref();
  }
  SmartPtr(const SmartPtr& other) {
    m_ptr = other.m_ptr;
    ref();
  }
  virtual ~SmartPtr() {
    unref();
  }
  SmartPtr& operator=(const SmartPtr& other) {
    if (m_ptr != other.m_ptr) {
      unref();
      m_ptr = other.m_ptr;
      ref();
    }
    return *this;
  }

  T* get() const { return m_ptr; }
  T& operator*() const { return *m_ptr; }
  T* operator->() const { return m_ptr; }

private:
  void ref() {
    if (m_ptr)
      m_ptr->ref();
  }
  void unref() {
    if (m_ptr) {
      if (m_ptr->unref() == 0)
	delete m_ptr;
      m_ptr = NULL;
    }
  }
};

/**
 * Compares if two smart-pointers points to the same place (object, memory address).
 * 
 * @see @ref SmartPtr
 */
template<class T>
bool operator==(const SmartPtr<T>& ptr1, const SmartPtr<T>& ptr2)
{
  return ptr1.get() == ptr2.get();
}

/**
 * Compares if two smart-pointers points to different places (objects, memory addresses).
 *
 * @see @ref SmartPtr
 */
template<class T>
bool operator!=(const SmartPtr<T>& ptr1, const SmartPtr<T>& ptr2)
{
  return ptr1.get() != ptr2.get();
}

} // namespace Vaca

#endif // VACA_SMARTPTR_H
