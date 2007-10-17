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

#include "XmlRawParser.h"
#include "XmlException.h"
#include "XmlStream.h"

// XmlRawParser::XmlRawParser(const String &fileName)
//   : mFileName(fileName)
XmlRawParser::XmlRawParser(XmlStream *stream)
  : mStream(stream)
{
//   mFile = fopen(fileName.c_str(), "rb");
//   if (mFile == NULL) {
//     printf("error loading file");
//     throw;
//   }

  mBuf[mPos = 0] = 0;
  mCurrentLine = 0;

  // read first line of text
  readLine();
}

XmlRawParser::~XmlRawParser()
{
  // close the file
//   fclose(mFile);
}

// return the text inside the next tag (the text inside the <...>)
String XmlRawParser::nextTag()
{
  for (; currentChar() != 0; // end of buffer
       forwardChar(1, false)) { // advance one character in buffer
    // begining of a tag
    if (currentChar() == '<') {
      forwardChar(1, true);	// jump the '<' character

      // comment start?
      if (areNextBytes("!--")) {
	// jump the comment start
	forwardChar(3, true);
	// "eat" the entire comment
	while (!areNextBytes("-->")) { // until the end of comment
	  forwardChar(1, true);
	}
	forwardChar(3, true); // done, we finish to jump the comment
      }
      else {
	String tag;

	while (currentChar() != '>') { // until the end of tag
	  tag.push_back(currentChar()); // add this character
	  forwardChar(1, true);
	}

	forwardChar(1, true);	// jump the '>' character

	return tag;
      }
    }
    else {
      // entity?
      if (currentChar() == '&') {
	String entity;
	int line = mCurrentLine;
	
	for (;;) {
	  entity.push_back(currentChar());
	  forwardChar(1, true);

	  if (currentChar() != ';')
	    break;

	  if (line != mCurrentLine || !isEntityChar(currentChar())) {
	    mCurrentLine = line;
	    throw XmlException("Invalid entity \""+entity+"\"");
	  }
	}

	for (String::iterator
	       it = entity.begin(); it != entity.end(); ++it)
	  onTextOutside(*it);
      }

      onTextOutside(currentChar());
    }
  }
  return String("");
}

String XmlRawParser::getFileName()
{
//   return mFileName;
  return mStream->getName();
}

int XmlRawParser::getCurrentLine()
{
  return mCurrentLine;
}

int XmlRawParser::currentChar()
{
  return mBuf[mPos];
}

void XmlRawParser::forwardChar(int howMany, bool needed)
{
  while (howMany-- > 0) {
    mPos++;

    if (mPos == 256 || mBuf[mPos] == 0) {
      mPos = 0;

      // read a new line of text
      if (!readLine()) {
	if (needed)
	  throw XmlException("End of file not expected.");
	else
	  mBuf[mPos] = 0;	// no more chars
      }
    }
  }
}

bool XmlRawParser::areNextBytes(const char *string)
{
  int hasNbytes = strlen(mBuf+mPos);
  int needNbytes = strlen(string);

  if (hasNbytes < needNbytes) {
    memmove(mBuf, mBuf+mPos, 256-mPos);
    mPos = 0;

    // read a new line of text
    if (!readLine(hasNbytes))
      return false;

    // hasNbytes changes
    hasNbytes = strlen(mBuf+mPos);
  }

  // TODO assert(hasNbytes < needNbytes);

  if (strncmp(mBuf+mPos, string, needNbytes) == 0)
    return true;
  else
    return false;
}

bool XmlRawParser::readLine(int offset)
{
  // bool res = fgets(mBuf, 256, mFile) != NULL;
  bool res = mStream->readLine(mBuf+offset, 256-offset);

  if (res) {
    // next line
    mCurrentLine++;
  }

  return res;
}

bool XmlRawParser::isEntityChar(int ch)
{
  return ((ch == '#') ||
	  (ch >= 'a' && ch <= 'z') ||
	  (ch >= 'A' && ch <= 'Z') ||
	  (ch >= '0' && ch <= '9'));
}

void XmlRawParser::onTextOutside(int ch)
{
  // do nothing
}
