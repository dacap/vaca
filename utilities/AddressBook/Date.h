// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

#ifndef ADDRESSBOOK_DATE_H
#define ADDRESSBOOK_DATE_H

#include <Vaca/String.h>

using namespace Vaca;

const String monthNames[] = {
  "January", "February", "March", "April",
  "May", "June", "July", "August",
  "September", "October", "November", "December"
};

const String weekdayNames[] = {
  "Sunday", "Monday", "Tuesday",
  "Wednesday", "Thursday", "Friday", "Saturday"
};

class Date
{
  std::time_t t;
  struct std::tm tm;

public:
  // current date
  Date() {
    t = std::time(NULL);
    tm = *std::localtime(&t);
  }
  // copy a date
  Date(const Date &date) {
    t = date.t;
    tm = date.tm;
  }
  // date from ISO string
  Date(String str) {
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;

    tm.tm_year = str.parseInt()-1900;
    str.erase(0, 5);
    
    tm.tm_mon = str.parseInt()-1;
    str.erase(0, 3);

    tm.tm_mday = str.parseInt();
    tm.tm_isdst = 0;
    t = mktime(&tm);
  }
  
  int getDay() const { return tm.tm_mday; }	  // 1-31
  int getMonth() const { return tm.tm_mon+1; }	  // 1-12
  int getYear() const { return tm.tm_year+1900; } // 1900-2038
  int getDayOfWeek() const { return tm.tm_wday; }   // 0-6

  String getMonthName() const { return monthNames[tm.tm_mon]; }

  void setDay(int day) { tm.tm_mday = day; t=std::mktime(&tm); } // 1-31

  void prevDay() { tm.tm_mday++; t=std::mktime(&tm); }
  void nextDay() { tm.tm_mday++; t=std::mktime(&tm); }

  void prevMonth() { tm.tm_mon--; t=std::mktime(&tm); }
  void nextMonth() { tm.tm_mon++; t=std::mktime(&tm); }

  bool operator==(const Date &date) const
  {
    return
      getYear() == date.getYear() &&
      getMonth() == date.getMonth() &&
      getDay() == date.getDay();
  }

  bool operator<(const Date &date) const
  {
    return
      (getYear() < date.getYear()) ||
      (getYear() == date.getYear() &&
       getMonth() < date.getMonth()) ||
      (getYear() == date.getYear() &&
       getMonth() == date.getMonth() &&
       getDay() < date.getDay());
  }

  String getISO() const
  {
    return
      String::fromInt(getYear(),  10, 4) + "-" +
      String::fromInt(getMonth(), 10, 2) + "-" +
      String::fromInt(getDay(),   10, 2);
  }

};

#endif
