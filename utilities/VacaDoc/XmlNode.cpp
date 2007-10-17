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

#include <algorithm>
#include <ctype.h>
#include "XmlNode.h"
#include "XmlAttribute.h"
#include "XmlException.h"
#include "XmlString.h"

XmlNode::XmlNode(const String &name)
  : mName(name)
  , mParent(NULL)
{
}

XmlNode::~XmlNode()
{
  for (list<XmlType *>::iterator it=mTypeString.begin();
       it!=mTypeString.end(); ++it)
    if ((*it)->isString())
      delete *it;
  
  for (list<XmlNode *>::iterator it=mChildren.begin();
       it!=mChildren.end(); ++it)
    delete *it;

  for (list<XmlAttribute *>::iterator it=mAttributes.begin();
       it!=mAttributes.end(); ++it)
    delete *it;
}

String XmlNode::getText()
{
  String res;

  for (list<XmlType *>::iterator it=mTypeString.begin();
       it!=mTypeString.end(); ++it) {
    // XmlString
    if ((*it)->isString()) {
      res += (*it)->getText();
    }
//     // XmlNode
//     else if ((*it)->isNode()) {
//       XmlNode *node = (*it)->getNode();
//       list<XmlAttribute *> attributes = node->getAttributes();
//       res += "<" + node->getName();
//       for (list<XmlAttribute *>::iterator it=attributes.begin();
// 	   it!=attributes.end(); ++it) {
// 	res += " " + (*it)->getName() + "=\"" + (*it)->getValue() + "\"";
//       }
//       res += ">";
//       res += node->getText();
//       res += "</" + node->getName() + ">";
//     }
  }
  
  return res;
}

String XmlNode::getName()
{
  return mName;
}

XmlNode *XmlNode::getParent()
{
  return mParent;
}

void XmlNode::addChar(int character)
{
  if (!mTypeString.empty()) {
    XmlString *back = dynamic_cast<XmlString *>(mTypeString.back());
    if (back != NULL) {
      back->addChar(character);
      return;
    }
  }

  XmlString *newString = new XmlString();
  newString->addChar(character);
  mTypeString.push_back(newString);
}

void XmlNode::addChild(XmlNode *child)
{
  child->mParent = this;
  mChildren.push_back(child);

  mTypeString.push_back(child);
}

void XmlNode::addAttribute(XmlAttribute *attribute)
{
  mAttributes.push_back(attribute);
}

list<XmlNode *> XmlNode::getChildren()
{
  return mChildren;
}

list<XmlAttribute *> XmlNode::getAttributes()
{
  return mAttributes;
}

list<XmlType *> XmlNode::getTypeString()
{
  return mTypeString;
}
  
XmlNode *XmlNode::getChild(const String &name)
{
  list<XmlNode *>::iterator it;
  for (it = mChildren.begin(); it != mChildren.end(); ++it) {
    if ((*it)->getName() == name)
      return *it;
  }
  return NULL;
}

XmlAttribute *XmlNode::getAttribute(const String &name)
{
  list<XmlAttribute *>::iterator it;
  for (it = mAttributes.begin(); it != mAttributes.end(); ++it) {
    if ((*it)->getName() == name)
      return *it;
  }
  return NULL;
}

String XmlNode::getAttributeValue(const String &name)
{
  XmlAttribute *attrib = getAttribute(name);
  if (attrib != NULL)
    return attrib->getValue();
  else
    return "";
}

void XmlNode::setAttributeValue(const String &name, const String &value)
{
  XmlAttribute *attrib = getAttribute(name);
  if (attrib != NULL)
    attrib->setValue(value);
  else
    addAttribute(new XmlAttribute(name, value));
}
