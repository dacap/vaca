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

#ifndef VACA_XMLNODE_H
#define VACA_XMLNODE_H

#include <list>
#include "Vaca/String.h"
#include "XmlType.h"

using namespace std;
using namespace Vaca;

class XmlAttribute;

class XmlNode : public XmlType
{
  String mName;
  XmlNode *mParent;
  list<XmlNode *> mChildren;
  list<XmlAttribute *> mAttributes;
  list<XmlType *> mTypeString;
  
public:

  XmlNode(const String &name);
  virtual ~XmlNode();

  virtual bool isNode() { return true; }
  virtual XmlNode *getNode() { return this; }
  virtual String getText();

  String getName();
  XmlNode *getParent();

  void addChar(int character);
  void addChild(XmlNode *child);
  void addAttribute(XmlAttribute *attribute);

  list<XmlNode *> getChildren();
  list<XmlAttribute *> getAttributes();
  list<XmlType *> getTypeString();

  XmlNode *getChild(const String &name);
  XmlAttribute *getAttribute(const String &name);

  String getAttributeValue(const String &name);
  void setAttributeValue(const String &name, const String &value);
  
};

#endif
