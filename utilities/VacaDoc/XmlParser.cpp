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
#include "XmlParser.h"
#include "XmlException.h"
#include "XmlNode.h"
#include "XmlAttribute.h"

XmlParser::XmlParser(XmlStream *stream)
  : XmlRawParser(stream)
{
}

XmlParser::~XmlParser()
{
}

XmlNode *XmlParser::parseRoot()
{
  try {
    String tagDefinition;
    stack<XmlNode *> nodeStack;
    XmlNode *root = NULL;

    currentNode = NULL;
    
    while (!(tagDefinition = nextTag()).empty()) {
      TagType tagType;
      XmlNode *parent;
      XmlNode *node = getXmlNodeFromTagDefinition(tagDefinition, tagType);

      if (nodeStack.empty())
	parent = NULL;
      else
	parent = nodeStack.top();

      //       printf("%s:%d: ", getFileName().c_str(), getCurrentLine());

      switch (tagType) {

	case Prolog:
	  node = onPrologTag(node);
	  break;
	  
	case Start:
	  // first tag?
	  if (root == NULL) {
	    root = node;
	  }
	  
	  node = onStartTag(node);
	  if (node != NULL) {
	    if (parent != NULL)
	      parent->addChild(node);
	  
	    nodeStack.push(node);
	  }
	  break;
	  
	case End:
	  if (nodeStack.empty()) {
	    throw XmlException(String("End tag '") +
			       node->getName() +
			       "' not expected.");
	  }
	  else {
	    if (node->getName() != parent->getName())
	      throw XmlException(String("End tag '") +
				 node->getName() +
				 "' mismatch with start tag '" +
				 parent->getName() + "'");
	  }

	  // don't call onEndTag(node), because "node" doesn't has the
	  // children and attributes (like "parent")
	  onEndTag(parent);
	  nodeStack.pop();
	  break;
	  
	case Empty:
	  // first tag?
	  if (root == NULL) {
	    throw XmlException("Root tag can't be an empty tag");
	  }

	  node = onEmptyTag(node);
	  if (node != NULL) {
	    if (parent != NULL)
	      parent->addChild(node);
	  }
	  break;
	  
      }
      
      //       printf("\"%s\" -> \"%s\"\n", tagDefinition.c_str(), node->getName().c_str());
      //       printf("\"%s\"\n", node->getName().c_str());

      currentNode = !nodeStack.empty() ? nodeStack.top(): NULL;
    }

    if (!nodeStack.empty()) {
      String msg = "Expected end tags: ";
      for (;;) {
	msg += "'" + nodeStack.top()->getName() + "'";
	nodeStack.pop();
	if (!nodeStack.empty())
	  msg += ", ";
	else
	  break;
      }
      throw XmlException(msg);
    }

    return root;
  }
  catch (XmlException &e) {
    // add more information to the exception
    throw XmlException(getFileName() + ":" +
		       String::fromInt(getCurrentLine(), 10)
		       + ": " +
		       e.getMessage());
  }
}

void XmlParser::onTextOutside(int character)
{
  if (currentNode != NULL)
    currentNode->addChar(character);
}

XmlNode *XmlParser::getXmlNodeFromTagDefinition(const String &tagDefinition,
						TagType &tagType)
{
  String::const_iterator it = tagDefinition.begin();
  String::const_iterator end = tagDefinition.end();
  int lastChar = tagDefinition.empty() ? 0: tagDefinition[tagDefinition.size()-1];
      
  if (tagDefinition[0] == '?' ||
      tagDefinition[0] == '!') {
    // TODO throw if tagDefinition[0] == '?' && lastChar != '?'
	
    tagType = Prolog;
  }
  else if (tagDefinition[0] == '/') {
    tagType = End;
    ++it;
  }
  else if (lastChar == '/') {
    tagType = Empty;
  }
  else {
    tagType = Start;
  }

  // name of the tag
  String name;
  for (; it != end && *it != '\0' && !isspace(*it); ++it)
    name.push_back(*it);
  XmlNode *node = new XmlNode(name);

  // parse attributes
  for (; it != end && *it != '\0'; ++it) {
    // prolog tags
    if (*it == '?' && tagDefinition[0] == '?') {
      if ((it+1) == end)
	break;
      else
	throw XmlException("Whitespace or '?' expected.");
    }
    // empty tags
    else if (*it == '/') {
      if (tagType == Empty && (it+1) == end)
	break;
      else
	throw XmlException("'/' not expected.");
    }

    if (!isspace(*it)) {
      // end tag?
      if (tagType == End)
	throw XmlException("End tag can't have attributes");
	
      String attrName;
      String attrValue;

      for (; it != end && *it != '\0' && *it != '='; ++it) {
	attrName.push_back(*it);
      }

      if (it == end || *it == '\0')
	throw XmlException("Attribute '"+attrName+"' without '=' sign.");

      ++it;			// jump the '=' character
      if (it == end || *it == '\0' || (*it != '\'' && *it != '\"'))
	throw XmlException("Literal string with quotes expected as value for attribute '"+attrName+"'.");

      char quote = *it;
      ++it;			// jump the quote character
	
      for (; it != end && *it != '\0' && *it != quote; ++it) {
	attrValue.push_back(*it);
      }

      node->addAttribute(new XmlAttribute(attrName, attrValue));
    }
  }

  return node;
}
