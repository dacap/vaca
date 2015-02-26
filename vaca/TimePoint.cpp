// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/TimePoint.h"

using namespace vaca;

/**
   Creates a new TimePoint starting the chronometer from this point.

   @see #elapsed
*/
TimePoint::TimePoint()
{
  QueryPerformanceFrequency(&m_freq);
  reset();
}

/**
   Destroys the TimePoint.
*/
TimePoint::~TimePoint()
{
}

/**
   Resets the chronometer.

   Next calls to #elapsed will return the time elapsed from the
   last call of #reset.

   @see #elapsed
*/
void TimePoint::reset()
{
  QueryPerformanceCounter(&m_point);
}

/**
   Returns the life-time in seconds of this object.

   The life-time is the elapsed time from the construction of the
   object (or from the last call to #reset member function).

   @see reset
*/
double TimePoint::elapsed() const
{
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return static_cast<double>(now.QuadPart - m_point.QuadPart)
    / static_cast<double>(m_freq.QuadPart);
}
