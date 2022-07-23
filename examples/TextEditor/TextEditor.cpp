// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include <cstdio>
#include "scintilla/Scintilla.h"
#include "resource.h"

using namespace std;
using namespace vaca;

//////////////////////////////////////////////////////////////////////
// View

class Document;

class View
{
protected:
  Document* m_document;

public:
  virtual ~View() { }
  virtual void onNotifyDocument() = 0;

  virtual Document* getDocument() { return m_document; }
};

//////////////////////////////////////////////////////////////////////
// Document

class Document
{
  String m_fileName;		// current file name for the document
  bool m_hasFileName;		// this document has a file name?
  vector<View*> m_views;	// Views attached to this Document

  typedef vector<View*>::iterator iterator;

public:

  Document(String fileName, bool hasFileName)
    : m_fileName(fileName)
    , m_hasFileName(hasFileName)
  {
  }

  void addView(View* view)
  {
    m_views.push_back(view);
    notify();
  }

  void removeView(View* view)
  {
    remove_from_container(m_views, view);
    notify();
  }

  int getViewNumber(View* view)
  {
    int index = 1;
    for (iterator it=m_views.begin(); it!=m_views.end(); ++it, ++index) {
      if (*it == view)
	return index;
    }
    return 0;
  }

  int getViewCount() { return m_views.size(); }
  bool hasFileName() { return m_hasFileName; }
  String getFileName() { return m_fileName; }

  void setHasFileName(bool state)
  {
    m_hasFileName = state;
  }

  void setFileName(const String &fileName)
  {
    m_fileName = fileName;
    notify();
  }

  // notifies all views that must be updated (the title-bar mainly,
  // because Scintilla handles views internally)
  void notify()
  {
    for (iterator it=m_views.begin(); it!=m_views.end(); ++it)
      (*it)->onNotifyDocument();
  }

};

//////////////////////////////////////////////////////////////////////
// A MdiChild with a SciEditor in its client area

class TextEditor : public MdiChild, public View
{
  SciEdit m_editor; // text editor inside this MdiChild frame

public:

  // creates a new TextEditor
  TextEditor(const String& fileName, bool hasFileName, MdiFrame* parent)
    : MdiChild(file_name(fileName), parent, MdiChild::Styles::Default |
					    Widget::Styles::ClientEdge)
    , m_editor(this)
  {
    // creates a new document
    m_document = new Document(fileName, hasFileName);

    // common initialization
    initialize();
  }

  // creates a new view for the specified textEditor
  TextEditor(TextEditor &textEditor)
    : MdiChild(textEditor.getText(),
	       dynamic_cast<MdiClient*>(textEditor.getParent()),
	       MdiChild::Styles::Default +
	       Widget::Styles::ClientEdge)
    , View()
    , m_editor(this)
  {
    // same document
    m_document = textEditor.m_document;

    // reference the same document pointer at Scintilla level (only
    // necessary for Scintilla)
    m_editor.setDocPointer(textEditor.m_editor.getDocPointer());

    // common initialization
    initialize();
  }

  virtual ~TextEditor()
  {
    // remove this view from the document
    m_document->removeView(this);

    // last view closed?
    if (m_document->getViewCount() == 0)
      delete m_document;
  }

private:

  void initialize()
  {
    // Set icon
    setIcon(ResourceId(IDI_VACADOC));

    // the editor'll be arranged to client area bounds
    setLayout(new ClientLayout);

    // on FocusEnter or Activate signals, put the focus to the m_editor
    // (so the user can start writting)
    FocusEnter.connect([this]{ m_editor.requestFocus(); });
    // Activate.connect([this]{ m_editor.requestFocus(); });

    // add this view to the document
    m_document->addView(this);

    // when the text or the selection of the editor is updated, we
    // call m_document->notify() to put the "*" in the title-bar of the
    // TextEditor
    m_editor.UpdateUI.connect([this]{ m_document->notify(); });
  }

  // method from View class
  virtual void onNotifyDocument()
  {
    String newTitle;

    // the modified mark (an asterisk)
    if (m_editor.isModified())
      newTitle += L"* ";

    // the name of the file
    newTitle += file_name(getFileName());

    // the view number (only if it's necessary)
    if (!isLastView()) {
      newTitle += L" : ";
      newTitle += convert_to<String>(m_document->getViewNumber(this));
    }

    // is the text different? (this avoid flicker of the title-bar
    // when we write text)
    if (getText() != newTitle)
      setText(newTitle);

    // here we don't need to invalidate the m_editor of each view,
    // because it's done by Scintilla automatically
  }

public:

  // opens a file and put its text into the editor
  bool openFile()
  {
    FILE* file = _wfopen(getFileName().c_str(), L"rb");
    if (file != NULL) {
      char buf[4096];
      int bytesReaded;
      while (!feof(file)) {
	bytesReaded = fread(buf, 1, sizeof(buf), file);
	m_editor.appendText(buf, bytesReaded);
      }
      fclose(file);

      // because the m_editor.appendText, we must reset the save-point
      // and undo information
      m_editor.emptyUndoBuffer();
      m_editor.setSavePoint();
      return true;
    }
    else
      return false;
  }

  // saves the text of the editor in a file
  bool saveFile()
  {
    FILE* file = _wfopen(getFileName().c_str(), L"wb");
    if (file != NULL) {
      int c, lines = m_editor.getLineCount();
      for (c=0; c<lines; c++) {
	String line = m_editor.getLine(c);
	fwrite(line.c_str(), 1, line.size(), file);
      }
      fclose(file);

      // set save-point (here we are in sync with the file in disk)
      m_editor.setSavePoint();
      // now the document has a file name
      m_document->setHasFileName(true);
      // and finally notify all views to update the "*" mark in them title-bar
      m_document->notify();
      return true;
    }
    else
      return false;
  }

  bool hasFileName() { return m_document->hasFileName(); }
  String getFileName() { return m_document->getFileName(); }
  SciEdit& getEditor() { return m_editor; }
  bool isLastView() const { return m_document->getViewCount() == 1; }

  void setFileName(const String& fileName)
  {
    m_document->setFileName(fileName);
  }

};

//////////////////////////////////////////////////////////////////////
// A MdiFrame with a collection of TextEditor as MdiChild

class MainFrame : public MdiFrame
		, public CommandsClient
{
  int m_docCounter; // counter for documents (only to make "Untitled1", "Untitled2", etc.)
  Font m_font;
  bool m_viewEol;

public:

  MainFrame()
    : MdiFrame(L"TextEditor")
    , m_docCounter(0)
    , m_font(L"Courier New", 10)
    , m_viewEol(false)
  {
    setMenuBar(createMenuBar());
    setIcon(ResourceId(IDI_VACA));

    createCommands();
  }

  virtual ~MainFrame()
  {
    // delete all TextEditors
    WidgetList editors = getMdiClient()->getChildren();

    for (WidgetList::iterator it=editors.begin(); it!=editors.end(); ++it) {
      TextEditor* textEditor = dynamic_cast<TextEditor*>(*it);
      delete_widget(textEditor);
    }
  }

private:

  //////////////////////////////////////////////////////////////////////
  // menu-bar & commands

  MenuBar* createMenuBar()
  {
    MenuBar* menuBar = new MenuBar;
    Menu* fileMenu = new Menu(L"&File");
    Menu* editMenu = new Menu(L"&Edit");
    Menu* optionsMenu = new Menu(L"&Options");
    MdiListMenu* windowsMenu = new MdiListMenu(L"&Windows");

    fileMenu->add(L"&New\tCtrl+N", ID_FILE_NEW, Keys::Control | Keys::N);
    fileMenu->add(L"&Open\tCtrl+O", ID_FILE_OPEN, Keys::Control | Keys::O);
    fileMenu->addSeparator();
    fileMenu->add(L"&Save\tCtrl+S", ID_FILE_SAVE, Keys::Control | Keys::S);
    fileMenu->add(L"Save &as...", ID_FILE_SAVE_AS);
    fileMenu->addSeparator();
    fileMenu->add(L"E&xit", ID_FILE_EXIT);

    editMenu->add(L"&Undo\tCtrl+Z", ID_EDIT_UNDO, Keys::Control | Keys::Z);
    editMenu->add(L"&Redo\tCtrl+Y", ID_EDIT_REDO, Keys::Control | Keys::Y);
    editMenu->addSeparator();
    editMenu->add(L"Cu&t\tCtrl+X", ID_EDIT_CUT, Keys::Control | Keys::X);
    editMenu->add(L"&Copy\tCtrl+C", ID_EDIT_COPY, Keys::Control | Keys::C);
    editMenu->add(L"&Paste\tCtrl+V", ID_EDIT_PASTE, Keys::Control | Keys::P);
    editMenu->add(L"Clea&r", ID_EDIT_CLEAR);
    editMenu->addSeparator();
    editMenu->add(L"&Find\tCtrl+F", ID_EDIT_FIND, Keys::Control | Keys::F);
    editMenu->add(L"R&eplace\tCtrl+H", ID_EDIT_REPLACE, Keys::Control | Keys::H);

    optionsMenu->add(L"Change &Font", ID_OPTIONS_CHANGE_FONT);
    optionsMenu->add(L"View &EOL", ID_OPTIONS_VIEW_EOL);

    windowsMenu->add(L"&Close\tCtrl+W", ID_WINDOWS_CLOSE, Keys::Control | Keys::W);
    windowsMenu->add(L"&Duplicate\tCtrl+D", ID_WINDOWS_DUPLICATE, Keys::Control | Keys::D);
    windowsMenu->add(L"&Cascade", ID_WINDOWS_CASCADE);

    menuBar->add(fileMenu);
    menuBar->add(editMenu);
    menuBar->add(optionsMenu);
    menuBar->add(windowsMenu);

    return menuBar;
  }

  void createCommands()
  {
    SignalCommand* cmd;

    addCommand(new SignalCommand(ID_FILE_NEW, &MainFrame::onNew, this));

    addCommand(new SignalCommand(ID_FILE_OPEN, &MainFrame::onOpen, this));

    addCommand(cmd = new SignalCommand(ID_FILE_SAVE));
    cmd->Execute.connect(&MainFrame::onSave, this);
    cmd->Enabled.connect([this]{ return canSave(); });

    addCommand(cmd = new SignalCommand(ID_FILE_SAVE_AS));
    cmd->Execute.connect(&MainFrame::onSaveAs, this);
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(new SignalCommand(ID_FILE_EXIT, &MainFrame::onExit, this));

    addCommand(cmd = new SignalCommand(ID_EDIT_UNDO));
    cmd->Execute.connect(&MainFrame::onUndo, this);
    cmd->Enabled.connect([this]{ return canUndo(); });

    addCommand(cmd = new SignalCommand(ID_EDIT_REDO));
    cmd->Execute.connect(&MainFrame::onRedo, this);
    cmd->Enabled.connect([this]{ return canRedo(); });

    addCommand(cmd = new SignalCommand(ID_EDIT_CUT));
    cmd->Execute.connect(&MainFrame::onCut, this);
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(cmd = new SignalCommand(ID_EDIT_COPY));
    cmd->Execute.connect(&MainFrame::onCopy, this);
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(cmd = new SignalCommand(ID_EDIT_PASTE));
    cmd->Execute.connect(&MainFrame::onPaste, this);
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(cmd = new SignalCommand(ID_EDIT_CLEAR));
    cmd->Execute.connect(&MainFrame::onClear, this);
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(cmd = new SignalCommand(ID_EDIT_FIND));
    cmd->Execute.connect([this]{ onFindReplace(false); });
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(cmd = new SignalCommand(ID_EDIT_REPLACE));
    cmd->Execute.connect([this]{ onFindReplace(true); });
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(new SignalCommand(ID_OPTIONS_CHANGE_FONT, &MainFrame::onChangeFont, this));

    addCommand(cmd = new SignalCommand(ID_OPTIONS_VIEW_EOL));
    cmd->Execute.connect(&MainFrame::onToggleViewEol, this);
    cmd->Enabled.connect([this]{ return updateViewEolMenuItem(); });

    addCommand(cmd = new SignalCommand(ID_WINDOWS_CLOSE));
    cmd->Execute.connect(&MainFrame::onCloseWindow, this);
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(cmd = new SignalCommand(ID_WINDOWS_DUPLICATE));
    cmd->Execute.connect(&MainFrame::onDuplicateWindow, this);
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });

    addCommand(cmd = new SignalCommand(ID_WINDOWS_CASCADE));
    cmd->Execute.connect(&MdiClient::cascade, getMdiClient());
    cmd->Enabled.connect([this]{ return isTextEditorAvailable(); });
  }

  void onNew()
  {
    String title = format_string(L"Untitled %d", ++m_docCounter);
    addTextEditor(new TextEditor(title, false, this));
  }

  void onOpen()
  {
    OpenFileDialog dlg(L"Open file", this);
    addFilters(dlg);

    dlg.setMultiselect(true);

    if (dlg.doModal()) {
      // selected file names
      vector<String> fileNames = dlg.getFileNames();

      // for each selected file name
      for (vector<String>::iterator it=fileNames.begin(); it!=fileNames.end(); ++it) {
	String fileName = *it;

	// check if this file is already opened
	TextEditor* textEditor = getTextEditorByFileName(fileName);

	// active that editor (if exist)
	if (textEditor != NULL) {
	  getMdiClient()->activate(textEditor);
	}
	// create the new editor reading the file
	else {
	  textEditor = new TextEditor(fileName, true, this);

	  // open the file
	  if (textEditor->openFile())
	    addTextEditor(textEditor);
	  else {
	    delete_widget(textEditor);
	    MsgBox::show(this, L"Error",
			 L"Error opening file '"+fileName+L"'",
			 MsgBox::Type::Ok,
			 MsgBox::Icon::Error);
	    break;
	  }
	}
      }
    }
  }

  void onSave()
  {
    saveTextEditor(getTextEditor(), L"Save file", false);
  }

  bool canSave()
  {
    TextEditor* textEditor = getTextEditor();
    return (textEditor != NULL)
      && (!(textEditor->hasFileName())
	  || (textEditor->getEditor().isModified()));
  }

  void onSaveAs()
  {
    saveTextEditor(getTextEditor(), L"Save file as...", true);
  }

  bool isTextEditorAvailable()
  {
    return getTextEditor() != NULL;
  }

  void onExit()
  {
    CloseEvent ev(this);
    onClose(ev);
    if (!ev.isCanceled())
      // DISPOSE();
      setVisible(false);
  }

  void onUndo()
  {
    getTextEditor()->getEditor().undo();
  }

  bool canUndo()
  {
    TextEditor* textEditor = getTextEditor();
    return textEditor && (textEditor->getEditor().canUndo());
  }

  void onRedo()
  {
    getTextEditor()->getEditor().redo();
  }

  bool canRedo()
  {
    TextEditor* textEditor = getTextEditor();
    return textEditor && (textEditor->getEditor().canRedo());
  }

  void onCut()
  {
    getTextEditor()->getEditor().cutTextToClipboard();
  }

  void onCopy()
  {
    getTextEditor()->getEditor().copyTextToClipboard();
  }

  void onPaste()
  {
    getTextEditor()->getEditor().pasteTextFromClipboard();
  }

  void onClear()
  {
    getTextEditor()->getEditor().clearText();
  }

  //////////////////////////////////////////////////////////////////////
  // Find stuff begin
  void onFindReplace(bool replace)
  {
    FindTextDialog dlg(replace, this);
    dlg.FindNext.connect([this, &dlg]{ onFindNext(&dlg); });
    dlg.Replace.connect([this, &dlg]{ onReplace(&dlg); });
    dlg.ReplaceAll.connect([this, &dlg]{ onReplaceAll(&dlg); });
    dlg.doModal();
  }

  void onFindNext(FindTextDialog* dlg)
  {
    SciEdit &sciEdit(getTextEditor()->getEditor());
    String findWhat = dlg->getFindWhat();
    int flags =
      (dlg->isMatchCase() ? SCFIND_MATCHCASE: 0) |
      (dlg->isWholeWord() ? SCFIND_WHOLEWORD: 0);

    // forward search
    if (dlg->isForward()) {
      sciEdit.goToPos(sciEdit.getSelectionEnd());
      sciEdit.searchAnchor();

      if (!sciEdit.searchNext(flags, findWhat))
	Beep(100, 10);

      int start = sciEdit.getSelectionStart();
      int end = sciEdit.getSelectionEnd();
      sciEdit.goToPos(end);
      sciEdit.setAnchor(start);
    }
    // backward search
    else {
      sciEdit.goToPos(sciEdit.getSelectionStart());
      sciEdit.searchAnchor();

      if (!sciEdit.searchPrev(flags, findWhat))
	Beep(100, 10);

      int start = sciEdit.getSelectionStart();
      int end = sciEdit.getSelectionEnd();
      sciEdit.goToPos(start);
      sciEdit.setAnchor(end);
    }
  }

  void onReplace(FindTextDialog* dlg)
  {
    SciEdit &sciEdit(getTextEditor()->getEditor());
    int start = sciEdit.getSelectionStart();
    int end = sciEdit.getSelectionEnd();

    sciEdit.goToPos(start);

    onFindNext(dlg);

    if (start != end &&
	start == sciEdit.getSelectionStart() &&
	end == sciEdit.getSelectionEnd()) {
      sciEdit.replaceSel(dlg->getReplaceWith());
      onFindNext(dlg);
    }
  }

  void onReplaceAll(FindTextDialog* dlg)
  {
    SciEdit &sciEdit(getTextEditor()->getEditor());
    do {
      onReplace(dlg);
    } while (sciEdit.getSelectionStart() != sciEdit.getSelectionEnd());
  }

  // Find stuff end
  //////////////////////////////////////////////////////////////////////

  void onChangeFont()
  {
    FontDialog dialog(m_font, this);
    if (dialog.doModal()) {
      m_font = dialog.getFont();

      // set the font of all editors
      WidgetList editors = getMdiClient()->getChildren();
      for (WidgetList::iterator
	     it=editors.begin(); it!=editors.end(); ++it)
	dynamic_cast<TextEditor*>(*it)->getEditor().setFont(m_font);
    }
  }

  void onToggleViewEol()
  {
    m_viewEol = !m_viewEol;

    // set the new state of ViewEol to all editors
    WidgetList editors = getMdiClient()->getChildren();

    for (WidgetList::iterator it=editors.begin(); it!=editors.end(); ++it)
      dynamic_cast<TextEditor*>(*it)->getEditor().setViewEol(m_viewEol);
  }

  bool updateViewEolMenuItem()
  {
    getMenuBar()->getMenuItemById(ID_OPTIONS_VIEW_EOL)->setChecked(m_viewEol);
    return true;
  }

  void onCloseWindow()
  {
    TextEditor* textEditor = getTextEditor();
    if (closeTextEditor(textEditor, false))
      delete_widget(textEditor);
  }

  void onDuplicateWindow()
  {
    addTextEditor(new TextEditor(*getTextEditor()));
  }

  // when the user press the close button in a MdiChild (TextEditor child)
  void onCloseTextEditor(CloseEvent& ev)
  {
    TextEditor* textEditor = dynamic_cast<TextEditor*>(ev.getSource());

    // show the warning dialog
    if (closeTextEditor(textEditor, false)) {
      // the user really want to close this...

      // the widget must be deleted but we can't delete it because
      // we're inside an event that was generated by the same widget
      // (see TN006)
      delete_widget(textEditor);
    }
    // the user cancels the close operation
    else {
      // cancel the close event (don't close the TextEditor)
      ev.cancel();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // events

  // when the user press the close button of the MainFrame we must to
  // close all the editors
  virtual void onClose(CloseEvent &ev)
  {
    MdiFrame::onClose(ev);

    vector<Document*> asked;
    WidgetList editors = getMdiClient()->getChildren();

    // for each children
    for (WidgetList::iterator it=editors.begin(); it!=editors.end(); ++it) {
      TextEditor* textEditor = dynamic_cast<TextEditor*>(*it);
      Document* document = textEditor->getDocument();

      // we already asked to close document?
      if (std::find(asked.begin(), asked.end(), document) != asked.end())
	continue;
      // if not, add it to the list
      else
	asked.push_back(document);

      // close the editor
      if (!closeTextEditor(textEditor, true)) {
	// the user cancels the close operation...

	// cancel the event (don't close the MainFrame)
	ev.cancel();

	// don't delete any TextEditor
	return;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // logic
private:

  // returns the current text editor
  TextEditor* getTextEditor()
  {
    return dynamic_cast<TextEditor*>(getMdiClient()->getActive());
  }

  // adds file filters to the FileDialog
  void addFilters(FileDialog &dlg)
  {
    dlg.addFilter(L"All (*.*)", L"*.*", true);
    dlg.addFilter(L"Text files (*.txt)", L"*.txt");
  }

  // adds a new text editor to the application
  void addTextEditor(TextEditor* textEditor)
  {
    // set the font of the editor to the current selected font
    textEditor->getEditor().setFont(m_font);

    // set the ViewEol state
    textEditor->getEditor().setViewEol(m_viewEol);

    // bind the close event
    textEditor->Close.connect(&MainFrame::onCloseTextEditor, this);
  }

  // Saves the file that is associated with 'textEditor'.  Returns
  // true if the file was saved, or false in case of error or if the
  // user cancels the operation.
  bool saveTextEditor(TextEditor* textEditor, const String& saveDialogTitle,
		      bool forceSelectName)
  {
    // the file doesn't have a file name?
    if (forceSelectName || !textEditor->hasFileName()) {
      // create the dialog to select the file name
      SaveFileDialog dlg(saveDialogTitle, this);
      addFilters(dlg);

      // set initial file name for the dialog
      dlg.setFileName(textEditor->getFileName());

      // show the dialog to the user, and ask if it press OK
      if (dlg.doModal()) {
	// the user press OK...

	// change the name of the file in the text editor (to update the
	// title, and to known that this file has another name now)
	textEditor->setFileName(dlg.getFileName());
      }
      // the user cancels the operation
      else
	return false;
    }

    // save the file to the disk
    if (textEditor->saveFile()) {
      // all just fine
      return true;
    }
    // error...
    else {
      MsgBox::show(this, L"Error",
		   L"Error saving file '" + textEditor->getFileName() + L"'",
		   MsgBox::Type::Ok,
		   MsgBox::Icon::Error);
      return false;
    }
  }

  // returns false if the user don't want to close the text editor
  bool closeTextEditor(TextEditor* textEditor, bool forceAsk)
  {
    if ((textEditor->isLastView() || forceAsk) &&
	textEditor->getEditor().isModified()) {
      MsgBox::Result res = MsgBox::show(this, L"TextEditor",
					L"Save changes to '" + textEditor->getFileName() + L"'?",
					MsgBox::Type::YesNoCancel,
					MsgBox::Icon::Question);

      if (res == MsgBox::Result::Cancel) {
	return false;		// "Cancel" in message-box
      }
      else if (res == MsgBox::Result::Yes) {
	if (!saveTextEditor(textEditor, L"Save file", false))
	  return false;		// "Cancel" in save-file dialog
      }
    }
    return true;
  }

  // returns the first found TextEditor that is editing the "fileName"
  TextEditor* getTextEditorByFileName(const String& fileName)
  {
    WidgetList listOfTextEditors = getMdiClient()->getChildren();
    WidgetList::iterator it;

    for (it=listOfTextEditors.begin(); it!=listOfTextEditors.end(); ++it) {
      TextEditor* textEditor = dynamic_cast<TextEditor*>(*it);

      if (textEditor->getFileName() == fileName)
	return textEditor;
    }

    return NULL;
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
