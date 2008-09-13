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

#include "Vaca/Referenceable.h"
#include "Vaca/Debug.h"

#ifndef NDEBUG
#include "Vaca/ScopedLock.h"
#include <typeinfo>
#endif

using namespace Vaca;

#ifndef NDEBUG
Mutex Referenceable::mutex;
volatile int Referenceable::instanceCounter = 0;
std::vector<Referenceable*> Referenceable::list;
#endif

Referenceable::Referenceable()
{
  m_refCount = 0;
#ifndef NDEBUG
  {
    ScopedLock hold(mutex);
    VACA_TRACE("new Referenceable (%d, %p)\n", ++instanceCounter, this);
    list.push_back(this);
  }
#endif
}

Referenceable::~Referenceable()
{
#ifndef NDEBUG
  {
    ScopedLock hold(mutex);
    VACA_TRACE("delete Referenceable (%d, %p)\n", --instanceCounter, this);
    remove_from_container(list, this);
  }
#endif
  assert(m_refCount == 0);
}

void Referenceable::ref()
{
  ++m_refCount;
}

unsigned Referenceable::unref()
{
  assert(m_refCount > 0);
  return --m_refCount;
}

unsigned Referenceable::getRefCount()
{
  return m_refCount;
}

#ifndef NDEBUG
void Referenceable::showLeaks()
{
  for (std::vector<Referenceable*>::iterator
	 it=list.begin(); it!=list.end(); ++it) {
    VACA_TRACE("leak Referenceable %p\n", *it);
  }
}
#endif
