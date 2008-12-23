// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David Capello
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

#include "Vaca/HttpRequest.h"
#include <memory>
#include <cassert>

using namespace Vaca;
using namespace std;

//////////////////////////////////////////////////////////////////////
// HttpRequest::InetHandle

/**
 * @throw HttpRequestException
 */
HttpRequest::InetHandle::InetHandle(HINTERNET handle,
				    const String& errorMsg)
  : handle(handle)
{
  if (!handle)
    throw HttpRequestException(errorMsg);
}

HttpRequest::InetHandle::~InetHandle()
{
  if (handle)
    InternetCloseHandle(handle);
}

//////////////////////////////////////////////////////////////////////
// HttpRequest::InetConnection

HttpRequest::InetConnection::InetConnection(const String& agentName)
  : InetHandle(InternetOpen(agentName.c_str(),
			    INTERNET_OPEN_TYPE_DIRECT,
			    NULL, NULL, 0),
	       "Can't connect to Internet")
{
}

//////////////////////////////////////////////////////////////////////
// HttpRequest::InetSession

HttpRequest::InetSession::InetSession(InetConnection& conn,
				      const String& host)
  : InetHandle(InternetConnect(conn.handle,
			       host.c_str(),
			       INTERNET_DEFAULT_HTTP_PORT,
			       NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0),
	       String("Can't open a HTTP session with ") + host)
{
}

//////////////////////////////////////////////////////////////////////
// HttpRequest::InetRequest

HttpRequest::InetRequest::InetRequest(InetSession& ses,
				      const String& method,
				      const String& object)
  : InetHandle(HttpOpenRequest(ses.handle,
			       method.c_str(),
			       object.c_str(),
			       NULL, NULL, NULL,
			       INTERNET_FLAG_NO_UI, 0),
	       "Can't send a " + method + " request to " + object)
{
}

//////////////////////////////////////////////////////////////////////
// HttpRequest

HttpRequest::HttpRequest(const String& url,
			 const String& method)
    : conn("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)")
    , ses(conn, url.getUrlHost())
    , req(ses, method, url.getUrlObject())
{
}

HttpRequest::~HttpRequest()
{
}

/**
 * Sends the HTTP request.
 * 
 * @return
 *   The status code of the HTTP response that comes from the
 *   server.
 */
int HttpRequest::send(const String& headers, const char* body)
{
  if (HttpSendRequest(req.handle,
		      headers.empty() ? NULL: headers.c_str(),
		      headers.size(),
		      reinterpret_cast<LPVOID>(const_cast<char*>(body)),
		      body ? strlen(body): 0)) {
    return getStatusCode();
  }
  return 0;
}

/**
 * Reads the specified amount of bytes from the stream of bytes.
 *
 * @param buf
 *   Where to put the read data. Must have at least @a length bytes.
 *
 * @param length
 *   How many bytes to read. The size of the buffer.
 * 
 * @return
 *   The number of bytes that were read.
 *
 * @throw HttpRequestException
 */
size_t HttpRequest::read(char* buf, size_t length)
{
  assert(buf != NULL);
  assert(length > 0);
  
  DWORD bytes = 0;
  if (!InternetReadFile(req.handle, buf, length, &bytes))
    throw HttpRequestException();

  return bytes;
}

int HttpRequest::getStatusCode()
{
  DWORD statusCode = 0;
  DWORD bufLength = sizeof(DWORD);
  DWORD index = 0;

  if (HttpQueryInfo(req.handle,
		    HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		    &statusCode,
		    &bufLength, 
		    &index)) {
    return statusCode;
  }
  else
    return 0;
}

size_t HttpRequest::getContentLength()
{
  DWORD contentLength = 0;
  DWORD bufLength = sizeof(DWORD);
  DWORD index = 0;

  if (HttpQueryInfo(req.handle,
		    HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		    &contentLength,
		    &bufLength, 
		    &index)) {
    return contentLength;
  }
  else
    return 0;
}

bool HttpRequest::hasHeader(const String& headerName)
{
  DWORD bufLength = headerName.size()+1;
  auto_ptr<char> buf(new char[bufLength]);
  DWORD index = 0;

  strcpy(buf.get(), headerName.to_string().c_str());

  if (HttpQueryInfo(req.handle, HTTP_QUERY_CUSTOM,
		    buf.get(), &bufLength, &index)) {
    return true;
  }
  else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    return true;
  else
    return false;
}

/**
 * Returns the value of the specified header.
 * 
 * @return
 *   The value of the specified header, or an empty string if the
 *   header is not found.
 *
 * @throw HttpRequestException
 *   If there are an error getting the header.
 */
String HttpRequest::getHeader(const String& headerName)
{
  DWORD bufLength = headerName.size()+1;
  auto_ptr<char> buf(new char[bufLength]);
  DWORD index = 0;

  while (true) {
    // copy the header's name in the buffer 'buf'
    strcpy(buf.get(), headerName.to_string().c_str());

    // get the header's value
    if (HttpQueryInfo(req.handle, HTTP_QUERY_CUSTOM,
		      buf.get(), &bufLength, &index)) {
      // ok
      return String(buf.get());
    }
    else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      // the old value is deleted by auto_ptr::operator=
      buf = auto_ptr<char>(new char[bufLength]);
      continue;
    }
    else if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND)
      return String();
    else
      throw HttpRequestException("Error retrieving header");
  }
}
