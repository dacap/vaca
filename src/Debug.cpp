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

#include "Vaca/Debug.h"
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"
#include "Vaca/System.h"
#include "Vaca/Thread.h"

using namespace Vaca;

#ifndef NDEBUG
static Mutex log_mutex;
static FILE* log_file = NULL;
static bool log_closed = false;
#endif

void Vaca::__vaca_trace(LPCSTR filename, UINT line, LPCSTR fmt, ...)
{
#ifndef NDEBUG
  ScopedLock hold(log_mutex);
  char buf[1024];		// TODO: overflow
  va_list ap;

  if (log_closed)
    return;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);

  if (log_file == NULL) {
    log_file = fopen("vaca.log", "w");
    fprintf(log_file, "Log file created\n");
  }

  if (log_file != NULL) {
    fprintf(log_file, "%s:%d: [%d] %s", filename, line, ::GetCurrentThreadId(), buf);
    fflush(log_file);
  }
#endif
}

void Vaca::__vaca_close_log_file()
{
#ifndef NDEBUG
  ScopedLock hold(log_mutex);

  if (log_file != NULL) {
    fprintf(log_file, "Log file closed\n");
    fclose(log_file);
    log_file = NULL;

    log_closed = TRUE;
  }
#endif
}
