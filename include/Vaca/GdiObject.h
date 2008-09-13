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

#ifndef VACA_GDIOBJECT_H
#define VACA_GDIOBJECT_H

#include "Vaca/base.h"
#include "Vaca/Referenceable.h"

namespace Vaca {

/**
 * Helper for GdiObject class to destroy Win32's HGDIOBJ handles.
 * 
 * @internal
 */
struct Win32DestroyGdiObject
{
  static void destroy(HGDIOBJ handle)
  {
    ::DeleteObject(handle);
  }
};

/**
 * This class is a wrapper for Win32's GDI objects.
 *
 * @internal
 */
template<typename T, class Destroyer = Win32DestroyGdiObject>
class GdiObject : public Referenceable
{
  T m_handle;

public:
  GdiObject() : m_handle(NULL) { }
  GdiObject(T handle) : m_handle(handle) { }
  virtual ~GdiObject() {
    if (isValid())
      Destroyer::destroy(m_handle);
  }

  bool isValid() const { return m_handle != NULL; }
  T getHandle() const { return m_handle; }
  void setHandle(T handle) { m_handle = handle; }

};

} // namespace Vaca

#endif // VACA_GDIOBJECT_H
