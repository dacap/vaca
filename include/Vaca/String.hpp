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

#ifndef VACA_STRING_HPP
#define VACA_STRING_HPP

#include <string>

#include "Vaca/base.hpp"

namespace Vaca {

#ifndef VACA_STRING_BASE
#  define VACA_STRING_BASE std::basic_string<Character>
#endif

/**
 * A character string used in Vaca namespace. It's like a
 * std::basic_string<Character> with some extra functionalities. This
 * class doesn't have extra data members.
 */
class VACA_DLL String : public VACA_STRING_BASE
{
public:

  //////////////////////////////////////////////////////////////////////
  // Ctors & Dtor

  String();
  explicit String(int length);
  String(const String& str);
  String(const VACA_STRING_BASE& str);
  String(const char* str);
  String(const char* str, int length);
  String(const wchar_t* str);
  virtual ~String();

  // String& format(LPCTSTR fmt, ...);

  //////////////////////////////////////////////////////////////////////
  // Conversion

  // Method to convert the string to the standard
  std::string to_string() const;
  std::wstring to_wstring() const;

  // Converts the string to a string for Win32 API
  void copyTo(LPTSTR dest, int size) const;

  // String <-> int conversion
  static String fromInt(int value, int base = 10, int precision = 0);
  int parseInt(int base = 10) const;

  // String <-> double conversion
  static String fromDouble(double value, int precision);
  double parseDouble() const;

  //////////////////////////////////////////////////////////////////////
  // File names & paths
  
  String getFilePath() const;
  String getFileName() const;
  String getFileExtension() const;
  String getFileTitle() const;

  String addPathComponent(const String& component) const;

//   String removeExtension() const;
//   String replaceExtension(const String& newExtension) const;

};

/**
 * Concatenate a Vaca::String with a normal C string.
 * 
 * With this you can do:
 * @code
 * String res = String("...") + "...";
 * @endcode
 *
 * @return A new string.
 */
inline String operator+(const String& _s1, const char* _s2)
{
  String _res(_s1);
  _res.append(String(_s2));
  return _res;
}

/**
 * Concatenate a Vaca::String with a C string with wide-characters.
 * 
 * With this you can do:
 * @code
 * String res = String("...") + _T("...");
 * @endcode
 *
 * @return A new string.
 */
inline String operator+(const String& _s1, const wchar_t* _s2)
{
  String _res(_s1);
  _res.append(String(_s2));
  return _res;
}

/**
 * Concatenate a normal C string and a Vaca::String.
 * 
 * With this you can do:
 * @code
 * String res = "..." + String("...");
 * @endcode
 *
 * @return A new string.
 */
inline String operator+(const char* _s1, const String& _s2)
{
  String _res(_s1);
  _res.append(_s2);
  return _res;
}

/**
 * Concatenate a C string with wide-characters and a Vaca::String.
 * 
 * With this you can do:
 * @code
 * String res = _T("...") + String("...");
 * @endcode
 *
 * @return A new string.
 */
inline String operator+(const wchar_t* _s1, const String& _s2)
{
  String _res(_s1);
  _res.append(_s2);
  return _res;
}

} // namespace Vaca

#endif
