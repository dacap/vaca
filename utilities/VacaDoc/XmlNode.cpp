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

#include <ctype.h>
#include "XmlNode.h"
#include "XmlAttribute.h"
#include "XmlException.h"
#include "XmlString.h"

XmlNode::XmlNode(const Vaca::String &name)
  : mName(name)
  , mParent(NULL)
{
}

XmlNode::~XmlNode()
{
  for (std::list<XmlType *>::iterator it=mTypeString.begin();
       it!=mTypeString.end(); ++it)
    if ((*it)->isString())
      delete *it;
  
  for (std::list<XmlNode *>::iterator it=mChildren.begin();
       it!=mChildren.end(); ++it)
    delete *it;

  for (std::list<XmlAttribute *>::iterator it=mAttributes.begin();
       it!=mAttributes.end(); ++it)
    delete *it;
}

Vaca::String XmlNode::getText()
{
  Vaca::String res;

  for (std::list<XmlType *>::iterator it=mTypeString.begin();
       it!=mTypeString.end(); ++it) {
    // XmlString
    if ((*it)->isString()) {
      res += (*it)->getText();
    }
//     // XmlNode
//     else if ((*it)->isNode()) {
//       XmlNode *node = (*it)->getNode();
//       std::list<XmlAttribute *> attributes = node->getAttributes();
//       res += "<" + node->getName();
//       for (std::list<XmlAttribute *>::iterator it=attributes.begin();
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

Vaca::String XmlNode::getName()
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
    XmlType *back = mTypeString.back();
    if (back->isString()) {
      (static_cast<XmlString *>(back))->addChar(character);
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

std::list<XmlNode *> XmlNode::getChildren()
{
  return mChildren;
}

std::list<XmlAttribute *> XmlNode::getAttributes()
{
  return mAttributes;
}

std::list<XmlType *> XmlNode::getTypeString()
{
  return mTypeString;
}
  
XmlNode *XmlNode::getChild(const Vaca::String &name)
{
  std::list<XmlNode *>::iterator it;
  for (it = mChildren.begin(); it != mChildren.end(); ++it) {
    if ((*it)->getName() == name)
      return *it;
  }
  return NULL;
}

XmlAttribute *XmlNode::getAttribute(const Vaca::String &name)
{
  std::list<XmlAttribute *>::iterator it;
  for (it = mAttributes.begin(); it != mAttributes.end(); ++it) {
    if ((*it)->getName() == name)
      return *it;
  }
  return NULL;
}

Vaca::String XmlNode::getAttributeValue(const Vaca::String &name)
{
  XmlAttribute *attrib = getAttribute(name);
  if (attrib != NULL)
    return attrib->getValue();
  else
    return "";
}

void XmlNode::setAttributeValue(const Vaca::String &name, const Vaca::String &value)
{
  XmlAttribute *attrib = getAttribute(name);
  if (attrib != NULL)
    attrib->setValue(value);
  else
    addAttribute(new XmlAttribute(name, value));
}
