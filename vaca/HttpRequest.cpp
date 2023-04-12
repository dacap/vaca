// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/HttpRequest.h"
#include "vaca/String.h"

#include <cassert>
#include <cstring>
#include <vector>

using namespace vaca;

// ======================================================================
// HttpRequest::InetHandle

/**
   @throw HttpRequestException
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

// ======================================================================
// HttpRequest::InetConnection

HttpRequest::InetConnection::InetConnection(const String& agentName)
  : InetHandle(InternetOpen(agentName.c_str(),
			    INTERNET_OPEN_TYPE_DIRECT,
			    NULL, NULL, 0),
	       L"Can't connect to Internet")
{
}

// ======================================================================
// HttpRequest::InetSession

HttpRequest::InetSession::InetSession(InetConnection& conn,
				      const String& host)
  : InetHandle(InternetConnect(conn.handle,
			       host.c_str(),
			       INTERNET_DEFAULT_HTTP_PORT,
			       NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0),
	       String(L"Can't open a HTTP session with ") + host)
{
}

// ======================================================================
// HttpRequest::InetRequest

HttpRequest::InetRequest::InetRequest(InetSession& ses,
				      const String& method,
				      const String& object)
  : InetHandle(HttpOpenRequest(ses.handle,
			       method.c_str(),
			       object.c_str(),
			       NULL, NULL, NULL,
			       INTERNET_FLAG_NO_UI, 0),
	       L"Can't send a " + method + L" request to " + object)
{
}

// ======================================================================
// HttpRequest

HttpRequest::HttpRequest(const String& url,
			 const String& method)
    : conn(L"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)")
    , ses(conn, url_host(url))
    , req(ses, method, url_object(url))
{
}

HttpRequest::~HttpRequest()
{
}

/**
   Sends the HTTP request.

   @return
     The status code of the HTTP response that comes from the
     server.
*/
int HttpRequest::send(const String& headers, const char* body)
{
  if (HttpSendRequest(req.handle,
		      headers.empty() ? nullptr: headers.c_str(),
		      headers.size(),
		      reinterpret_cast<LPVOID>(const_cast<char*>(body)),
		      body ? std::strlen(body): 0)) {
    return getStatusCode();
  }
  return 0;
}

/**
   Reads the specified amount of bytes from the stream of bytes.

   @param buf
     Where to put the read data. Must have at least @a length bytes.

   @param length
     How many bytes to read. The size of the buffer.

   @return
     The number of bytes that were read.

   @throw HttpRequestException
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
  std::vector<char> buf(bufLength);
  DWORD index = 0;

  std::strcpy(&buf[0], convert_to<std::string>(headerName).c_str());

  if (HttpQueryInfo(req.handle, HTTP_QUERY_CUSTOM,
		    &buf[0], &bufLength, &index)) {
    return true;
  }
  else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    return true;
  else
    return false;
}

/**
   Returns the value of the specified header.

   @return
     The value of the specified header, or an empty string if the
     header is not found.
*/
String HttpRequest::getHeader(const String& headerName)
{
  DWORD bufLength = headerName.size()+1;
  std::vector<char> buf(bufLength);
  DWORD index = 0;

  while (true) {
    // copy the header's name in the buffer 'buf'
    std::strcpy(&buf[0], convert_to<std::string>(headerName).c_str());

    // get the header's value
    if (HttpQueryInfo(req.handle, HTTP_QUERY_CUSTOM,
		      &buf[0], &bufLength, &index)) {
      // ok
      return convert_to<String, const char*>(&buf[0]);
    }
    else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      buf.resize(bufLength);
      continue;
    }
    else
      return String(L"");
  }
}
