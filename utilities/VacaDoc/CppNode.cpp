//-*- coding: utf-8; -*-
// 
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

#include <stack>

#include "Vaca/Debug.h"

#include "CppNode.h"
#include "XmlNode.h"
#include "XmlAttribute.h"

using namespace std;
using namespace Vaca;

CppNode::CppNode(XmlNode *xmlNode, CppNode *parent)
{
  this->solved = false;
  this->xmlNode = xmlNode;
  // the only CppNode that doesn't has a parent is the anonymous namespace
  this->name = parent != NULL ? xmlNode->getAttributeValue("name"): "";
  this->syntax = xmlNode->getAttributeValue("syntax");
  this->parent = parent;
}

CppNode *CppNode::findByXmlNode(XmlNode *xmlNode)
{
  if (this->xmlNode == xmlNode)
    return this;

  for (vector<CppNode *>::iterator it = children.begin();
       it != children.end(); ++it) {
    CppNode *cppNode = (*it)->findByXmlNode(xmlNode);
    if (cppNode != NULL)
      return cppNode;
  }

  return NULL;
}

bool CppNode::resolveReferences()
{
  if (solved)
    return true;
  solved = true;
  
  for (vector<CppNode *>::iterator
	 it = children.begin();
       it != children.end(); ++it)
    (*it)->resolveReferences();

  return false;
}

void CppNode::sortVectors()
{
  sort(children.begin(), children.end(), compNameOfCppNode());

  for (vector<CppNode *>::iterator
	 it = children.begin();
       it != children.end(); ++it)
    (*it)->sortVectors();
}

CppNode *CppNode::resolveCrossRef(const String &name, bool up)
{
  for (vector<CppNode *>::iterator
	 it = children.begin();
       it != children.end(); ++it) {
    CppNode *cppNode = (*it)->resolveCrossRef(name, false);
    if (cppNode != NULL)
      return cppNode;
  }

  // root node
  if (parent == NULL)
    return NULL;

  String elem;
  String::size_type index;

  index = name.find_first_of(':');
    
  // has "namespace1::namespace2..."?
  if (index != String::npos) {
    elem = name.substr(0, index); // elem = "namespace1"

    if (this->name == elem) {
      // find "namespace2..."
      index = name.find_first_not_of(':', index);

      if (index == String::npos)
	return NULL;		// error in the "name"
      // go through children
      else {
	elem = name.substr(index); // elem = "namespace2..."

	for (vector<CppNode *>::iterator
	       it = children.begin();
	     it != children.end(); ++it) {
	  CppNode *cppNode = (*it)->resolveCrossRef(elem, false);
	  if (cppNode != NULL)
	    return cppNode;
	}
      }
    }
  }
  // it's just "namespace1"
  else {
    if (this->name == name)
      return this;
  }

  // go to the parent
  if (up) {
    assert(parent != NULL);

    // search in siblings
    for (vector<CppNode *>::iterator
	   it = parent->children.begin();
	 it != parent->children.end(); ++it) {
      CppNode *cppNode = (*it)->resolveCrossRef(name, false);
      if (cppNode != NULL)
	return cppNode;
    }

    // search in parent
    return parent->resolveCrossRef(name, true);
  }
  else
    return NULL;
}

Enum::Enum(XmlNode *node, Namespace *parent)
: CppNode(node, parent)
{
}

Macro::Macro(XmlNode *node, Namespace *parent)
  : CppNode(node, parent)
{
}

Typedef::Typedef(XmlNode *node, Namespace *parent)
  : CppNode(node, parent)
{
}

Function::Function(XmlNode *node, Namespace *parent)
  : CppNode(node, parent)
{
}

Member::Member(XmlNode *node, Class *parent)
  : CppNode(node, parent)
{
  fromClass = parent;
  isReplaced = false;
  isStatic = syntax.find(" static ") != String::npos;

  access =
    syntax.find("public ") != String::npos ? PublicAccess:
    syntax.find("protected ") != String::npos ? ProtectedAccess:
						PrivateAccess;
}

Namespace::Namespace(XmlNode *node, Namespace *parent)
  : CppNode(node, parent)
{
  //////////////////////////////////////////////////////////////////////
  // add enums, macros, typedefs, functions, namespaces
    
  stack<XmlNode *> nodeStack;

  // add children of "node" to "nodeStack"
  list<XmlNode *> xmlChildren = node->getChildren();
  for (list<XmlNode *>::iterator it = xmlChildren.begin();
       it != xmlChildren.end(); ++it)
    nodeStack.push(*it);

  while (!nodeStack.empty()) {
    node = nodeStack.top();
    nodeStack.pop();

    if (node->getName() == "enum") {
      children.push_back(new Enum(node, this));
    }
    else if (node->getName() == "macro") {
      children.push_back(new Macro(node, this));
    }
    else if (node->getName() == "typedef") {
      children.push_back(new Typedef(node, this));
    }
    else if (node->getName() == "function") {
      children.push_back(new Function(node, this));
    }
    else if (node->getName() == "namespace") {
      children.push_back(new Namespace(node, this));
    }
    else if (node->getName() == "class") {
      children.push_back(new Class(node, this));
    }
    else if (node->getName() == "struct") {
      children.push_back(new Struct(node, this));
    }
    // chapter or section...
    else {
      list<XmlNode *> children = node->getChildren();
      for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	nodeStack.push(*it);
    }
  }

}

Field::Field(XmlNode *node, Class *parent)
  : Member(node, parent)
{
}

Method::Method(XmlNode *node, Class *parent)
  : Member(node, parent)
{
}

Operator::Operator(XmlNode *node, Class *parent)
  : Member(node, parent)
{
}

Event::Event(XmlNode *node, Class *parent)
  : Method(node, parent)
{
}

Signal::Signal(XmlNode *node, Class *parent)
  : Member(node, parent)
{
}

Class::Class(XmlNode *node, Namespace *parent)
  : Namespace(node, parent)
  , baseClass(NULL)
{
  //////////////////////////////////////////////////////////////////////
  // add members
    
  stack<XmlNode *> nodeStack;

  // add children of "node" to "nodeStack"
  list<XmlNode *> xmlChildren = node->getChildren();
  for (list<XmlNode *>::iterator
	 it = xmlChildren.begin(); it != xmlChildren.end(); ++it)
    nodeStack.push(*it);

  while (!nodeStack.empty()) {
    node = nodeStack.top();
    nodeStack.pop();

    if (node->getName() == "field") {
      children.push_back(new Field(node, this));
    }
    else if (node->getName() == "method") {
      children.push_back(new Method(node, this));
    }
    else if (node->getName() == "operator") {
      children.push_back(new Operator(node, this));
    }
    else if (node->getName() == "event") {
      children.push_back(new Event(node, this));
    }
    else if (node->getName() == "signal") {
      children.push_back(new Signal(node, this));
    }
    else {
      list<XmlNode *> children = node->getChildren();
      for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	nodeStack.push(*it);
    }
  }

}

bool Class::resolveReferences()
{
  if (Namespace::resolveReferences())
    return true;

  //////////////////////////////////////////////////////////////////////
  // members

  for (vector<CppNode *>::iterator
	 it = children.begin();
       it != children.end(); ++it) {
    Member *member = dynamic_cast<Member *>(*it);
    if (member != NULL)
      // members.push_back(member->clone());
      members.push_back(member); // TODO warning!!! two pointers to same place
  }

  //////////////////////////////////////////////////////////////////////
  // baseClass and subClasses

  // first try the "base" attribute
  XmlAttribute *baseAttribute = xmlNode->getAttribute("base");
  if (baseAttribute != NULL) {
    baseClassName = baseAttribute->getValue();
  }
  // well, we can use the "syntax"
  else {
    if (syntax.find_last_of(':') != String::npos) {
//       String::reverse_iterator it = syntax.rbegin();
//       String::reverse_iterator end = syntax.rend();

      // the last word of the "syntax" attribute, is the name of the
      // base class
//       for (; it != end && !_istspace(*it); ++it)
// 	baseClassName.push_back(*it);

      int c = syntax.size()-1;

      for (; c>=0 && !_istspace(syntax[c]); --c)
	;
// 	baseClassName.push_back(syntax[c]);

//       reverse(baseClassName.begin(), baseClassName.end());
      baseClassName = syntax.substr(c+1);
    }
  }

  // has base class name?
  if (!baseClassName.empty()) {
    // find the base Class
    baseClass = dynamic_cast<Class *>(resolveCrossRef(baseClassName));
    // exist?
    if (baseClass != NULL) {
      baseClass->subClasses.push_back(this);

      // resolve references of the base class (to get its members)
      baseClass->resolveReferences();

      // more members from the baseClass
      for (MembersIterator
	     it = baseClass->members.begin();
	   it != baseClass->members.end(); ++it) {

	Member *member = *it;
	// this member is overrided?
	Member *memberInThisClass = getSimilarMember(member);

	if (memberInThisClass == NULL) {
// 	  members.push_back(member->clone());
	  members.push_back(member); // TODO warning!!! two pointers to same place
	}
	else
	  memberInThisClass->isReplaced = true;
      }
    }
  }

  // sort members
  sort(members.begin(), members.end(), compNameOfCppNode());

  return false;
}

void Class::sortVectors()
{
  Namespace::sortVectors();

  sort(subClasses.begin(), subClasses.end(), compNameOfCppNode());
}

Member *Class::getSimilarMember(Member *similarTo)
{
  for (vector<CppNode *>::iterator
	 it = children.begin();
       it != children.end(); ++it) {
    Member *member = dynamic_cast<Member *>(*it);

    if (member != NULL &&
	member->name == similarTo->name &&
	member->syntax == similarTo->syntax)
      return member;
  }

  return NULL;
}

Struct::Struct(XmlNode *node, Namespace *parent)
  : Class(node, parent)
{
}
