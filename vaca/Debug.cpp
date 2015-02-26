// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Debug.h"
#include "vaca/Mutex.h"
#include "vaca/ScopedLock.h"
#include "vaca/System.h"
#include "vaca/Thread.h"

#include <cstdio>

using namespace std;
using namespace vaca;

#ifndef NDEBUG
struct Debug;

static bool closed;
static Debug* dbg;

struct Debug {
  Mutex mutex;
  FILE* file;
  Debug() {
    file = fopen("vaca.log", "w");
    fprintf(file, "Log file created\n");
  }
  virtual ~Debug() {
    fprintf(file, "Log file closed\n");
    fclose(file);
    file = NULL;
    closed = true;
  }
};
#endif

void vaca::details::trace(const char* filename, size_t line, const char* fmt, ...)
{
#ifndef NDEBUG
  if (closed) { return; }
  if (!dbg) { dbg = new Debug; }

  ScopedLock hold(dbg->mutex);
  char buf[1024];		// TODO: overflow
  va_list ap;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);

  fprintf(dbg->file, "%s:%d: [%d] %s", filename, line,
	  static_cast<unsigned>(::GetCurrentThreadId()), buf);
  fflush(dbg->file);
#endif
}

void vaca::details::closeLogFile()
{
#ifndef NDEBUG
  delete dbg;
  dbg = NULL;
#endif
}
