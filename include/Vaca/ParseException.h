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

#ifndef VACA_PARSEEXCEPTION_H
#define VACA_PARSEEXCEPTION_H

#include "Vaca/Exception.h"

namespace Vaca {

/// This exception is thrown when a routine that parse a string has
/// failed due ill-formed input.
/// 
/// @see Bix#parse
/// 
class ParseException : public Exception
{
  int m_line;
  int m_column;
  int m_index;
public:

  /// Creates a ParserException without more information.
  /// 
  ParseException() throw()
    : Exception()
    , m_line(-1)
    , m_column(-1)
    , m_index(-1)
  {
  }

  /// Creates a ParserException with full information: error message,
  /// line and column number where the error was found inside the
  /// parsed-string.
  /// 
  /// @param message Error message.
  /// @param line Line number (1 should be the first line).
  /// @param column Column number (0 should be the first column).
  /// @param index Index of the character where the error was found (starting in 0).
  /// 
  ParseException(const String& message, int line = -1, int column = -1, int index = -1) throw()
    : Exception(message)
    , m_line(line)
    , m_column(column)
    , m_index(index)
  {
  }

  /// Destroys the exception.
  /// 
  virtual ~ParseException() throw()
  {
  }

  /// Returns the line number where the error was found.
  /// 
  /// @warning 1 should be the first line.
  /// 
  /// @see #getColumn
  /// 
  int getLine() const
  {
    return m_line;
  }

  /// Returns the column number where the error was found.
  /// 
  /// @warning 0 should be the first column.
  /// 
  /// @see #getLine
  /// 
  int getColumn() const
  {
    return m_column;
  }

  /// Returns the index of the character within the string where the
  /// error was found.
  /// 
  /// @warning 0 should be the first character.
  /// 
  int getIndex() const
  {
    return m_index;
  }

};

} // namespace Vaca

#endif // VACA_PARSEEXCEPTION_H
