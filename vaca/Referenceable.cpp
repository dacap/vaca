// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Referenceable.h"
#include "vaca/Debug.h"

#ifndef NDEBUG
#include "vaca/Mutex.h"
#include "vaca/ScopedLock.h"
#include <vector>
#include <typeinfo>
  #ifdef VACA_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
  #endif
#endif

using namespace vaca;

#ifndef NDEBUG
static Mutex s_mutex;
static std::vector<Referenceable*> s_list;
#endif

/**
   Constructs a new referenceable object starting with zero references.
*/
Referenceable::Referenceable()
{
  m_refCount = 0;
#ifndef NDEBUG
  {
    ScopedLock hold(s_mutex);
    VACA_TRACE("new Referenceable (%d, %p)\n", s_list.size()+1, this);
    s_list.push_back(this);
  }
#endif
}

/**
   Destroys a referenceable object.

   When compiling with assertions it checks that the references'
   counter is really zero.
*/
Referenceable::~Referenceable()
{
#ifndef NDEBUG
  {
    ScopedLock hold(s_mutex);
    VACA_TRACE("delete Referenceable (%d, %p)\n", s_list.size()-1, this);
    remove_from_container(s_list, this);
  }
#endif
  assert(m_refCount == 0);
}

/**
   Called by SharedPtr to destroy the referenceable.
*/
void Referenceable::destroy()
{
  delete this;
}

/**
   Makes a new reference to this object.

   You are responsible for removing references using the #unref
   member function. Remember that for each call to #ref that you made,
   there should be a corresponding #unref.

   @see unref
*/
void Referenceable::ref()
{
  ++m_refCount;
}

/**
   Deletes an old reference to this object.

   If assertions are activated this routine checks that the
   reference counter never get negative, because that implies
   an error of the programmer.

   @see ref
*/
unsigned Referenceable::unref()
{
  assert(m_refCount > 0);
  return --m_refCount;
}

/**
   Returns the current number of references that this object has.

   If it's zero you can delete the object safely.
*/
unsigned Referenceable::getRefCount()
{
  return m_refCount;
}

#ifndef NDEBUG
void Referenceable::showLeaks()
{
#ifdef VACA_WINDOWS
  if (!s_list.empty())
    ::Beep(400, 100);
#endif

  for (std::vector<Referenceable*>::iterator
	 it=s_list.begin(); it!=s_list.end(); ++it) {
    VACA_TRACE("leak Referenceable %p\n", *it);
  }
}
#endif
