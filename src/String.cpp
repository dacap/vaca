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

#include "stdvaca.h"
#include "Vaca/String.h"
#include "Vaca/Debug.h"
#include <cstdarg>
#include <cstdlib>

#include <algorithm>

using namespace Vaca;

String::String()
  : VACA_STRING_BASE(_T(""))
{
}

String::String(int length)
  : VACA_STRING_BASE(length, static_cast<Character>(0))
{
}

String::String(const String &str)
  : VACA_STRING_BASE(str)
{
}

String::String(const VACA_STRING_BASE &str)
  : VACA_STRING_BASE(str)
{
}

#ifdef _UNICODE
String::String(const char *str)
  : VACA_STRING_BASE()
{
  // TODO check if this really works
  for (const char *p = str; *p != 0; ++p)
    push_back(static_cast<wchar_t>(*p));
}

String::String(const char *str, int length)
  : VACA_STRING_BASE()
{
  // TODO check if this really works
  for (int c=0; c<length; ++c)
    push_back(static_cast<wchar_t>(str[c]));
}

String::String(const wchar_t *str)
  : VACA_STRING_BASE(str)
{
}
#else
String::String(const char *str)
  : VACA_STRING_BASE(str)
{
}

String::String(const char *str, int length)
  : VACA_STRING_BASE(str, length)
{
}

String::String(const wchar_t *str)
  : VACA_STRING_BASE()
{
  int len = wcslen(str)+1;
  char *buf = new char[len];
  int ret = WideCharToMultiByte(CP_ACP, 0, str, len, buf, len, NULL, NULL);

  if (ret != 0)
    assign(buf);
  else {
    // error!
  }

  delete buf;
}
#endif

String::~String()
{
}

void String::copyTo(LPTSTR dest, int size) const
{
  _tcsncpy(dest, c_str(), size);
}

String String::fromInt(int value, int base, int precision)
{
  String str;

  assert(base > 0);

  div_t d;
  d.quot = value;
  d.rem = 0;

  int i = 0;
  do {
    d = div(d.quot, base);
    str.push_back(d.rem < 10 ? '0'+d.rem:
			       'a'+(d.rem-10));
    i++;
  } while (d.quot != 0);

  for (; i<precision; i++)
    str.push_back('0');

  if (value < 0)
    str.push_back('-');

  std::reverse(str.begin(), str.end());

  return str;
}

int String::parseInt(int base)
{
  return _tcstol(c_str(), NULL, base);
}

double String::parseDouble()
{
  return _tcstod(c_str(), NULL);
}

/**
 * Returns the file path (the path of "C:\foo\main.cpp" is "C:\foo"
 * without the file name).
 *
 * @see getFileName
 */
String String::getFilePath() const
{
  const_iterator it, it2;
  String res;

  for (it=end()-1; it!=begin()-1; --it)
    if (*it == '\\' || *it == '/')
      break;

  if (it != begin()-1)
    for (it2=begin(); it2!=it; ++it2)
      res.push_back(*it2);

  return res;
}

/**
 * Returns the file name (the file name of "C:\foo\main.cpp" is
 * "main.cpp", without the path).
 *
 * @see getFilePath, getFileTitle
 */
String String::getFileName() const
{
  const_iterator it;
  String res;

  for (it=end()-1; it!=begin()-1; --it)
    if (*it == '\\' || *it == '/')
      break;

  for (++it; it!=end(); ++it)
    res.push_back(*it);

  return res;
}

/**
 * Returns the file extension (the extension of "C:\foo\main.cpp" is
 * "cpp", without the path and its title).
 *
 * @see getFilePath, getFileTitle
 */
String String::getFileExtension() const
{
  const_iterator it;
  String res;

  for (it=end()-1; it!=begin()-1; --it)
    if (*it == '\\' || *it == '/')
      return res;
    else if (*it == '.')
      break;

  if (it != begin()-1)
    for (++it; it!=end(); ++it)
      res.push_back(*it);

  return res;
}

/**
 * Returns the file title (the title of "C:\foo\main.cpp" is "main",
 * without the path and without the extension).
 *
 * @see getFilePath, getFileExtension
 */
String String::getFileTitle() const
{
  const_iterator it;
  String res;

  for (it=end()-1; it!=begin()-1; --it)
    if (*it == '\\' || *it == '/')
      break;

  for (++it; it!=end(); ++it)
    if (*it == '.')
      break;
    else
      res.push_back(*it);

  return res;
}

String String::addPathComponent(const String &component) const
{
#if 0
  TCHAR buf[MAX_PATH];
  copyTo(buf, MAX_PATH);
    
  if (PathAppend(buf, component.c_str()))
    return String(buf);
  else
    return *this;
#else  
  String res(*this);
  
  if (!empty() && (*end() != '/' || *end() != '\\'))
    res.push_back('\\');

  res += component;
  return res;
#endif
}

#if 0
String String::removeExtension() const
{
  TCHAR buf[MAX_PATH];
  copyTo(buf, MAX_PATH);

  PathRemoveExtension(buf);
  return String(buf);
}

String String::replaceExtension(const String &newExtension) const
{
  return removeExtension() + "." + newExtension;
//   TCHAR buf[MAX_PATH];
//   copyTo(buf, MAX_PATH);

//   if (PathAppend(buf, component.c_str()))
//     return String(buf);
//   else
//     return *this;
}
#endif