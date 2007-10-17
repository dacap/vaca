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

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <algorithm>
#include <boost/signal.hpp>

#include "Vaca/Application.h"
#include "Vaca/String.h"
#include "Vaca/Exception.h"
#include "Vaca/System.h"
#include "Vaca/Debug.h"
#include "Vaca/Bind.h"

#include "XmlParser.h"
#include "XmlNode.h"
#include "XmlAttribute.h"
#include "XmlStream.h"
#include "CppNode.h"

using namespace std;
using namespace Vaca;

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

Namespace *RootNamespace;

namespace NodeEx
{
  
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

  String resolveFileName(XmlNode *node)
  {
    if (node->getParent() == NULL)
      return "cover";
      
    String fileName;

    if (node->getName() == "chapter" ||
	node->getName() == "section")
      fileName += node->getName() + ".";

    // stack of parents node
    stack<XmlNode *> parentsStack;

    XmlNode *parent = node->getParent();
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

    if (node->getName() == "operator") {
      String op = getNameAttributeOfNode(node);
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
    else if (node->getName() == "ctor") {
      fileName += "Constructor";
    }
    else if (node->getName() == "dtor") {
      fileName += "Destructor";
    }
    else
      fileName += getNameAttributeOfNode(node);
    
    return fileName;
  }

  XmlNode *resolveCrossRef(XmlNode *node, const String &name)
  {
    while (node->getName() != "doc" &&
	   node->getName() != "namespace" &&
	   node->getName() != "class" &&
	   node->getName() != "struct")
      node = node->getParent();

    // reference in namespaces
    CppNode *cppNode = RootNamespace->findByXmlNode(node);
    if (cppNode != NULL) {
      CppNode *ref = cppNode->resolveCrossRef(name);
      if (ref != NULL)
	return ref->xmlNode;
    }

    // documentation reference
    // TODO
    
    return NULL;
  }

  String preProcessTitle(XmlNode *node)
  {
    String name = getNameAttributeOfNode(node);
    if (node->getName() == "namespace")     return name + " (Espacio de nombres)";
    else if (node->getName() == "class")    return name + " (Clase)";
    else if (node->getName() == "struct")   return name + " (Estructura)";
    else if (node->getName() == "enum")     return name + " (Enumeración)";
    else if (node->getName() == "typedef")  return name + " (Tipo de dato)";
    else if (node->getName() == "macro")    return name + " (Macro)";
    else if (node->getName() == "function") return name + " (Función)";
    else if (node->getName() == "ctor")     return name + " (Constructor)";
    else if (node->getName() == "dtor")     return name + " (Destructor)";
    else if (node->getName() == "field")    return name + " (Campo)";
    else if (node->getName() == "method")   return name + " (Método)";
    else if (node->getName() == "operator") return name + " (Operador)";
    else if (node->getName() == "event")    return name + " (Evento)";
    else if (node->getName() == "signal")   return name + " (Señal)";
    return node->getAttributeValue("title");
  }

  String getSectionTitle(XmlNode *node)
  {
    String title;
    XmlAttribute *titleAttrib = node->getAttribute("title");

    if (titleAttrib == NULL)
      title = preProcessTitle(node);
    else
      title = titleAttrib->getValue();

    return title;
  }

  String preProcessSyntax(XmlNode *node)
  {
    String syntax = node->getAttributeValue("syntax");
    String::const_iterator it = syntax.begin();
    String::const_iterator end = syntax.end();
    String res;

    for (; it != end && *it != '\0'; ++it) {
      if (*it == '{' && (*(it+1)) != '}') {
	res += "{<br />  ";
      }
      else if (*it == '}' && (*(it-1)) != '{') {
	res += "<br />}";
      }
      else if (*it == '(' && (*(it+1)) != ')') {
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

} // NodeEx namespace

using namespace NodeEx;

//////////////////////////////////////////////////////////////////////
// TOC generator

struct compNameOfXmlNode {

  bool operator()(XmlNode *node1, XmlNode *node2) const
  {
    return (node1->getAttributeValue("name") <
	    node2->getAttributeValue("name"));
  }
  
};

class TocGenerator : public File
{
public:

  TocGenerator(const String &fileName)
    : File(fileName)
  {
  }

  virtual void generate(XmlNode *root) = 0;

  boost::signal<void (const String &fileName)> NewTocFile;

protected:
  
  void addTocsInNode(XmlNode *xmlNode)
  {
    list<XmlNode *> children = xmlNode->getChildren();
    String name = getNameAttributeOfNode(xmlNode);
    Class *cppClass = NULL;

    if (xmlNode->getName() == "class" ||
	xmlNode->getName() == "struct") {
      cppClass = dynamic_cast<Class *>(RootNamespace->findByXmlNode(xmlNode));
      if (cppClass == NULL)
	throw Exception("Internal exception (class in XML tree but no in CPP tree)");
    }
    else
      cppClass = NULL;

    if (xmlNode->getName() == "doc" ||
	xmlNode->getName() == "chapter" ||
	xmlNode->getName() == "section" ||
	xmlNode->getName() == "namespace" ||
	xmlNode->getName() == "class" ||
	xmlNode->getName() == "struct" ||
	xmlNode->getName() == "enum" ||
	xmlNode->getName() == "typedef" ||
	xmlNode->getName() == "macro" ||
	xmlNode->getName() == "function") {
      // section name is mandatory
      if (name.empty())
	throw Exception("Tag '"+xmlNode->getName()+"' without 'name' attribute.");

      // title of the section
      String title = getSectionTitle(xmlNode);

      // is a leaf?
      bool leaf = children.empty();

      if (xmlNode->getName() == "chapter" ||
	  xmlNode->getName() == "section" ||
	  xmlNode->getName() == "namespace") {
	if (!xmlNode->getChild("section") &&
	    !xmlNode->getChild("namespace") &&
	    !xmlNode->getChild("class") &&
	    !xmlNode->getChild("struct") &&
	    !xmlNode->getChild("enum") &&
	    !xmlNode->getChild("typedef") &&
	    !xmlNode->getChild("macro") &&
	    !xmlNode->getChild("function"))
	  leaf = true;
      }
      else if (xmlNode->getName() == "class" ||
	       xmlNode->getName() == "struct")
	leaf = cppClass->members.empty();
      else if (xmlNode->getName() == "enum" ||
	       xmlNode->getName() == "typedef" ||
	       xmlNode->getName() == "macro" ||
	       xmlNode->getName() == "function")
	leaf = true;

      // add the TOC entry
      addToc(resolveFileName(xmlNode)+".html", title, leaf);

      if (!leaf) {
	openSubnodes();

	// "class" is special
	if (xmlNode->getName() == "class" ||
	    xmlNode->getName() == "struct") {
	  // has members?
	  if (!cppClass->members.empty()) {
	    addToc(resolveFileName(xmlNode)+"Members.html", name+" (Miembros de)", true);

	    list<XmlNode *> childrenList = xmlNode->getChildren();
	    vector<XmlNode *> childrenVector;

	    // convert the list to a vector, so we can sort it
	    childrenVector.resize(childrenList.size());
	    copy(childrenList.begin(), childrenList.end(), childrenVector.begin());
	    sort(childrenVector.begin(), childrenVector.end(), compNameOfXmlNode());

	    addTocForMembers(xmlNode, "ctor",     "Constructors", childrenVector);
	    addTocForMembers(xmlNode, "dtor",     "Destructors",  childrenVector);
	    addTocForMembers(xmlNode, "field",    "Fields",       childrenVector);
	    addTocForMembers(xmlNode, "method",   "Methods",      childrenVector);
	    addTocForMembers(xmlNode, "operator", "Operators",    childrenVector);
	    addTocForMembers(xmlNode, "event",    "Events",       childrenVector);
	    addTocForMembers(xmlNode, "signal",   "Signals",      childrenVector);

	    closeToc();
	  }
	}
	else {
	  // recurse through children
	  for (list<XmlNode *>::iterator it = children.begin();
	       it != children.end();
	       ++it)
	    addTocsInNode(*it);
	}

	closeSubnodes();
      }
      
      closeToc();
    }
  }

  void addTocForMembers(XmlNode *node,
			const String &memberType,
			const String &fileNameSuffix,
			vector<XmlNode *> &children)
  {
    vector<XmlNode *>::iterator it;

    // hasn't "memberType" nodes
    if (node->getChild(memberType) == NULL)
      return;

    String summaryFile = resolveFileName(node)+fileNameSuffix+".html";
    addToc(summaryFile, getPluralOf(memberType), false);
    openSubnodes();

    for (it = children.begin();
	 it != children.end(); ++it) {
      if ((*it)->getName() == memberType) {
	String memberFileName = resolveFileName(*it)+".html";

	addToc(memberFileName, preProcessTitle(*it), true);
	closeToc();
      }
    }

    closeSubnodes();
    closeToc();
  }

  void addToc(const String &fileName, const String &title, bool leaf)
  {
    NewTocFile(fileName);
    openToc(fileName, title, leaf);
  }

  virtual void openToc(const String &fileName, const String &title, bool leaf) = 0;
  virtual void openSubnodes() = 0;
  virtual void closeSubnodes() = 0;
  virtual void closeToc() = 0;

};

//////////////////////////////////////////////////////////////////////
// NAME.hhc generator

class HhcTocGenerator : public TocGenerator
{
public:

  HhcTocGenerator(const String &fileName)
    : TocGenerator(fileName)
  {
  }

  virtual void generate(XmlNode *root)
  {
    println("<UL>");
    addTocsInNode(root);
    println("</UL>");
  }

protected:

  virtual void openToc(const String &fileName, const String &title, bool leaf)
  {
    println("<LI><OBJECT type=\"text/sitemap\">"
	    "<param name=\"Name\" value=\""+title+"\" />"+
	    "<param name=\"Local\" value=\""+fileName+"\" />"+
	    (leaf ? String("<param name=\"ImageNumber\" value=\"11\" />"): "")+
	    "</OBJECT></LI>");
  }

  virtual void openSubnodes()
  {
    println("<UL>");
  }
  
  virtual void closeSubnodes()
  {
    println("</UL>");
  }

  virtual void closeToc()
  {
    // do nothing
  }

};

//////////////////////////////////////////////////////////////////////
// contents.html generator

class HtmlTocGenerator : public TocGenerator
{
public:

  HtmlTocGenerator(const String &fileName)
    : TocGenerator(fileName)
  {
  }

  virtual void generate(XmlNode *root)
  {
    println("<html>");
    println("	<head>");
    println("		<title>Contents</title>");
    println("		<link rel=\"stylesheet\" type=\"text/css\" href=\"tree.css\" />");
    println("		<script src=\"tree.js\" language=\"javascript\" type=\"text/javascript\">");
    println("		</script>");
    println("	</head>");
    println("	<body id=\"docBody\" style=\"background-color: #6699CC; color: White; margin: 0px 0px 0px 0px;\" onload=\"resizeTree()\" onresize=\"resizeTree()\" onselectstart=\"return false;\">");
    println("		<div style=\"font-family: verdana; font-size: 8pt; cursor: pointer; margin: 6 4 8 2; text-align: right\" onmouseover=\"this.style.textDecoration='underline'\" onmouseout=\"this.style.textDecoration='none'\" onclick=\"syncTree(window.parent.frames[1].document.URL)\">sync toc</div>");
    println("		<div id=\"tree\" style=\"top: 35px; left: 0px;\" class=\"treeDiv\">");
    println("			<div id=\"treeRoot\" onselectstart=\"return false\" ondragstart=\"return false\">");

    addTocsInNode(root);

    println("			</div>");
    println("		</div>");
    println("	</body>");
    println("</html>");
  }

protected:

  virtual void openToc(const String &fileName, const String &title, bool leaf)
  {
    println("<div class=\"treeNode\">");

    if (leaf)
      println("<img src=\"treenodedot.gif\" class=\"treeNoLinkImage\" />");
    else
      println("<img src=\"treenodeplus.gif\" class=\"treeLinkImage\" onclick=\"expandCollapse(this.parentNode)\" />");

    println("<a href=\""+fileName+"\" target=\"main\" class=\"treeUnselected\" onclick=\"clickAnchor(this)\">"+title+"</a>");
  }

  virtual void openSubnodes()
  {
    println("<div class=\"treeSubnodesHidden\">");
  }
  
  virtual void closeSubnodes()
  {
    println("</div>");
  }

  virtual void closeToc()
  {
    println("</div>");
  }

};

//////////////////////////////////////////////////////////////////////

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

    // TODO
    // FileSystem::createDirectory(mRootDirectory);
    ::CreateDirectory(mRootDirectory.c_str(), NULL);

//     if (!::PathIsDirectory(mRootDirectory.c_str())) {
//       System::println("Directory "+mRootDirectory+" doesn't exist");
//       throw;
//     }
    
  }
  
  void generate()
  {
    mRoot = parseRoot();
    mDocName = getNameAttributeOfNode(mRoot);

    if (mDocName.empty()) {
      throw Exception("'" + mRoot->getName() + "' tag must has a 'name' attribute");
    }

    RootNamespace = new Namespace(mRoot, NULL);
    RootNamespace->resolveReferences();
    RootNamespace->sortVectors();

    docLang = mRoot->getAttributeValue("lang");
    mDocTitle = mRoot->getAttributeValue("title");
    mDocFeedback = mRoot->getAttributeValue("feedbackAddress");
    docCopyright = mRoot->getAttributeValue("copyright");
    docCopyrightAddress = mRoot->getAttributeValue("copyrightAddress");

    // the first tag is the default topic

    XmlNode *node;
    String defaultTopic = "cover.html";

    // Create all .html files
    {
      // stack of parents tag
      stack<XmlNode *> nodeStack;
      nodeStack.push(mRoot);

      while (!nodeStack.empty()) {
	node = nodeStack.top();
	nodeStack.pop();
	
	list<XmlNode *> children = node->getChildren();

	if (node->getName() == "class" ||
	    node->getName() == "struct") {
	  Class *cppClass = dynamic_cast<Class *>(RootNamespace->findByXmlNode(node));
	  if (cppClass != NULL) {
	    createClassFile(resolveFileName(node)+".html", cppClass);

	    if (!cppClass->members.empty()) {
	      createMembersFile(resolveFileName(node)+"Members.html", cppClass);
	      // createMembersOfTypeFile(resolveFileName(node)+"Constructors.html", node, *members, "ctor");
	      // createMembersOfTypeFile(resolveFileName(node)+"Destructors.html",  node, *members, "dtor");
	      createMembersOfTypeFile(resolveFileName(node)+"Fields.html", cppClass, "field");
	      createMembersOfTypeFile(resolveFileName(node)+"Methods.html", cppClass ,"method");
	      createMembersOfTypeFile(resolveFileName(node)+"Operators.html", cppClass, "operator");
	      createMembersOfTypeFile(resolveFileName(node)+"Events.html", cppClass, "event");
	      createMembersOfTypeFile(resolveFileName(node)+"Signals.html", cppClass, "signal");
	    }

	    for (vector<CppNode *>::iterator
		   it = cppClass->children.begin();
		 it != cppClass->children.end(); ++it) {
	      Member *member = dynamic_cast<Member *>(*it);
	      if (member != NULL)
		createMemberFile(resolveFileName(member->xmlNode)+".html",
				 cppClass, member);
	    }
	  }
	}
	else if (node->getName() == "enum" ||
		 node->getName() == "typedef" ||
		 node->getName() == "macro" ||
		 node->getName() == "function") {
	  createAtomTypeFile(resolveFileName(node)+".html", node);
	}
	else if (node->getName() == "doc" ||
		 node->getName() == "chapter" ||
		 node->getName() == "section" ||
		 node->getName() == "namespace") {
	  createChapterFile(resolveFileName(node)+".html", node);
	  
	  for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
	    nodeStack.push(*it);
	}
	else {
// 	  for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
// 	    nodeStack.push(*it);
	}
      }
    }

    // Index
    {
      File file(mRootDirectory.addPathComponent(mDocName)+".hhk");
      file.println("<HTML><BODY></BODY></HTML>");
    }

    // TOC
    {
      HhcTocGenerator file(mRootDirectory.addPathComponent(mDocName)+".hhc");
      file.NewTocFile.connect(Bind(&list<String>::push_back, &mProjectFiles));
      file.generate(mRoot);
    }

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

    // index.html
    {
      File file(mRootDirectory.addPathComponent("index.html"));
      file.println("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Frameset//EN\">");
      file.println("<html>");
      file.println("	<head>");
      file.println("		<meta name=\"Robots\" content=\"noindex\" />");
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
      file.println("		<frame name=\"main\" src=\""+defaultTopic+"\" frameborder=\"1\">");
      file.println("		<noframes>");
      file.println("			<p>This page requires frames, but your browser does not support them.</p>");
      file.println("		</noframes>");
      file.println("	</frameset>");
      file.println("</html>");
    }

    // contents.html
    {
      HtmlTocGenerator file(mRootDirectory.addPathComponent("contents.html"));
      file.generate(mRoot);
    }

    ShellExecute(NULL, "open", "hhc.exe", String(mDocName+".hhp").c_str(), "doc", 0);

    delete mRoot;
  }

protected:

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
    listNodesWithTitle(file, node);
    tableNodesWithName(file, node, "class");
    tableNodesWithName(file, node, "struct");
    tableNodesWithName(file, node, "enum");
    tableNodesWithName(file, node, "typedef");
    tableNodesWithName(file, node, "macro");
    tableNodesWithName(file, node, "function");

    // more "See Also"? 
    list<XmlNode *> children = node->getChildren();
    for (list<XmlNode *>::iterator it=children.begin(); it!=children.end(); ++it)
      if ((*it)->getName() == "seealso")
	processSeeAlso(*it);

    // add the parent as "See Also"
    if (node->getParent() != NULL)
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

    // index
    file.println("<object type=\"application/x-oleobject\" classid=\"clsid:1e2a7bd0-dab9-11d0-b93a-00c04fc99f9e\" viewastext=\"true\" style=\"display: none;\">");
    file.println("<param name=\"Keyword\" value=\""+preProcessTitle(node)+"\"></param>");
//     file.println("<param name=\"Keyword\" value=\"setDefault method, Button class\"></param>");
//     file.println("<param name=\"Keyword\" value=\"Button.setDefault method\"></param>");
    file.println("</object>");

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
  void createClassFile(const String &fileName, Class *cppClass)
  {
    String className = cppClass->name;
    String classTitle = preProcessTitle(cppClass->xmlNode);
    String syntaxText = preProcessSyntax(cppClass->xmlNode);

    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, classTitle);
    mCrossRef.push_back(cppClass->xmlNode);

    // Summary
    XmlNode *summary = cppClass->xmlNode->getChild("summary");
    if (summary != NULL)
      file.println("<p>"+getFormattedNodeText(summary)+"</p>");

    // Members of
    if (!cppClass->members.empty())
      file.println("<p>Para obtener una lista de todos los miembros de este tipo, vea "
		   "<a href=\""+resolveFileName(cppClass->xmlNode)+"Members.html\">"
		   + className + " (Miembros de)</a>.</p>");

    // Hierarchy
    printHierarchy(file, cppClass);

    // Syntax
    if (!syntaxText.empty())
      file.println("<div class=\"syntax\">"+syntaxText+"</div>");

    // Remarks
    XmlNode *remarks = cppClass->xmlNode->getChild("remarks");
    if (remarks != NULL) {
      file.println("<h4 class=\"dtH4\">Comentarios</h4>");
      file.println("<p>"+getFormattedNodeText(remarks)+"</p>");
    }

    // Example
    XmlNode *example = cppClass->xmlNode->getChild("example");
    if (example != NULL) {
      file.println("<h4 class=\"dtH4\">Ejemplo</h4>");
      file.println("<p>"+getFormattedNodeText(example)+"</p>");
    }
    
    footerTemplate(file, fileName, cppClass->xmlNode);
  }

  // creates the "__ClassName__Members.html" file
  void createMembersFile(const String &fileName, Class *cppClass)
  {
    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, cppClass->name+" (Miembros de)");

    // Go to overview
    file.println("<p><a href=\""+resolveFileName(cppClass->xmlNode)+".html\">" +
		 cppClass->name + " (introducción)</a></p>");

    // Members

    listMembersOfTypeAndAccess(file, cppClass, "ctor",     PublicAccess);
    listMembersOfTypeAndAccess(file, cppClass, "field",    PublicAccess);
    listMembersOfTypeAndAccess(file, cppClass, "method",   PublicAccess);
    listMembersOfTypeAndAccess(file, cppClass, "operator", PublicAccess);
    listMembersOfTypeAndAccess(file, cppClass, "event",    PublicAccess);
    listMembersOfTypeAndAccess(file, cppClass, "signal",   PublicAccess);

    listMembersOfTypeAndAccess(file, cppClass, "ctor",     ProtectedAccess);
    listMembersOfTypeAndAccess(file, cppClass, "field",    ProtectedAccess);
    listMembersOfTypeAndAccess(file, cppClass, "method",   ProtectedAccess);
    listMembersOfTypeAndAccess(file, cppClass, "operator", ProtectedAccess);
    listMembersOfTypeAndAccess(file, cppClass, "event",    ProtectedAccess);
    listMembersOfTypeAndAccess(file, cppClass, "signal",   ProtectedAccess);

    listMembersOfTypeAndAccess(file, cppClass, "ctor",     PrivateAccess);
    listMembersOfTypeAndAccess(file, cppClass, "field",    PrivateAccess);
    listMembersOfTypeAndAccess(file, cppClass, "method",   PrivateAccess);
    listMembersOfTypeAndAccess(file, cppClass, "operator", PrivateAccess);
    listMembersOfTypeAndAccess(file, cppClass, "event",    PrivateAccess);
    listMembersOfTypeAndAccess(file, cppClass, "signal",   PrivateAccess);
    
    footerTemplate(file, fileName, cppClass->xmlNode);
  }

  // creates file like "__ClassName__Fields.html", "__ClassName__Methods.html", etc.
  void createMembersOfTypeFile(const String &fileName,
			       Class *cppClass,
			       const String &memberType)
  {
    MembersIterator it;

    for (it = cppClass->members.begin();
	 it != cppClass->members.end(); ++it) {
      if ((*it)->getKind() == memberType)
	break;
    }
    
    // hasn't "memberType" nodes?
    if (it == cppClass->members.end())
      return;

    String className = cppClass->name;
    String title = className + " (" + getPluralOf(memberType) + " de)";
    String syntaxText = preProcessSyntax(cppClass->xmlNode);
      
    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, title);
  
    // Go to "Members of"
    file.println("<p>A continuación se enumeran "+naturalPhraseForMemberType(memberType)+
		 " de la clase "+className+". Para obtener "
		 "una lista completa de los miembros de la clase "+className+", vea el tema "+
		 "<a href=\""+resolveFileName(cppClass->xmlNode)+"Members.html\">" +
		 className + " (Miembros de)</a>"+".</p>");

    // Members of type

    listMembersOfTypeAndAccess(file, cppClass, memberType, PublicAccess);
    listMembersOfTypeAndAccess(file, cppClass, memberType, ProtectedAccess);
    listMembersOfTypeAndAccess(file, cppClass, memberType, PrivateAccess);

    footerTemplate(file, fileName, cppClass->xmlNode);
  }

  // prints the table of members of the specified type and access
  void listMembersOfTypeAndAccess(File &file,
				  Class *cppClass,
				  const String &memberType,
				  Access memberAccess)
  {
    String className = cppClass->name;
    String title = getMembersOfTypeTitle(memberType, memberAccess);
    String imgFile = getImageFileName(memberType, memberAccess);

    MembersIterator it;
    bool first = true;

    for (it = cppClass->members.begin();
	 it != cppClass->members.end(); ++it) {
      Member *member = *it;
      
      if (member->getKind() == memberType && // is the type of member
	  member->access == memberAccess) { // has the access?

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
	if (member->isStatic) {
	  file.println("<img src=\"static.gif\"></img>");
	}

	// member name (and link)
	file.println("<a href=\""+resolveFileName(member->xmlNode)+".html\">"+
		     member->name+"</a>");

	// inherited
	if (member->fromClass != NULL &&
	    member->fromClass != cppClass)
	  file.println("(se hereda de <b>"+member->fromClass->name+"</b>)");

	// separator
	file.println("</td><td width=\"50%\">");

	// replaced
	if (member->isReplaced && member->fromClass == cppClass)
	  file.println("Reemplazado.");
	
	// summary
	XmlNode *summary = member->xmlNode->getChild("summary");
	file.println(summary != NULL ? getFormattedNodeText(summary): "");
	file.println("</td></tr>");
      }
    }

    if (!first) {
      file.println("</table></div>");
    }
  }

  // prints the list of subnodes with the specified name
  void listNodesWithTitle(File &file, XmlNode *node)
  {
    list<XmlNode *> children = node->getChildren();
    list<XmlNode *>::iterator it;
    bool first = true;

    String name = node->getName() == "doc" ? "chapter": "section";

    for (it = children.begin(); it != children.end(); ++it) {
      XmlAttribute *titleAttrib = (*it)->getAttribute("title");
      
      if (titleAttrib != NULL) {
	if (first) {
	  first = false;
	  file.println("<h3 class=\"dtH3\">"+getPluralOf(name)+"</h3>");
	}
	
	// new item
	file.println("<b><a href=\""+resolveFileName(*it)+".html\">"+titleAttrib->getValue()+"</a></b>");
	file.println("<blockquote class=\"dtBlock\">");
	file.println((*it)->getChild("summary") != NULL ? getFormattedNodeText((*it)->getChild("summary")): "");
	file.println("</blockquote>");
      }
    }

//     if (!first) {
//       file.println("</div>");
//     }
  }

  // prints the table of subnodes with the specified name
  void tableNodesWithName(File &file,
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
			Class *cppClass,
			Member *member)
  {
    String className = cppClass->name;
    String memberName = member->name;
    String memberTitle = preProcessTitle(member->xmlNode);
    String syntaxText = preProcessSyntax(member->xmlNode);

    File file(mRootDirectory.addPathComponent(fileName));

    headerTemplate(file, className+"::"+memberTitle);
    mCrossRef.push_back(member->xmlNode);

    // Summary
    XmlNode *summary = member->xmlNode->getChild("summary");
    if (summary != NULL)
      file.println("<p>"+getFormattedNodeText(summary)+"</p>");

    // Syntax
    if (!syntaxText.empty())
      file.println("<div class=\"syntax\">"+syntaxText+"</div>");

    // Parameters
    if (member->xmlNode->getChild("param")) {
      file.println("<h4 class=\"dtH4\">Parámetros</h4>");
      file.println("<dl>");

      list<XmlNode *> children = member->xmlNode->getChildren();
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
    XmlNode *remarks = member->xmlNode->getChild("remarks");
    if (remarks != NULL) {
      file.println("<h4 class=\"dtH4\">Comentarios</h4>");
      file.println("<p>"+getFormattedNodeText(remarks)+"</p>");
    }

    // Example
    XmlNode *example = member->xmlNode->getChild("example");
    if (example != NULL) {
      file.println("<h4 class=\"dtH4\">Ejemplo</h4>");
      file.println("<p>"+getFormattedNodeText(example)+"</p>");
    }

    footerTemplate(file, fileName, member->xmlNode);
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

  String getMembersOfTypeTitle(const String &memberType, Access memberAccess)
  {
    String prefix, suffix;

    if (memberAccess == PublicAccess)
      suffix = (memberType == "signal") ? "públicas": "públicos";
    else if (memberAccess == ProtectedAccess)
      suffix = (memberType == "signal") ? "protegidas": "protegidos";
    else if (memberAccess == PrivateAccess)
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

  String getImageFileName(String memberType, Access memberAccess)
  {
    String prefix, suffix = ".gif";

    if (memberAccess == PublicAccess)
      prefix = "pub";
    else if (memberAccess == ProtectedAccess)
      prefix = "prot";
    else if (memberAccess == PrivateAccess)
      prefix = "priv";

    if (memberType == "ctor")
      memberType = "method";
    else if (memberType == "signal")
      memberType = "event";

    return prefix + memberType + suffix;
  }

  void printHierarchy(File &file, Class *cppClass)
  {
    file.println("<p>");

    ////////////////////////////////////////
    // print base classes
    
    stack<Class *> baseStack;

    Class *baseClass = cppClass->baseClass;
    while (baseClass != NULL) {
      baseStack.push(baseClass);
      baseClass = baseClass->baseClass;
    }

    int indent = 0;

    while (!baseStack.empty()) {
      baseClass = baseStack.top();
      baseStack.pop();

      file.println("<a href=\""+resolveFileName(baseClass->xmlNode)+".html\">"+
		   baseClass->name+"</a><br />");

      ++indent;
      for (int c=0; c<indent; ++c)
	file.println("  ");
    }

    ////////////////////////////////////////
    // print classNode

    file.println("<b>"+cppClass->name+"</b>");
    indent++;

    ////////////////////////////////////////
    // now print sub-classes

    for (vector<Class *>::iterator
	   it = cppClass->subClasses.begin();
	 it != cppClass->subClasses.end(); ++it) {
      file.println("<br />");
      for (int c=0; c<indent; ++c)
	file.println("  ");
      file.println("<a href=\""+resolveFileName((*it)->xmlNode)+".html\">"+
		   (*it)->name+"</a>");
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
    list<XmlType *> typeString = contentNode->getTypeString();
    String res;

    for (list<XmlType *>::iterator it=typeString.begin();
	 it!=typeString.end(); ++it) {
      // XmlString
      if ((*it)->isString()) {
	res += (*it)->getText();
      }
      // XmlNode
      else if ((*it)->isNode()) {
	XmlNode *node = (*it)->getNode();

	// <br />
	if (node->getName() == "br") {
	  res += "<br />";
	}
	// <em></em>
	else if (node->getName() == "em") {
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
	// <paramref></paramref>
	else if (node->getName() == "paramref") {
	  res += "<b>" + getFormattedNodeText(node) + "</b>";
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
	  XmlNode *term, *desc;

	  // bullet & number
	  if (type == "bullet" || type == "number") {
	    if (type == "bullet")
	      res += "<ul type=\"disc\">";
	    else
	      res += "<ol>";
	    
	    for (list<XmlNode *>::iterator it = children.begin(); it != children.end(); ++it) {
	      if ((*it)->getName() == "item") {
		term = (*it)->getChild("term");
		desc = (*it)->getChild("description");

		res += "<li>";

		// term
		if (term != NULL) {
		  res += "<b>" + getFormattedNodeText(term);
		  if (desc != NULL) res += " - ";
		  res += "</b>";
		}

		// desc
		if (desc != NULL)
		  res += getFormattedNodeText(desc);
		
		res += "</li>";
	      }
	      else {
		throw Exception("Invalid tag inside <list></list>: \""+(*it)->getName()+"\"");
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
	      term = (*it)->getChild("term");
	      desc = (*it)->getChild("description");

	      if ((*it)->getName() == "listheader")
		res +=
		  "<tr valign=\"top\">"
		  "<th width=\"50%\">" + getFormattedNodeText(term) + "</th>"
		  "<th width=\"50%\">" + getFormattedNodeText(desc) + "</th></tr>";
	      else if ((*it)->getName() == "item")
		res +=
		  "<tr valign=\"top\">"
		  "<td>" + getFormattedNodeText(term) + "</td>"
		  "<td>" + getFormattedNodeText(desc) + "</td></tr>";
	      else {
		throw Exception("Invalid tag inside <list></list>: \""+(*it)->getName()+"\"");
	      }
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

  virtual String getName()
  {
    return mFileName;
  }
  
  virtual bool readLine(char *buf, int max)
  {
    return fgets(buf, max, mFile) != NULL;
  }
  
};

//////////////////////////////////////////////////////////////////////

class VacaDoc : public Application
{
public:

  virtual void main(vector<String> args)
  {
    // TODO remove this
    if (args.size() < 2) {
      args.push_back(String("..\\utilities\\VacaDoc\\Vaca.xml"));
    }

    if (args.size() < 2) {
      System::println("Usage:");
      System::println("  VacaDoc.exe DocFile.xml");
      System::println("");
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
  clock_t start = clock();
  printf("Start\n");

  VacaDoc *app(new VacaDoc);
  app->run();
  delete app;

  clock_t end = clock();
  printf("End (%d.%d)\n",
	 (end-start) / CLOCKS_PER_SEC,
	 100 * ((end-start) % CLOCKS_PER_SEC) / CLOCKS_PER_SEC);
  return 0;
}
