// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#ifndef VACA_ICON_HPP
#define VACA_ICON_HPP

#include "Vaca/base.hpp"
#include "Vaca/NonCopyable.hpp"
#include "Vaca/Size.hpp"

namespace Vaca {

class String;

/**
 * An icon (HICON wrapper).
 */
class VACA_DLL Icon : private NonCopyable
{
  HICON m_HICON;

protected:

  Icon();

public:

  Icon(int iconId, const Size& sz = Size(0, 0));
  Icon(const String& fileName, const Size& sz = Size(0, 0));
  virtual ~Icon();

  HICON getHICON();

protected:

  void setHICON(HICON hicon);

};

/**
 * An icon that is shared (isn't destroyed).
 */
class VACA_DLL SharedIcon : public Icon
{
public:

  SharedIcon(int iconId, const Size& sz = Size(0, 0));
  SharedIcon(const String& fileName, const Size& sz = Size(0, 0));
  virtual ~SharedIcon();

};

} // namespace Vaca

#endif
