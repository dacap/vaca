// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_DEBUG_H
#define VACA_DEBUG_H

#include "vaca/base.h"

#include <cassert>

namespace vaca {

#ifdef NDEBUG
#  ifdef __GNUC__
#    define VACA_TRACE(msg...)
#  else
#    define VACA_TRACE(...)
#  endif
#else
#  ifdef __GNUC__
#    define VACA_TRACE(msg...) vaca::details::trace(__FILE__, __LINE__, msg)
#  else
#    define VACA_TRACE         vaca::details::make_trace(__FILE__, __LINE__)
#  endif
#endif

namespace details {

void VACA_DLL trace(const char* filename, size_t line, const char* msg, ...);
void VACA_DLL closeLogFile();

#ifndef __GNUC__
/**
   @internal
      Dirty trick for compilers that does not support
      macros with ellipsis (...).
*/
struct trace_t {
  const char* filename;
  size_t line;
  void operator()(const char* fmt, ...) {
    char buf[1024];		// TODO: overflow
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    vaca::details::trace(filename, line, buf);
  }
};
inline trace_t make_trace(const char* filename, size_t line) {
  trace_t tmp;
  tmp.filename = filename;
  tmp.line = line;
  return tmp;
}
#endif

} // namespace details

} // namespace vaca

#endif // VACA_DEBUG_H
