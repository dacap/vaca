// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <Vaca/Vaca.h>
#include <memory>
#include "Document.h"
#include "Editor.h"

using namespace Vaca;

enum {
  ID_NEW_DOCUMENT,
  ID_CLOSE_DOCUMENT,
  ID_EXIT,
  ID_SELECT_DOCUMENT
};

//////////////////////////////////////////////////////////////////////
// Example class definition

class Document;
class MainFrame;

class Example : public Application
	      , public CommandsClient
{
  typedef std::vector<DocumentPtr> Documents;

  int m_docCounter;
  Documents m_docs;
  DocumentPtr m_currentDoc;
  std::auto_ptr<MainFrame> m_mainFrame;

  virtual void main();

public:
  Example();
  virtual ~Example();

  void onNew();
  void onClose();

  bool hasCurrentDoc();
  void setCurrentDocument(DocumentPtr doc);

  Documents getDocuments() { return m_docs; };
  DocumentPtr getCurrentDocument() { return m_currentDoc; };

  Signal1<void, DocumentPtr> DocumentChange;
  Signal1<void, DocumentPtr> NewDocument;
  Signal1<void, DocumentPtr> CloseDocument;
};

Example* GetApp()
{
  return static_cast<Example*>(Application::getInstance());
}

//////////////////////////////////////////////////////////////////////
// DocMenuItem

class DocMenuItem : public MenuItem
{
  DocumentPtr m_doc;
public:
  DocMenuItem(DocumentPtr doc)
    : MenuItem(doc->getName(), 0)
    , m_doc(doc) {
  }

  DocumentPtr getDoc() {
    return m_doc;
  }

  virtual void onClick(MenuItemEvent& ev) {
    MenuItem::onClick(ev);
    GetApp()->setCurrentDocument(m_doc);
  }
};

//////////////////////////////////////////////////////////////////////
// MainFrame

class MainFrame : public Frame
{
  Editor m_editor;
  Menu* m_listMenu;

public:
  MainFrame()
    : Frame(L"Undo (WIP)")
    , m_editor(this)
  {
    setMenuBar(createMenuBar());
    setLayout(new BoxLayout(Orientation::Horizontal, false));
    m_editor.setConstraint(new BoxConstraint(true));

    GetApp()->DocumentChange.connect(&MainFrame::onDocumentChange, this);
    GetApp()->NewDocument.connect(&MainFrame::onNewDocument, this);
    GetApp()->CloseDocument.connect(&MainFrame::onCloseDocument, this);
  }

private:
  MenuBar* createMenuBar()
  {
    MenuBar* menuBar = new MenuBar();
    Menu* docMenu = new Menu(L"Document");
    m_listMenu = new Menu(L"List");

    docMenu->add(L"New", ID_NEW_DOCUMENT);
    docMenu->add(L"Close", ID_CLOSE_DOCUMENT);
    docMenu->addSeparator();
    docMenu->add(L"Exit", ID_EXIT);

    menuBar->add(docMenu);
    menuBar->add(m_listMenu);
    return menuBar;
  }

  void onDocumentChange(DocumentPtr doc)
  {
    m_editor.setDocument(doc);
  }

  void onNewDocument(DocumentPtr doc)
  {
    m_listMenu->add(new DocMenuItem(doc));
    updateListMenuIds();
  }

  void onCloseDocument(DocumentPtr doc)
  {
    int i, count = m_listMenu->getItemCount();
    for (i=0; i<count; ++i) {
      DocMenuItem* item = static_cast<DocMenuItem*>(m_listMenu->getMenuItemByIndex(i));
      if (item->getDoc() == doc) {
	m_listMenu->remove(item);
	updateListMenuIds();
	delete item;
	break;
      }
    }
  }

  // Important: After we modified the "m_listMenu" we have to call
  // this routine to setup the IDs of each menu-item in the menu.
  // See TN013
  void updateListMenuIds()
  {
    int i, count = m_listMenu->getItemCount();
    for (i=0; i<count; ++i) {
      MenuItem* item = m_listMenu->getMenuItemByIndex(i);
      item->setId(ID_SELECT_DOCUMENT+i);
    }
  }

};

//////////////////////////////////////////////////////////////////////
// Example implementation

Example::Example()
  : m_docCounter(0)
  , m_currentDoc(NULL)
{
  // Warning: the "Example::m_mainFrame" must be constructed after the
  // "Example::DocumentChange" member is constructed. See TN012.
  m_mainFrame.reset(new MainFrame());

  // commands
  addCommand(new SignalCommand(ID_NEW_DOCUMENT, &Example::onNew, this));
  addCommand(new SignalCommand(ID_EXIT, Bind(&MainFrame::setVisible, m_mainFrame.get(), false)));
  
  SignalCommand* cmd = new SignalCommand(ID_CLOSE_DOCUMENT);
  cmd->Execute.connect(&Example::onClose, this);
  cmd->Enabled.connect(&Example::hasCurrentDoc, this);
  addCommand(cmd);
}

Example::~Example()
{
}

void Example::main()
{
  m_mainFrame->setVisible(true);
}

void Example::onNew()
{
  m_docCounter++;

  // create the new document
  DocumentPtr doc(new Document(format_string(L"Document #%d", m_docCounter)));
  doc->add(0, format_string(L"Text %d", m_docCounter));

  // add the document to the list
  m_docs.push_back(doc);

  // new document signal, and set this document as the current one
  NewDocument(doc);
  DocumentChange(m_currentDoc = doc);
}

void Example::onClose()
{
  CloseDocument(m_currentDoc);

  m_currentDoc.reset();
  DocumentChange(m_currentDoc);
}

bool Example::hasCurrentDoc()
{
  return m_currentDoc != NULL;
}

void Example::setCurrentDocument(DocumentPtr doc)
{
  DocumentChange(m_currentDoc = doc);
}

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Example app;
  app.run();
  return 0;
}
