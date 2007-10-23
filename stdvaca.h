#ifndef STDVACA_H
#define STDVACA_H

// libc & STL
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <list>
#include <stack>
#include <string>
#include <vector>

#include <math.h>
#include <stdarg.h>
#include <tchar.h>

// Windows
#define ISOLATION_AWARE_ENABLED 1

#include <windows.h>
#include <commctrl.h>
#include <lmcons.h>
#include <pbt.h>
#include <security.h>

// M_PI is a problem in some compilers
#ifndef M_PI
#  define M_PI 3.14159265358979323846 
#endif

// Boost
#include <boost/signal.hpp>
#include <boost/noncopyable.hpp>

#endif
