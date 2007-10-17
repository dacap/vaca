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

#include <stdio.h>

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <algorithm>
#include <boost/signal.hpp>
#include <boost/bind.hpp>

// #include <boost/filesystem.hpp>
// #include <boost/filesystem/operations.hpp>
// #include <boost/filesystem/fstream.hpp>
// #include <boost/regex.hpp>
// #include <boost/algorithm/string/regex.hpp>
// #include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Vaca/Application.h"
#include "Vaca/String.h"
#include "Vaca/Exception.h"
#include "Vaca/System.h"

#include "XmlParser.h"
#include "XmlNode.h"
#include "XmlAttribute.h"
#include "XmlStream.h"

using namespace std;
// using namespace boost;
// using namespace boost::filesystem;
// using namespace boost::algorithm;
using namespace Vaca;

// enum Access {
//   Public,
//   Protected,
//   Private, 
// };

// template<class Container, class Type>
// void remove_element(Container &container, const Type &element)
// {
//   container.erase(std::remove(container.begin(),
// 			      container.end(),
// 			      element),
// 		  container.end());
// }

// template<class Container>
// void print_container(Container &container)
// {
//   typename Container::iterator it;
//   for (it=container.begin(); it != container.end(); ++it)
//     cout << *it << " ";
//   cout << endl;
// }

// class Node
// {
// public:
//   XmlNode *node;
//   String name;
// };

// class Member : public Node
// {
// public:
//   String type;
//   String syntax;
// };

// class Field : public Member
// {
// public:
// };

// class Method : public Member
// {
// public:
// };

// class Event : public Member
// {
// public:
// };

// class Signal : public Member
// {
// public:
// };

// class Namespace : public Node
// {
// public:
// };

// class Class : public Namespace
// {
// public:
//   Namespace *parent;
// };

//////////////////////////////////////////////////////////////////////
// Helper class to write in a file

class File
{
  FILE *mFile;
  
public:

  File(const String &fileName)
  {
    mFile = fopen(fileName.c_str(), "wb");
    if (mFile == NULL)
      throw Exception("Error creating file '"+fileName+"'");
  }

  virtual ~File()
  {
    // close the file
    fclose(mFile);
  }

  void println(const String &string)
  {
    String output = convert_utf8_string_to_latin1(string + "\r\n");
    unsigned len = output.size();
    if (fwrite(output.c_str(), 1, len, mFile) < len)
      throw Exception("Can't write in file");
  }

private:

  static String convert_utf8_string_to_latin1(const String &string)
  {
    int len = string.size()+1;
    LPWSTR wideBuf = new WCHAR[len];
    LPSTR ansiBuf = new CHAR[len];
    int ret;
    ret = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), len, wideBuf, len);
    if (ret == 0) {
      printf("%u\n", (int)GetLastError());
      fflush(stdout);
      throw;
    }
    ret = WideCharToMultiByte(CP_ACP, 0, wideBuf, len, ansiBuf, len, NULL, NULL);
    if (ret == 0) {
      printf("%u\n", (int)GetLastError());
      fflush(stdout);
      throw;
    }
    String result = String(ansiBuf);
    delete wideBuf;
    delete ansiBuf;
    return result;
  }

};

//////////////////////////////////////////////////////////////////////
// Documentation Generator

namespace NodeEx
{

  struct compNameAttributeOfNodes {
    bool operator()(XmlNode *node1, XmlNode *node2) const
    {
      return node1->getAttributeValue("name") < node2->getAttributeValue("name");
    }
  };

  struct splitPredicate {
    bool operator()(const Character &ch) const { return ch == ':'; }
  };
  
  String getSingularOf(const String &name)
  {
    if (name == "namespace")     return "Espacio de nombre";
    else if (name == "chapter")  return "Capítulo";
    else if (name == "section")  return "Sección";
    else if (name == "class")    return "Clase";
    else if (name == "struct")   return "Estructura";
    else if (name == "enum")     return "Enumeración";
    else if (name == "typedef")  return "Tipo de dato";
    else if (name == "macro")    return "Macro";
    else if (name == "function") return "Función";
    else if (name == "ctor")     return "Constructor";
    else if (name == "dtor")     return "Destructor";
    else if (name == "field")    return "Campo";
    else if (name == "method")   return "Método";
    else if (name == "operator") return "Operador";
    else if (name == "event")    return "Evento";
    else if (name == "signal")   return "Señal";
    else return "";
  }
  
  String getPluralOf(const String &name)
  {
    if (name == "namespace")     return "Espacios de nombres";
    else if (name == "chapter")  return "Capítulos";
    else if (name == "section")  return "Secciones";
    else if (name == "class")    return "Clases";
    else if (name == "struct")   return "Estructuras";
    else if (name == "enum")     return "Enumeración";
    else if (name == "typedef")  return "Tipos de datos";
    else if (name == "macro")    return "Macros";
    else if (name == "function") return "Funciones";
    else if (name == "ctor")     return "Constructores";
    else if (name == "dtor")     return "Destructores";
    else if (name == "field")    return "Campos";
    else if (name == "method")   return "Métodos";
    else if (name == "operator") return "Operadores";
    else if (name == "event")    return "Eventos";
    else if (name == "signal")   return "Señales";
    else return "";
  }

// gets the name of this node, if this node hasn't a name (like <ctor>
  // or <dtor>), returns the name of the parent
  String getNameAttributeOfNode(XmlNode *node)
  {
    if (node != NULL) {
      XmlAttribute *name = node->getAttribute("name");
      if (name != NULL)
	return name->getValue();
      else
	return getNameAttributeOfNode(node->getParent());
    }
    else
      return "";
  }

  String resolveFileName(XmlNode *tag)
  {
    String fileName = tag->getName() + ".";

    // stack of parents tag
    stack<XmlNode *> parentsStack;

    XmlNode *parent = tag->getParent();
    while (parent != NULL) {
      if (parent->getName() == "namespace" ||
	  parent->getName() == "class" ||
	  parent->getName() == "struct" ||
	  parent->getName() == "enum" ||
	  parent->getName() == "typedef" ||
	  parent->getName() == "macro" ||
	  parent->getName() == "function") {
	parentsStack.push(parent);
      }
      parent = parent->getParent();
    }
      
    while (!parentsStack.empty()) {
      parent = parentsStack.top();
      parentsStack.pop();

      fileName += getNameAttributeOfNode(parent) + ".";
    }

    if (tag->getName() == "operator") {
      String op = getNameAttributeOfNode(tag);
      //       fileName += "operator";
      for (String::iterator it = op.begin(); it!=op.end(); ++it) {
	if ((*it >= 'a' && *it <= 'z' ) ||
	    (*it >= 'A' && *it <= 'Z' ) ||
	    (*it >= '0' && *it <= '9' ) ||
	    (*it == '_'))
	  fileName.push_back(*it);
	else
	  fileName += String::fromInt(*it, 16, 2);
      }
    }
    else if (tag->getName() == "ctor") {
      fileName += "Constructor";
    }
    else if (tag->getName() == "dtor") {
      fileName += "Destructor";
    }
    else
      fileName += getNameAttributeOfNode(tag);
    
    return fileName;
  }

  XmlNode *resolveCrossRef(XmlNode *node, const String &name)
  {
    list<XmlNode *> nodes;
    list<XmlNode *> visited;
    vector<VACA_STRING_BASE > names;
    bool addChildren;
    bool addParent;

    // split something like "MyNamespace::MyClass" to "MyNamespace" and "MyClass"
    boost::algorithm::split(names, name, splitPredicate(), boost::algorithm::token_compress_on);
    
//     nodes.push_back(node->getParent());
    nodes.push_back(node);

    while (!nodes.empty()) {
      node = nodes.front();
      nodes.erase(nodes.begin());

      // this node wasn't visited?
      if (find(visited.begin(), visited.end(), node) == visited.end()) {
	visited.push_back(node);

	addChildren = false;
	addParent = true;

	if (node->getName() == "namespace" ||
	    node->getName() == "class" ||
	    node->getName() == "struct" ||
	    node->getName() == "enum" ||
	    node->getName() == "typedef" ||
	    node->getName() == "macro" ||
	    node->getName() == "function" ||
	    node->getName() == "field" ||
	    node->getName() == "method" ||
	    node->getName() == "operator" ||
	    node->getName() == "event" ||
	    node->getName() == "signal") {
	  if (node->getAttributeValue("name") == names[0]) {
	    // this is the last target?
	    if (names.size() == 1)
	      return node;
	    // we enter in a namespace?
	    else {
	      // we resolve one name...
	      names.erase(names.begin());

	      // we can remove all visited nodes, and all the "nodes"
	      // to visit, because from here, we must to visit only
	      // the children of this node
	      nodes.clear();
	      visited.clear();
	      visited.push_back(node);

	      addChildren = true;
	      addParent = false;
	    }
	  }
	  else if (node->getName() == "namespace" ||
		   node->getName() == "class" ||
		   node->getName() == "struct") {
	    addChildren = true;
	  }
	}
	else if (node->getName() == "doc" ||
		 node->getName() == "chapter" ||
		 node->getName() == "section") {
	  addChildren = true;
	}

	// go to children
	if (addChildren) {
	  list<XmlNode *> children = node->getChildren();
	  for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	    nodes.push_back(*it);
	}

	// go to parent
	if (addParent) {
	  if (node->getParent() != NULL)
	    nodes.push_back(node->getParent());
	}
      }
    }

    return NULL;
  }

  String preProcessTitle(XmlNode *tag)
  {
    String name = getNameAttributeOfNode(tag);
    if (tag->getName() == "namespace")     return name + " (Espacio de nombres)";
    else if (tag->getName() == "class")    return name + " (Clase)";
    else if (tag->getName() == "struct")   return name + " (Estructura)";
    else if (tag->getName() == "enum")     return name + " (Enumeración)";
    else if (tag->getName() == "typedef")  return name + " (Tipo de dato)";
    else if (tag->getName() == "macro")    return name + " (Macro)";
    else if (tag->getName() == "function") return name + " (Función)";
    else if (tag->getName() == "ctor")     return name + " (Constructor)";
    else if (tag->getName() == "dtor")     return name + " (Destructor)";
    else if (tag->getName() == "field")    return name + " (Campo)";
    else if (tag->getName() == "method")   return name + " (Método)";
    else if (tag->getName() == "operator") return name + " (Operador)";
    else if (tag->getName() == "event")    return name + " (Evento)";
    else if (tag->getName() == "signal")   return name + " (Señal)";
    return tag->getAttributeValue("title");
  }

  String preProcessSyntax(XmlNode *node)
  {
    String syntax = node->getAttributeValue("syntax");
    String::const_iterator it = syntax.begin();
    String::const_iterator end = syntax.end();
    String res;

    for (; it != end && *it != '\0'; ++it) {
      if (*it == '(' && (*(it+1)) != ')') {
	res += "(<br />   ";
      }
      else if (*it == ':') {
	res += ":<br />  ";
      }
      else if (*it == ',') {
	res += ",<br />  ";
      }
      else if (*it == ')' && (*(it-1)) != '(') {
	res += "<br />)";
      }
      else {
	// reference to other class?
	if ((*it >= 'a' && *it <= 'z') ||
	    (*it >= 'A' && *it <= 'Z') ||
	    (*it == '_')) {
	  String identifier;
	  identifier.push_back(*(it++));

	  for (; it != end && *it != '\0' && ((*it >= 'a' && *it <= 'z') ||
					      (*it >= 'A' && *it <= 'Z') ||
					      (*it == '_') || (*it == ':'));
	       ++it)
	    identifier.push_back(*it);
	  it--;

	  XmlNode *ref = resolveCrossRef(node, identifier);
	  if (ref != NULL && node != ref) {
	    res += "<a href=\""+resolveFileName(ref)+".html\">" + identifier + "</a>";
	  }
	  else {
	    res += identifier;
	  }
	}
	else {
	  res.push_back(*it);
	}
      }
    }
    
    return res;
  }

  String getSectionTitle(XmlNode *tag)
  {
    String title;
    XmlAttribute *titleAttrib = tag->getAttribute("title");

    if (titleAttrib == NULL)
      title = preProcessTitle(tag);
    else
      title = titleAttrib->getValue();

    return title;
  }

  // gets the base class node (or NULL if it doesn't exist)
  XmlNode *getBaseClass(XmlNode *classNode)
  {
    String baseClassName;

    // first try the "base" attribute
    XmlAttribute *baseAttribute = classNode->getAttribute("base");
    if (baseAttribute != NULL) {
      baseClassName = baseAttribute->getValue();
    }
    // well, we can use the "syntax"
    else {
      XmlAttribute *syntaxAttribute = classNode->getAttribute("syntax");
      if (syntaxAttribute == NULL)
	return NULL;

      String syntax = syntaxAttribute->getValue();

      if (strchr(syntax.c_str(), ':') == NULL)
	return NULL;
    
      String::reverse_iterator it = syntax.rbegin();
      String::reverse_iterator end = syntax.rend();

      // the last word of the "syntax" attribute, is the name of the
      // base class
      for (; it != end && !isspace(*it); ++it)
	baseClassName.push_back(*it);

      reverse(baseClassName.begin(), baseClassName.end());
    }

    return resolveCrossRef(classNode, baseClassName);
  }

  bool hasMembers(XmlNode *classNode)
  {
    list<XmlNode *> members = classNode->getChildren();
    list<XmlNode *>::iterator it;

    for (it = members.begin(); it != members.end(); ++it) {
      if ((*it)->getName() == "ctor" ||
	  (*it)->getName() == "dtor" ||
	  (*it)->getName() == "field" ||
	  (*it)->getName() == "method" ||
	  (*it)->getName() == "operator" ||
	  (*it)->getName() == "event" ||
	  (*it)->getName() == "signal")
	return true;
    }

    XmlNode *base = getBaseClass(classNode);
    if (base != NULL)
      return hasMembers(base);
    else
      return false;
  }

  vector<XmlNode *> getSubClasses(XmlNode *classNode, XmlNode *root)
  {
    vector<XmlNode *> subclasses;
    stack<XmlNode *> nodeStack;
    nodeStack.push(root);

    while (!nodeStack.empty()) {
      XmlNode *node = nodeStack.top();
      nodeStack.pop();

      if (node != classNode &&
	  (node->getName() == "class" ||
	   node->getName() == "struct")) {
	// classNode is the baseclass of node?
	if (classNode == getBaseClass(node))
	  subclasses.push_back(node);
      }

      list<XmlNode *> children = node->getChildren();
      for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	nodeStack.push(*it);
    }

    sort(subclasses.begin(), subclasses.end(), compNameAttributeOfNodes());

    return subclasses;
  }

  vector<XmlNode *> getClassMembers(XmlNode *classNode)
  {
    String className = classNode->getAttributeValue("name");
    vector<XmlNode *> members;
    list<XmlNode *> children = classNode->getChildren();

    for (list<XmlNode *>::iterator it = children.begin();
	 it != children.end(); ++it) {
      if ((*it)->getName() == "ctor" ||
	  (*it)->getName() == "dtor" ||
	  (*it)->getName() == "field" ||
	  (*it)->getName() == "method" ||
	  (*it)->getName() == "operator" ||
	  (*it)->getName() == "event" ||
	  (*it)->getName() == "signal") {
	if ((*it)->getAttribute("__origClass") == NULL)
	  (*it)->addAttribute(new XmlAttribute("__origClass",
					       className));

	members.push_back(*it);
      }
    }

    XmlNode *base = getBaseClass(classNode);
    if (base != NULL) {
      vector<XmlNode *> baseMembers = getClassMembers(base);
      vector<XmlNode *>::iterator it;
      vector<XmlNode *>::iterator it2;

      for (it = baseMembers.begin(); it != baseMembers.end(); ++it) {
	for (it2 = members.begin(); it2 != members.end(); ++it2) {
	  if ((*it)->getAttributeValue("name") == (*it2)->getAttributeValue("name") &&
	      ((*it2)->getAttributeValue("__origClass") == className)) {
	    (*it2)->setAttributeValue("__replaced", "true");
	    break;
	  }
	}

	if (it2 == members.end())
	  members.push_back(*it);
      }
    }

    return members;
  }

} // NodeEx namespace

using namespace NodeEx;

class TocGenerator : public File
{
public:

  TocGenerator(const String &fileName)
    : File(fileName)
  {
  }

  void generate(XmlNode *root)
  {
    addToc(root);
  }

  boost::signal<void (const String &fileName)> NewTocFile;

private:
  
  void addToc(XmlNode *tag)
  {
    list<XmlNode *> children = tag->getChildren();
    String name = getNameAttributeOfNode(tag);

    if (tag->getName() == "chapter" ||
	tag->getName() == "section" ||
	tag->getName() == "namespace" ||
	tag->getName() == "class" ||
	tag->getName() == "struct" ||
	tag->getName() == "enum" ||
	tag->getName() == "typedef" ||
	tag->getName() == "macro" ||
	tag->getName() == "function") {
      // section name is mandatory
      if (name.empty())
	throw Exception("Tag '"+tag->getName()+"' without 'name' attribute.");

      // title of the section
      String title = getSectionTitle(tag);

      // is a leaf?
      bool leaf = children.empty();

      if (tag->getName() == "chapter" ||
	  tag->getName() == "section" ||
	  tag->getName() == "namespace") {
	if (!tag->getChild("section") &&
	    !tag->getChild("namespace") &&
	    !tag->getChild("class") &&
	    !tag->getChild("struct") &&
	    !tag->getChild("enum") &&
	    !tag->getChild("typedef") &&
	    !tag->getChild("macro") &&
	    !tag->getChild("function"))
	  leaf = true;
      }
      else if (tag->getName() == "class" ||
	       tag->getName() == "struct")
	leaf = !hasMembers(tag);
      else if (tag->getName() == "enum" ||
	       tag->getName() == "typedef" ||
	       tag->getName() == "macro" ||
	       tag->getName() == "function")
	leaf = true;

      // add the TOC entry
      println(genToc(resolveFileName(tag)+".html",
		     title,
		     leaf));
    }

    println("<UL>");

    // "class" is special
    if (tag->getName() == "class" ||
	tag->getName() == "struct") {
      if (hasMembers(tag)) {
	println(genToc(resolveFileName(tag)+"Members.html", name+" (Miembros de)", true));

	// addTocForMembers(tag, "ctor", "Constructors");
	// addTocForMembers(tag, "ctor", "Destructors");
	addTocForMembers(tag, "field",    "Fields");
	addTocForMembers(tag, "method",   "Methods");
	addTocForMembers(tag, "operator", "Operators");
	addTocForMembers(tag, "event",    "Events");
	addTocForMembers(tag, "signal",   "Signals");
      }
    }
    else {
      // recurse through children
      for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	addToc(*it);
    }
      
    println("</UL>");
  }

  void addTocForMembers(XmlNode *tag,
			const String &memberType,
			const String &fileNameSuffix)
  {
    list<XmlNode *> children = tag->getChildren();
    list<XmlNode *>::iterator it;

    // hasn't "memberType" nodes
    if (tag->getChild(memberType) == NULL)
      return;

    String summaryFile = resolveFileName(tag)+fileNameSuffix+".html";
    println(genToc(summaryFile, getPluralOf(memberType), false));
    
    println("<UL>");

    for (it = children.begin();
	 it != children.end(); ++it) {
      if ((*it)->getName() == memberType) {
	String memberFileName = resolveFileName(*it)+".html";
	println(genToc(memberFileName, preProcessTitle(*it), true));
      }
    }

    println("</UL>");
  }

  String genToc(const String &fileName, const String &title, bool leaf)
  {
    NewTocFile(fileName);

    // return the new TOC text
    return
      "<LI><OBJECT type=\"text/sitemap\">"
      "<param name=\"Name\" value=\""+title+"\" />"+
      "<param name=\"Local\" value=\""+fileName+"\" />"+
      (leaf ? String("<param name=\"ImageNumber\" value=\"11\" />"): "")+
      "</OBJECT></LI>";
  }

};

class VacaDocGenerator : private XmlParser
{
  list<String> mProjectFiles;   // lines of text
  XmlNode *mRoot;
  String mRootDirectory;
  String mDocName;
  String docLang;
  String mDocTitle;
  String mDocFeedback;
  String docCopyright;
  String docCopyrightAddress;
  list<XmlNode *> mCrossRef;	// list of cross-references used in the current page
  list<XmlNode *> mSeeAlso;	// list of "See Also" nodes for the current page

public:

  VacaDocGenerator(XmlStream *stream)
    : XmlParser(stream)
  {
    mRootDirectory = stream->getName().getFilePath().addPathComponent("doc");
  }
  
  void generate()
  {
    mRoot = parseRoot();
    mDocName = getNameAttributeOfNode(mRoot);

    if (mDocName.empty()) {
      throw Exception("'" + mRoot->getName() + "' tag must has a 'name' attribute");
    }

    docLang = mRoot->getAttributeValue("lang");
    mDocTitle = mRoot->getAttributeValue("title");
    mDocFeedback = mRoot->getAttributeValue("feedback");
    docCopyright = mRoot->getAttributeValue("copyright");
    docCopyrightAddress = mRoot->getAttributeValue("copyright_address");

    // the first tag is the default topic
    XmlNode *node = mRoot->getChildren().front();
    String defaultTopic = resolveFileName(node)+".html";

    // Index
    {
      File file(mRootDirectory.addPathComponent(mDocName)+".hhk");
      file.println("<HTML><BODY></BODY></HTML>");
    }

    // TOC
    {
      TocGenerator file(mRootDirectory.addPathComponent(mDocName)+".hhc");
      file.NewTocFile.connect(boost::bind(&std::list<String>::push_back,
					  &mProjectFiles, boost::arg<1>()));
      file.generate(mRoot);

      // a new file for the project
//       mProjectFiles.push_back(fileName);

    }

    // TOC (html)
//     {
//       File file("doc/contents.hhc");
//       addTocHtml(mRoot, file);
//     }

    // HHP (Project file to create CHM using hhc.exe utility)
    {
      File file(mRootDirectory.addPathComponent(mDocName)+".hhp");
      
      file.println("[FILES]");
      file.println("MSDN.css");

      for (list<String>::iterator it = mProjectFiles.begin(); it != mProjectFiles.end(); ++it)
	file.println(*it);

      file.println("");
      file.println("[OPTIONS]");
      file.println("Title="+mDocName);
      file.println("Auto Index=Yes");
      file.println("Compatibility=1.1 or later");
      file.println("Compiled file="+mDocName+".chm");
      file.println("Default Window=MsdnHelp");
      file.println("Default topic="+defaultTopic);
      file.println("Display compile progress=No");
      file.println("Error log file="+mDocName+".log");
      file.println("Full-text search=Yes");
      file.println("Index file="+mDocName+".hhk");
      file.println("Language=0x409 Inglés (Estados Unidos)");
      file.println("Contents file="+mDocName+".hhc");
      file.println("");
      //file.println("[WINDOWS]");
      //file.println("MsdnHelp=\"Vaca Help\",\"Vaca.hhc\",\"Vaca.hhk\",\"Vaca.html\",\"Vaca.html\",,,,,0x62520,220,0x387e,[86,51,872,558],,,,,,,0");
      //file.println("");
      //file.println("[INFOTYPES]");
    }

    // Create all .html files
    String mainHtml;
    {
      String fileHtml;
      // stack of parents tag
      stack<XmlNode *> nodeStack;
      nodeStack.push(mRoot);

      while (!nodeStack.empty()) {
	node = nodeStack.top();
	nodeStack.pop();
	
	list<XmlNode *> children = node->getChildren();

	if (node->getName() == "class" ||
	    node->getName() == "struct") {
	  vector<XmlNode *> members = getClassMembers(node);

	  sort(members.begin(), members.end(), compNameAttributeOfNodes());

	  createClassFile(fileHtml = resolveFileName(node)+".html", node);
	  createMembersFile(resolveFileName(node)+"Members.html", node, members);
// 	  createMembersOfTypeFile(resolveFileName(node)+"Constructors.html", node, members, "ctor");
// 	  createMembersOfTypeFile(resolveFileName(node)+"Destructors.html",  node, members, "dtor");
	  createMembersOfTypeFile(resolveFileName(node)+"Fields.html",       node, members, "field");
	  createMembersOfTypeFile(resolveFileName(node)+"Methods.html",      node, members ,"method");
	  createMembersOfTypeFile(resolveFileName(node)+"Operators.html",    node, members, "operator");
	  createMembersOfTypeFile(resolveFileName(node)+"Events.html",       node, members, "event");
	  createMembersOfTypeFile(resolveFileName(node)+"Signals.html",      node, members, "signal");
	  
	  for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	    createMemberFile(resolveFileName(*it)+".html", node, *it);
	}
	else if (node->getName() == "enum" ||
		 node->getName() == "typedef" ||
		 node->getName() == "macro" ||
		 node->getName() == "function") {
	  createAtomTypeFile(fileHtml = resolveFileName(node)+".html", node);
	}
	else if (node->getName() == "chapter" ||
		 node->getName() == "section" ||
		 node->getName() == "namespace") {
	  createChapterFile(fileHtml = resolveFileName(node)+".html", node);
	  
	  for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	    nodeStack.push(*it);
	}
	else {
	  for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	    nodeStack.push(*it);
	}

	if (mainHtml.empty())
	  mainHtml = fileHtml;
      }
    }

    // index.html
    {
      File file(mRootDirectory.addPathComponent("index.html"));
      file.println("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Frameset//EN\">");
      file.println("<html>");
      file.println("	<head>");
      file.println("		<meta name=\"Robots\" content=\"noindex\">");
      file.println("		<title>"+mDocTitle+"</title>");
      file.println("		<script language=\"JavaScript\">");
      file.println("		// ensure this page is not loaded inside another frame");
      file.println("		if (top.location != self.location)");
      file.println("		{");
      file.println("			top.location = self.location;");
      file.println("		}");
      file.println("		</script>");
      file.println("	</head>");
      file.println("	<frameset cols=\"250,*\" framespacing=\"6\" bordercolor=\"#6699CC\">");
      file.println("		<frame name=\"contents\" src=\"contents.html\" frameborder=\"0\" scrolling=\"no\">");
      file.println("		<frame name=\"main\" src=\""+mainHtml+".html\" frameborder=\"1\">");
      file.println("		<noframes>");
      file.println("			<p>This page requires frames, but your browser does not support them.</p>");
      file.println("		</noframes>");
      file.println("	</frameset>");
      file.println("</html>");
    }

    ShellExecute(NULL, "open", "hhc.exe", String(mDocName+".hhp").c_str(), "doc", 0);

    delete mRoot;
  }

private:

  void headerTemplate(File &file, const String &title)
  {
    file.println("<html dir=\"LTR\">");
    file.println("  <head>");
    file.println("    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=Windows-1252\" />");
    file.println("    <meta name=\"vs_targetSchema\" content=\"http://schemas.microsoft.com/intellisense/ie5\" />");
    file.println("    <title>"+title+"</title>");
    file.println("    <xml>");
    file.println("    </xml>");
    file.println("    <link rel=\"stylesheet\" type=\"text/css\" href=\"MSDN.css\" />");
    file.println("  </head>");
    file.println("  <body id=\"bodyID\" class=\"dtBODY\">");
    file.println("    <div id=\"nsbanner\">");
    file.println("      <div id=\"bannerrow1\">");
    file.println("        <table class=\"bannerparthead\" cellspacing=\"0\">");
    file.println("          <tr id=\"hdr\">");
    file.println("            <td class=\"runninghead\">"+mDocTitle+"</td>");
    file.println("            <td class=\"product\">");
    file.println("            </td>");
    file.println("          </tr>");
    file.println("        </table>");
    file.println("      </div>");
    file.println("      <div id=\"TitleRow\">");
    file.println("        <h1 class=\"dtH1\">"+title+"</h1>");
    file.println("      </div>");
    file.println("    </div>");
    file.println("    <div id=\"nstext\">");

    mCrossRef.clear();
    mSeeAlso.clear();
  }

  void footerTemplate(File &file, const String &fileName, XmlNode *node)
  {
    listNodesWithName(file, node, "section");
    listNodesWithName(file, node, "class");
    listNodesWithName(file, node, "struct");
    listNodesWithName(file, node, "enum");
    listNodesWithName(file, node, "typedef");
    listNodesWithName(file, node, "macro");
    listNodesWithName(file, node, "function");

    // more "See Also"? 
    list<XmlNode *> children = node->getChildren();
    for (list<XmlNode *>::iterator it=children.begin(); it!=children.end(); ++it)
      if ((*it)->getName() == "seealso")
	processSeeAlso(*it);

    // add the parent as "See Also"
    if (node->getParent() != NULL &&
	node->getParent() != mRoot) // parent != <doc> tag
      mSeeAlso.push_back(node->getParent());

    // See Also
    if (!mSeeAlso.empty()) {
      file.println("<h4 class=\"dtH4\">Ver también</h4>");
      file.println("<p>");

      for (list<XmlNode *>::iterator it=mSeeAlso.begin(); it!=mSeeAlso.end(); ++it) {
	file.println(String(it != mSeeAlso.begin() ? "| ": "")+
		     "<a href=\""+resolveFileName(*it)+".html\">"+
		     preProcessTitle(*it)+"</a>");
      }
      
      file.println("</p>");
    }

    // Footer
    file.println("      <hr />");
    file.println("      <div id=\"footer\">");
    file.println("        <p>");
    file.println("          <a href=\"mailto:"+
		 mDocFeedback+"?subject="+
		 mDocName+"%20Documentation%20Feedback:%20"+fileName+"\">"
		 "Enviar comentarios sobre este tema.</a>");
    file.println("        </p>");
    file.println("        <p>");
    file.println("          <a href=\"http://vaca.sourceforge.net/index.php?section=license\">© 2005-2006, David A. Capello. Todos los derechos reservados.</a>");
    file.println("        </p>");
    file.println("        <p>");
    file.println("        </p>");
    file.println("      </div>");
    file.println("    </div>");
    file.println("  </body>");
    file.println("</html>");
  }

  // for "chapter", "section" and "namespace"
  void createChapterFile(const String &fileName, XmlNode *chapterNode)
  {
    String chapterTitle = chapterNode->getAttributeValue("title");

    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, chapterTitle);
    mCrossRef.push_back(chapterNode);

    // Content
    XmlNode *content = chapterNode->getChild("content");
    if (content != NULL)
      file.println("<p>"+getFormattedNodeText(content)+"</p>");

    footerTemplate(file, fileName, chapterNode);
  }

  // for "enum", "typedef", "macro" and "function"
  void createAtomTypeFile(const String &fileName, XmlNode *node)
  {
    String typeName = node->getAttributeValue("name");
    String title = preProcessTitle(node);
    String syntax = preProcessSyntax(node);

    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, title);
    mCrossRef.push_back(node);

    // Summary
    XmlNode *summary = node->getChild("summary");
    if (summary != NULL)
      file.println("<p>"+getFormattedNodeText(summary)+"</p>");

    // Syntax
    if (!syntax.empty())
      file.println("<div class=\"syntax\">"+syntax+"</div>");

    // Remarks
    XmlNode *remarks = node->getChild("remarks");
    if (remarks != NULL) {
      file.println("<h4 class=\"dtH4\">Comentarios</h4>");
      file.println("<p>"+getFormattedNodeText(remarks)+"</p>");
    }

    // Example
    XmlNode *example = node->getChild("example");
    if (example != NULL) {
      file.println("<h4 class=\"dtH4\">Ejemplo</h4>");
      file.println("<p>"+getFormattedNodeText(example)+"</p>");
    }
    
    footerTemplate(file, fileName, node);
  }
  
  // for "class" and "struct"
  void createClassFile(const String &fileName, XmlNode *classNode)
  {
    String className = classNode->getAttributeValue("name");
    String classTitle = preProcessTitle(classNode);
    String syntaxText = preProcessSyntax(classNode);

    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, classTitle);
    mCrossRef.push_back(classNode);

    // Summary
    XmlNode *summary = classNode->getChild("summary");
    if (summary != NULL)
      file.println("<p>"+getFormattedNodeText(summary)+"</p>");

    // Members of
    if (hasMembers(classNode))
      file.println("<p>Para obtener una lista de todos los miembros de este tipo, vea "
		   "<a href=\""+resolveFileName(classNode)+"Members.html\">" + className + " (Miembros de)</a>.</p>");

    // Hierarchy
    printHierarchy(file, classNode);

    // Syntax
    if (!syntaxText.empty())
      file.println("<div class=\"syntax\">"+syntaxText+"</div>");

    // Remarks
    XmlNode *remarks = classNode->getChild("remarks");
    if (remarks != NULL) {
      file.println("<h4 class=\"dtH4\">Comentarios</h4>");
      file.println("<p>"+getFormattedNodeText(remarks)+"</p>");
    }

    // Example
    XmlNode *example = classNode->getChild("example");
    if (example != NULL) {
      file.println("<h4 class=\"dtH4\">Ejemplo</h4>");
      file.println("<p>"+getFormattedNodeText(example)+"</p>");
    }
    
    footerTemplate(file, fileName, classNode);
  }

  // creates the "__ClassName__Members.html" file
  void createMembersFile(const String &fileName,
			 XmlNode *classNode,
			 vector<XmlNode *> &members)
  {
    if (!hasMembers(classNode))
      return;
    
    String className = classNode->getAttributeValue("name");
    String classTitle = className+" (Miembros de)";
    String syntaxText = preProcessSyntax(classNode);

    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, classTitle);

    // Go to overview
    file.println("<p><a href=\""+resolveFileName(classNode)+".html\">" + className + " (introducción)</a></p>");

    // Members

    listMembersOfTypeAndAccess(file, classNode, members, "ctor",     "public");
    listMembersOfTypeAndAccess(file, classNode, members, "field",    "public");
    listMembersOfTypeAndAccess(file, classNode, members, "method",   "public");
    listMembersOfTypeAndAccess(file, classNode, members, "operator", "public");
    listMembersOfTypeAndAccess(file, classNode, members, "event",    "public");
    listMembersOfTypeAndAccess(file, classNode, members, "signal",   "public");

    listMembersOfTypeAndAccess(file, classNode, members, "ctor",     "protected");
    listMembersOfTypeAndAccess(file, classNode, members, "field",    "protected");
    listMembersOfTypeAndAccess(file, classNode, members, "method",   "protected");
    listMembersOfTypeAndAccess(file, classNode, members, "operator", "protected");
    listMembersOfTypeAndAccess(file, classNode, members, "event",    "protected");
    listMembersOfTypeAndAccess(file, classNode, members, "signal",   "protected");

    listMembersOfTypeAndAccess(file, classNode, members, "ctor",     "private");
    listMembersOfTypeAndAccess(file, classNode, members, "field",    "private");
    listMembersOfTypeAndAccess(file, classNode, members, "method",   "private");
    listMembersOfTypeAndAccess(file, classNode, members, "operator", "private");
    listMembersOfTypeAndAccess(file, classNode, members, "event",    "private");
    listMembersOfTypeAndAccess(file, classNode, members, "signal",   "private");
    
    footerTemplate(file, fileName, classNode);
  }

  // creates file like "__ClassName__Fields.html", "__ClassName__Methods.html", etc.
  void createMembersOfTypeFile(const String &fileName,
			       XmlNode *classNode,
			       vector<XmlNode *> &members,
			       const String &memberType)
  {
    vector<XmlNode *>::iterator it;
    for (it=members.begin(); it!=members.end(); ++it) {
      if ((*it)->getName() == memberType)
	break;
    }
    // hasn't "memberType" nodes?
    if (it == members.end())
      return;

    String className = classNode->getAttributeValue("name");
    String title = className + " (" + getPluralOf(memberType) + " de)";
    String syntaxText = preProcessSyntax(classNode);
      
    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, title);
  
    // Go to "Members of"
    file.println("<p>A continuación se enumeran "+naturalPhraseForMemberType(memberType)+
		 " de la clase "+className+". Para obtener "
		 "una lista completa de los miembros de la clase "+className+", vea el tema "+
		 "<a href=\""+resolveFileName(classNode)+"Members.html\">" + className + " (Miembros de)</a>"+".</p>");

    // Members of type

    listMembersOfTypeAndAccess(file, classNode, members, memberType, "public");
    listMembersOfTypeAndAccess(file, classNode, members, memberType, "protected");
    listMembersOfTypeAndAccess(file, classNode, members, memberType, "private");

    footerTemplate(file, fileName, classNode);
  }

  // prints the table of members of the specified type and access
  void listMembersOfTypeAndAccess(File &file,
				  XmlNode *classNode,
				  vector<XmlNode *> &members,
				  const String &memberType,
				  const String &memberAccess)
  {
    String className = classNode->getAttributeValue("name");
    String title = getMembersOfTypeTitle(memberType, memberAccess);
    String imgFile = getImageFileName(memberType, memberAccess);

    vector<XmlNode *>::iterator it;
    bool first = true;

    for (it = members.begin(); it != members.end(); ++it) {
      if (// is the type of member
	  (*it)->getName() == memberType &&
	  // has the access?
	  strncmp((*it)->getAttributeValue("syntax").c_str(),
		  memberAccess.c_str(),
		  memberAccess.size()) == 0) {

	if (first) {
	  first = false;
	  file.println("<h4 class=\"dtH4\">"+title+"</h4>");
	  file.println("<div class=\"tablediv\"><table class=\"dtTABLE\" cellspacing=\"0\">");
	}

	file.println("<tr VALIGN=\"top\"><td width=\"50%\">");

	// access/memberType icon
	if (!imgFile.empty())
	  file.println("<img src=\""+imgFile+"\"></img>");

	// static icon
	if ((*it)->getAttributeValue("syntax").find(" static ") != String::npos) {
	  file.println("<img src=\"static.gif\"></img>");
	}

	// member name (and link)
	file.println("<a href=\""+resolveFileName(*it)+".html\">"+
		     getNameAttributeOfNode(*it)+"</a>");

	// inherited
	XmlAttribute *__origClass = (*it)->getAttribute("__origClass");
	if ((*it)->getAttribute("__origClass") != NULL) {
	  String origClassName = __origClass->getValue();
	  if (origClassName != className)
	    file.println("(se hereda de <b>"+origClassName+"</b>)");
	}

	// separator
	file.println("</td><td width=\"50%\">");

	// replaced
	XmlAttribute *__replaced = (*it)->getAttribute("__replaced");
	if ((*it)->getAttribute("__replaced") != NULL)
	  file.println("Reemplazado.");
	
	// summary
	file.println((*it)->getChild("summary") != NULL ? getFormattedNodeText((*it)->getChild("summary")): "");
	file.println("</td></tr>");
      }
    }

    if (!first) {
      file.println("</table></div>");
    }
  }

  // prints the table of subnodes with the specified name
  void listNodesWithName(File &file,
			 XmlNode *node,
			 const String &name)
  {
    list<XmlNode *> children = node->getChildren();
    list<XmlNode *>::iterator it;
    bool first = true;

    for (it = children.begin(); it != children.end(); ++it) {
      if ((*it)->getName() == name) {
	if (first) {
	  first = false;
	  file.println("<h3 class=\"dtH3\">"+getPluralOf(name)+"</h3>");
	  file.println("<div class=\"tablediv\"><table class=\"dtTABLE\" cellspacing=\"0\">");

	  // heading
	  file.println("<tr VALIGN=\"top\"><th width=\"50%\">");
	  file.println(getSingularOf(name));
	  file.println("</th><th width=\"50%\">");
	  file.println("Descripción");
	  file.println("</th></tr>");
	}

	// new row
	file.println("<tr VALIGN=\"top\"><td>");
	
	// subnode name (and link)
	XmlAttribute *titleAttrib = (*it)->getAttribute("title");
	String title;
	if (titleAttrib == NULL)
	  title = getNameAttributeOfNode(*it);
	else
	  title = titleAttrib->getValue();
	
	file.println("<a href=\""+resolveFileName(*it)+".html\">"+title+"</a>");

	// separator
	file.println("</td><td>");

	// summary
	file.println((*it)->getChild("summary") != NULL ? getFormattedNodeText((*it)->getChild("summary")): "");
	file.println("</td></tr>");
      }
    }

    if (!first) {
      file.println("</table></div>");
    }
  }

  void createMemberFile(const String &fileName,
			XmlNode *classNode,
			XmlNode *memberNode)
  {
    String className = classNode->getAttributeValue("name");
    String memberName = memberNode->getAttributeValue("name");
    String memberTitle = preProcessTitle(memberNode);
    String syntaxText = preProcessSyntax(memberNode);

    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, className+"::"+memberTitle);
    mCrossRef.push_back(memberNode);

    // Summary
    XmlNode *summary = memberNode->getChild("summary");
    if (summary != NULL)
      file.println("<p>"+getFormattedNodeText(summary)+"</p>");

    // Syntax
    if (!syntaxText.empty())
      file.println("<div class=\"syntax\">"+syntaxText+"</div>");

    // Parameters
    if (memberNode->getChild("param")) {
      file.println("<h4 class=\"dtH4\">Parámetros</h4>");
      file.println("<dl>");

      list<XmlNode *> children = memberNode->getChildren();
      for (list<XmlNode *>::iterator it=children.begin(); it!=children.end(); ++it) {
	if ((*it)->getName() == "param") {
	  file.println("<dt>");
	  file.println("<i>"+(*it)->getAttributeValue("name")+"</i>");
	  file.println("</dt>");
	  file.println("<dd>"+getFormattedNodeText(*it)+"</dd>");
	}
      }
      
      file.println("</dl>");
    }

    // Remarks
    XmlNode *remarks = memberNode->getChild("remarks");
    if (remarks != NULL) {
      file.println("<h4 class=\"dtH4\">Comentarios</h4>");
      file.println("<p>"+getFormattedNodeText(remarks)+"</p>");
    }

    // Example
    XmlNode *example = memberNode->getChild("example");
    if (example != NULL) {
      file.println("<h4 class=\"dtH4\">Ejemplo</h4>");
      file.println("<p>"+getFormattedNodeText(example)+"</p>");
    }
    
//     file.println("      <object type=\"application/x-oleobject\" classid=\"clsid:1e2a7bd0-dab9-11d0-b93a-00c04fc99f9e\" viewastext=\"true\" style=\"display: none;\">");
//     file.println("        <param name=\"Keyword\" value=\"setDefault method\">");
//     file.println("        </param>");
//     file.println("        <param name=\"Keyword\" value=\"setDefault method, Button class\">");
//     file.println("        </param>");
//     file.println("        <param name=\"Keyword\" value=\"Button.setDefault method\">");
//     file.println("        </param>");
//     file.println("      </object>");

    footerTemplate(file, fileName, memberNode);
  }

  String naturalPhraseForMemberType(const String &memberType)
  {
    if (memberType == "ctor")
      return "los constructores";
    else if (memberType == "field")
      return "los campos";
    else if (memberType == "method")
      return "los métodos";
    else if (memberType == "operator")
      return "los operadores";
    else if (memberType == "event")
      return "los eventos";
    else if (memberType == "signal")
      return "las señales";
    
    return "";
  }

  String getMembersOfTypeTitle(const String &memberType, const String &memberAccess)
  {
    String prefix, suffix;
      
    if (memberAccess == "public")
      suffix = (memberType == "signal") ? "públicas": "públicos";
    else if (memberAccess == "protected")
      suffix = (memberType == "signal") ? "protegidas": "protegidos";
    else if (memberAccess == "private")
      suffix = (memberType == "signal") ? "privadas": "privados";

//     if (memberType == "ctor")
//        prefix = "Constructores";
//     else if (memberType == "field")
//        prefix = "Campos";
//     else if (memberType == "method")
//        prefix = "Métodos";
//     else if (memberType == "operator")
//        prefix = "Operadores";
//     else if (memberType == "event")
//        prefix = "Eventos";
//     else if (memberType == "signal")
//        prefix = "Señales";
    
    return getPluralOf(memberType) + " " + suffix;
  }

  String getImageFileName(String memberType, const String &memberAccess)
  {
    String prefix, suffix = ".gif";

    if (memberAccess == "public")
      prefix = "pub";
    else if (memberAccess == "protected")
      prefix = "prot";
    else if (memberAccess == "private")
      prefix = "priv";

    if (memberType == "ctor")
      memberType = "method";
    else if (memberType == "signal")
      memberType = "event";

    return prefix + memberType + suffix;
  }

  void printHierarchy(File &file, XmlNode *classNode)
  {
    file.println("<p>");

    ////////////////////////////////////////
    // print base classes
    
    stack<XmlNode *> baseStack;

    XmlNode *base = getBaseClass(classNode);
    while (base != NULL) {
      baseStack.push(base);
      base = getBaseClass(base);
    }

    int indent = 0;

    while (!baseStack.empty()) {
      base = baseStack.top();
      baseStack.pop();

      file.println("<a href=\""+resolveFileName(base)+".html\">"+
		   getNameAttributeOfNode(base)+"</a><br />");

      ++indent;
      for (int c=0; c<indent; ++c)
	file.println("  ");
    }

    ////////////////////////////////////////
    // print classNode

    file.println("<b>"+getNameAttributeOfNode(classNode)+"</b>");
    indent++;

    ////////////////////////////////////////
    // now print sub-classes

    vector<XmlNode *> subclasses = getSubClasses(classNode, mRoot);

    for (vector<XmlNode *>::iterator it = subclasses.begin(); it != subclasses.end(); ++it) {
      file.println("<br />");
      for (int c=0; c<indent; ++c)
	file.println("  ");
      file.println("<a href=\""+resolveFileName(*it)+".html\">"+
		   getNameAttributeOfNode(*it)+"</a>");
    }

    file.println("</p>");
  }

  void processSeeAlso(XmlNode *node)
  {
    String cref = node->getAttributeValue("cref");
    XmlNode *ref = resolveCrossRef(node, cref);
    if (ref != NULL)
      mSeeAlso.push_back(ref);
    else {
      printf("<seealso cref=\"%s\" /> fail\n", cref.c_str());
    }
  }

  String getFormattedNodeText(XmlNode *contentNode)
  {
    if (contentNode == NULL)
      return "";

    XmlNode *parentNode = contentNode->getParent();
    std::list<XmlType *> typeString = contentNode->getTypeString();
    String res;

    for (std::list<XmlType *>::iterator it=typeString.begin();
	 it!=typeString.end(); ++it) {
      // XmlString
      if ((*it)->isString()) {
	res += (*it)->getText();
      }
      // XmlNode
      else if ((*it)->isNode()) {
	XmlNode *node = (*it)->getNode();

	// <em></em>
	if (node->getName() == "em") {
	  res += "<em>" + node->getText() + "</em>";
	}
	// <b></b>
	else if (node->getName() == "b") {
	  res += "<b>" + node->getText() + "</b>";
	}
	// <c></c>
	else if (node->getName() == "c") {
	  res += "<code>" + node->getText() + "</code>";
	}
	// <code></code>
	else if (node->getName() == "code") {
	  res += "<pre class=\"code\">" + node->getText() + "</pre>";
	}
	// <para></para>
	else if (node->getName() == "para") {
	  res += "<p>" + getFormattedNodeText(node) + "<p>";
	}
	// <see/>
	else if (node->getName() == "see") {
	  String cref = node->getAttributeValue("cref");
	  XmlNode *ref = resolveCrossRef(node, cref);

	  if (ref != NULL &&
	      ref != parentNode &&
	      find(mCrossRef.begin(), mCrossRef.end(), ref) == mCrossRef.end()) {
	    res += "<a href=\"" + resolveFileName(ref) + ".html\">" + cref + "</a>";
	    mCrossRef.push_back(ref);
	  }
	  else
	    res += "<b>" + cref + "</b>";
	}
	// <seealso/>
	else if (node->getName() == "seealso") {
	  // TODO this should be an error!!!
	  processSeeAlso(node);
	}
	// <list></list>
	else if (node->getName() == "list") {
	  String type = node->getAttributeValue("type");
	  list<XmlNode *> children = node->getChildren();
	  XmlNode *item, *desc;

	  // bullet & number
	  if (type == "bullet" || type == "number") {
	    if (type == "bullet")
	      res += "<ul type=\"disc\">";
	    else
	      res += "<ol>";
	    
	    for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it) {
	      if ((*it)->getName() == "item") {
		item = (*it)->getChild("term");
		desc = (*it)->getChild("description");

		res += "<li><b>" +
		  getFormattedNodeText(item) + " - </b>" +
		  getFormattedNodeText(desc) + "</li>";
	      }
	    }
	    if (type == "bullet")
	      res += "</ul>";
	    else
	      res += "</ol>";
	  }
	  // table
	  else if (type == "table") {
	    res += "<div class=\"tablediv\"><table class=\"dtTABLE\" cellspacing=\"0\">";
	    
	    for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it) {
	      item = (*it)->getChild("term");
	      desc = (*it)->getChild("description");

	      if ((*it)->getName() == "listheader")
		res +=
		  "<tr valign=\"top\">"
		  "<th width=\"50%\">" + getFormattedNodeText(item) + "</th>"
		  "<th width=\"50%\">" + getFormattedNodeText(desc) + "</th></tr>";
	      else if ((*it)->getName() == "item")
		res +=
		  "<tr valign=\"top\">"
		  "<td>" + getFormattedNodeText(item) + "</td>"
		  "<td>" + getFormattedNodeText(desc) + "</td></tr>";
	    }
	    res += "</table></div>";
	  }
	}
	// unknown tag
	else {
	  throw Exception("Invalid tag '" + node->getName() + "'.");
	}
      }
    }
    
    return res;
  }

};

//////////////////////////////////////////////////////////////////////
//

class XmlFileStream : public XmlStream
{
  FILE *mFile;
  String mFileName;
  
public:

  XmlFileStream(const String &fileName)
  {
    mFile = fopen(fileName.c_str(), "rb");
    if (mFile == NULL)
      throw Exception("Error loading file");
  }
  
  virtual ~XmlFileStream()
  {
    fclose(mFile);
  }

  virtual Vaca::String getName()
  {
    return mFileName;
  }
  
  virtual bool readLine(char *buf, int max)
  {
    return fgets(buf, max, mFile);
  }
  
};

//////////////////////////////////////////////////////////////////////

class VacaDoc : public Application
{
public:

  virtual void main(std::vector<String> args)
  {
    if (args.size() < 2) {
      System::println("Usage:");
      System::println("VacaDoc.exe DocFile.xml");
    }
    else {
      try {
	XmlFileStream stream(System::getCurrentDirectory().addPathComponent(args[1]));
	VacaDocGenerator parser(&stream);
	parser.generate();
      }
      catch (Exception &exception) {
	printf("Exception: %s\n", exception.getMessage().c_str());
      }
    }
  }
  
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  printf("start\n");

  VacaDoc *app(new VacaDoc);
  app->run();
  delete app;
  
  printf("end\n");
  return 0;
}
