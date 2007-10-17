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

#ifndef VACA_CPPNODE_H
#define VACA_CPPNODE_H

#include <vector>
#include "Vaca/String.h"

using namespace std;
using namespace Vaca;

class XmlNode;

enum Access {
  PublicAccess,
  ProtectedAccess,
  PrivateAccess
};

class Member;
class Namespace;
class Class;

typedef vector<Member *> Members;
typedef Members::iterator MembersIterator;

class CppNode
{
public:
  bool solved;
  XmlNode *xmlNode;
  String name;
  String syntax;
  CppNode *parent;
  vector<CppNode *> children;

  CppNode(XmlNode *xmlNode, CppNode *parent);
  virtual ~CppNode() { }

  virtual CppNode *findByXmlNode(XmlNode *xmlNode);
  virtual bool resolveReferences();
  virtual void sortVectors();
  virtual CppNode *resolveCrossRef(const String &name, bool up = true);

  virtual String getKind() { return "unknown"; }

};

struct compNameOfCppNode {

  bool operator()(CppNode *node1, CppNode *node2) const
  {
    return (node1->name < node2->name);
  }
  
};

class Enum : public CppNode
{
public:

  Enum(XmlNode *node, Namespace *parent);

  virtual String getKind() { return "enum"; }
  
};

class Macro : public CppNode
{
public:

  Macro(XmlNode *node, Namespace *parent);

  virtual String getKind() { return "macro"; }
  
};

class Typedef : public CppNode
{
public:

  Typedef(XmlNode *node, Namespace *parent);

  virtual String getKind() { return "typedef"; }

};

class Function : public CppNode
{
public:

  Function(XmlNode *node, Namespace *parent);

  virtual String getKind() { return "function"; }
  
};

class Member : public CppNode
{
public:

  Class *fromClass;
  bool isReplaced;
  bool isStatic;
  Access access;

  Member(XmlNode *node, Class *parent);

  virtual String getKind() { return "member"; }
  
};

class Namespace : public CppNode
{
public:
  
  Namespace(XmlNode *node, Namespace *parent);

  virtual String getKind() { return "namespace"; }
  
};

class Field : public Member
{
public:

  Field(XmlNode *node, Class *parent);

  virtual String getKind() { return "field"; }
  
};

class Method : public Member
{
public:

  Method(XmlNode *node, Class *parent);

  virtual String getKind() { return "method"; }
  
};

class Operator : public Member
{
public:

  Operator(XmlNode *node, Class *parent);

  virtual String getKind() { return "operator"; }
  
};

class Event : public Method
{
public:

  Event(XmlNode *node, Class *parent);

  virtual String getKind() { return "event"; }
  
};

class Signal : public Member
{
public:

  Signal(XmlNode *node, Class *parent);

  virtual String getKind() { return "signal"; }
  
};

class Class : public Namespace
{
public:

  String baseClassName;
  Class *baseClass;
  vector<Class *> subClasses;
  Members members;

  Class(XmlNode *node, Namespace *parent);

  virtual bool resolveReferences();
  virtual void sortVectors();

  Member *getSimilarMember(Member *similarTo);
  
  virtual String getKind() { return "class"; }
  
};

class Struct : public Class
{
public:

  Struct(XmlNode *node, Namespace *parent);

  virtual String getKind() { return "struct"; }
  
};

#endif
