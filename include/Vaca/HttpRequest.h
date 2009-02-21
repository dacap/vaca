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

#ifndef VACA_HTTPREQUEST_H
#define VACA_HTTPREQUEST_H

#include "Vaca/base.h"
#include "Vaca/NonCopyable.h"
#include "Vaca/Exception.h"

#include <wininet.h>

namespace Vaca {

/// This exception is thrown when HttpRequest generates an error due
/// connectivity problems.
/// 
class VACA_DLL HttpRequestException : public Exception
{
public:
  HttpRequestException() : Exception() { }
  HttpRequestException(const String& message) : Exception(message) { }
  virtual ~HttpRequestException() throw() { }
};

/// Sends a HTTP request to the specified URL to get its content.
/// 
class VACA_DLL HttpRequest
{
  class InetHandle {
  public:
    HINTERNET handle;
    InetHandle(HINTERNET handle, const String& errorMsg);
    virtual ~InetHandle();
  };

  class InetConnection : public InetHandle {
  public:
    InetConnection(const String& agentName);
  };

  class InetSession : public InetHandle {
  public:
    InetSession(InetConnection& conn, const String& host);
  };

  class InetRequest : public InetHandle {
  public:
    InetRequest(InetSession& ses,
		const String& method,
		const String& object);
  };

  InetConnection conn;
  InetSession ses;
  InetRequest req;

public:
  HttpRequest(const String& url, const String& method = L"GET");
  virtual ~HttpRequest();

  int send(const String& headers = L"", const char* body = NULL);
  std::size_t read(char* buf, std::size_t length);

  int getStatusCode();
  std::size_t getContentLength();

  bool hasHeader(const String& headerName);
  String getHeader(const String& headerName);

};

} // namespace Vaca

#endif // VACA_HTTPREQUEST_H
