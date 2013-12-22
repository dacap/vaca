// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_TIMEPOINT_H
#define VACA_TIMEPOINT_H

#include "Vaca/base.h"

namespace Vaca {

/**
   Class to measure elapsed time, like a chronometer.
*/
class VACA_DLL TimePoint
{
  LARGE_INTEGER m_point;
  LARGE_INTEGER m_freq;

public:
  TimePoint();
  ~TimePoint();

  void reset();

  double elapsed() const;
};

} // namespace Vaca

#endif // VACA_TIMEPOINT_H
