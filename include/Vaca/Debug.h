// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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

#ifndef VACA_DEBUG_H
#define VACA_DEBUG_H

#include <assert.h>

#include "Vaca/base.h"

namespace Vaca {

#ifdef NDEBUG
#  ifdef __GNUC__
#    define VACA_TRACE(msg...)
#  else
#    define VACA_TRACE(...)
#  endif
#else
#  ifdef __GNUC__
#    define VACA_TRACE(msg...) Vaca::details::trace(__FILE__, __LINE__, msg)
#  else
#    define VACA_TRACE         Vaca::details::make_trace(__FILE__, __LINE__)
#  endif
#endif

namespace details {

void VACA_DLL trace(LPCSTR filename, UINT line, LPCSTR msg, ...);
void VACA_DLL close_log_file();

#ifndef __GNUC__
/// @internal
///    Dirty trick for compilers that does not support
///    macros with ellipsis (...).
/// 
struct trace_t {
  LPCSTR filename;
  UINT line;
  void operator()(LPCSTR fmt, ...) {
    char buf[1024];		// TODO: overflow
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    Vaca::details::trace(filename, line, buf);
  }
};
inline trace_t make_trace(LPCSTR filename, UINT line) {
  trace_t tmp;
  tmp.filename = filename;
  tmp.line = line;
  return tmp;
}
#endif

} // namespace details

} // namespace Vaca

#endif // VACA_DEBUG_H
