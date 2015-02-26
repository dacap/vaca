// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SHAREDPTR_H
#define VACA_SHAREDPTR_H

#include "vaca/base.h"
#include "vaca/Referenceable.h"

namespace vaca {

/**
   A pointer which maintains reference counting and
   automatically deletes the pointed object when it is
   no longer referenced.

   What is a shared pointer? It is a class that wraps a pointer to a
   dynamically allocated object. Various shared pointers can share
   the same object counting references for that object. When the
   last shared pointer is destroyed, the object is automatically
   deleted.

   The SharedPtr is mainly used to wrap classes that handle
   graphics resources (like Brush, Pen, Image, Icon, etc.).

   @tparam T Must be of Referenceable type, because Referenceable has
	     the reference counter.
*/
template<class T>
class SharedPtr
{
  T* m_ptr;

public:

  SharedPtr() {
    m_ptr = NULL;
  }

  /*explicit*/ SharedPtr(T* ptr) {
    m_ptr = ptr;
    ref();
  }

  SharedPtr(const SharedPtr<T>& other) {
    m_ptr = other.get();
    ref();
  }

  template<class T2>
  SharedPtr(const SharedPtr<T2>& other) {
    m_ptr = static_cast<T*>(other.get());
    ref();
  }

  virtual ~SharedPtr() {
    unref();
  }

  void reset(T* ptr = NULL) {
    if (m_ptr != ptr) {
      unref();
      m_ptr = ptr;
      ref();
    }
  }

  SharedPtr& operator=(const SharedPtr<T>& other) {
    if (m_ptr != other.get()) {
      unref();
      m_ptr = other.get();
      ref();
    }
    return *this;
  }

  template<class T2>
  SharedPtr& operator=(const SharedPtr<T2>& other) {
    if (m_ptr != static_cast<T*>(other.get())) {
      unref();
      m_ptr = static_cast<T*>(other.get());
      ref();
    }
    return *this;
  }

  inline T* get() const { return m_ptr; }
  inline T& operator*() const { return *m_ptr; }
  inline T* operator->() const { return m_ptr; }
  inline operator T*() const { return m_ptr; }

private:

  void ref() {
    if (m_ptr)
      ((Referenceable*)m_ptr)->ref();
  }

  void unref() {
    if (m_ptr) {
      if (((Referenceable*)m_ptr)->unref() == 0)
	((Referenceable*)m_ptr)->destroy();
      m_ptr = NULL;
    }
  }
};

/**
   Compares if two shared-pointers points to the same place (object, memory address).

   @see @ref SharedPtr
*/
template<class T>
bool operator==(const SharedPtr<T>& ptr1, const SharedPtr<T>& ptr2)
{
  return ptr1.get() == ptr2.get();
}

/**
   Compares if two shared-pointers points to different places (objects, memory addresses).

   @see @ref SharedPtr
*/
template<class T>
bool operator!=(const SharedPtr<T>& ptr1, const SharedPtr<T>& ptr2)
{
  return ptr1.get() != ptr2.get();
}

} // namespace vaca

#endif // VACA_SHAREDPTR_H
