// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <vaca/vaca.h>

using namespace vaca;

typedef String::size_type DocPos;

class Document : public Referenceable
{
  String m_name;
  String m_string;

public:

  Document(const String& name) : m_name(name) {
  }

  ~Document() {
  }

  String getName() {
    return m_name;
  }

  Char at(DocPos pos) const {
    return m_string.at(pos);
  }

  DocPos size() const {
    return m_string.size();
  }

  void add(DocPos pos, Char chr) {
    m_string.insert(pos, 1, chr);
  }

  void add(DocPos pos, const String& s) {
    m_string.insert(pos, s);
  }

  void remove(DocPos pos, DocPos n) {
    m_string.erase(pos, n);
  }

};

typedef SharedPtr<Document> DocumentPtr;

#endif // DOCUMENT_H
