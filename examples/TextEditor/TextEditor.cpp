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

#include "Vaca/Vaca.h"
#include "scintilla/include/Scintilla.h"
#include "resource.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// View

class Document;

class View
{
protected:
  Document *mDocument;

public:
  virtual void onNotifyDocument() = 0;

  virtual Document *getDocument() { return mDocument; }
};

//////////////////////////////////////////////////////////////////////
// Document

class Document
{
  String mFileName;		// current file name for the document
  bool mHasFileName;		// this document has a file name?
  std::vector<View *> mViews;	// Views attached to this Document

  typedef std::vector<View *>::iterator iterator;

public:
  
  Document(String fileName, bool hasFileName)
    : mFileName(fileName)
    , mHasFileName(hasFileName)
  {
  }

  void addView(View *view)
  {
    mViews.push_back(view);
    notify();
  }

  void removeView(View *view)
  {
    remove_element_from_container(mViews, view);
    notify();
  }

  int getViewNumber(View *view)
  {
    int index = 1;
    for (iterator it=mViews.begin(); it!=mViews.end(); ++it, ++index) {
      if (*it == view)
	return index;
    }
    return 0;
  }

  int getViewCount() { return mViews.size(); }
  bool hasFileName() { return mHasFileName; }
  String getFileName() { return mFileName; }

  void setHasFileName(bool state)
  {
    mHasFileName = state;
  }

  void setFileName(const String &fileName)
  {
    mFileName = fileName;
    notify();
  }

  // notifies all views that must be updated (the title-bar mainly,
  // because Scintilla handles views internally)
  void notify()
  {
    for (iterator it=mViews.begin(); it!=mViews.end(); ++it)
      (*it)->onNotifyDocument();
  }

};

//////////////////////////////////////////////////////////////////////
// A MdiChild with a SciEditor in its client area

class TextEditor : public MdiChild, public View
{
  SciEditor mEditor; // text editor inside this MdiChild frame

public:

  // creates a new TextEditor
  TextEditor(const String &fileName, bool hasFileName, MdiFrame *parent)
    : MdiChild(fileName.getFileName(), parent,
	       MdiChildStyle + ClientEdgeStyle)
    , mEditor(this)
  {
    // creates a new document
    mDocument = new Document(fileName, hasFileName);

    // common initialization
    initialize();
  }

  // creates a new view for the specified textEditor
  TextEditor(TextEditor &textEditor)
    : MdiChild(textEditor.getText(),
	       static_cast<MdiClient *>(textEditor.getParent()),
	       MdiChildStyle + ClientEdgeStyle)
    , mEditor(this)
  {
    // same document
    mDocument = textEditor.mDocument;

    // reference the same document pointer at Scintilla level (only
    // necessary for Scintilla)
    mEditor.setDocPointer(textEditor.mEditor.getDocPointer());

    // common initialization
    initialize();
  }

  ~TextEditor()
  {
    // remove this view from the document
    mDocument->removeView(this);

    // last view closed?
    if (mDocument->getViewCount() == 0)
      delete mDocument;
  }

private:

  void initialize()
  {
    // Set icon
    setIcon(IDI_VACADOC);

    // the editor'll be arranged to client area bounds
    setLayout(new ClientLayout);

    // on GotFocus or Activate signals, put the focus to the mEditor
    // (so the user can start writting)
    GotFocus.connect(Bind(&SciEditor::focus, &mEditor));
    // Activate.connect(Bind(&SciEditor::focus, &mEditor));

    // add this view to the document
    mDocument->addView(this);

    // when the text or the selection of the editor is updated, we
    // call mDocument->notify() to put the "*" in the title-bar of the
    // TextEditor
    mEditor.UpdateUI.connect(Bind(&Document::notify, mDocument));
  }

  // method from View class
  virtual void onNotifyDocument()
  {
    String newTitle =
      // the "*"
      (mEditor.isModified() ? "* ": "") +
      // the name of the file
      getFileName().getFileName() +
      // the view number (only if it's necessary)
      (isLastView() ? "": " : "+String::fromInt(mDocument->getViewNumber(this)));

    // is the text different? (this avoid flicker of the title-bar
    // when we write text)
    if (getText() != newTitle)
      setText(newTitle);

    // here we don't need to invalidate the mEditor of each view,
    // because it's done by Scintilla automatically
  }

public:

  // opens a file and put its text into the editor
  bool openFile()
  {
    FILE *file = fopen(getFileName().c_str(), "rb");
    if (file != NULL) {
      char buf[4096];
      int bytesReaded;
      while (!feof(file)) {
	bytesReaded = fread(buf, 1, sizeof(buf), file);
	mEditor.appendText(buf, bytesReaded);
      }
      fclose(file);

      // because the mEditor.appendText, we must reset the save-point
      // and undo information
      mEditor.emptyUndoBuffer();
      mEditor.setSavePoint();
      return true;
    }
    else
      return false;
  }

  // saves the text of the editor in a file
  bool saveFile()
  {
    FILE *file = fopen(getFileName().c_str(), "wb");
    if (file != NULL) {
      int c, lines = mEditor.getLineCount();
      for (c=0; c<lines; c++) {
	String line = mEditor.getLine(c);
	fwrite(line.c_str(), 1, line.size(), file);
      }
      fclose(file);

      // set save-point (here we are in sync with the file in disk)
      mEditor.setSavePoint();
      // now the document has a file name
      mDocument->setHasFileName(true);
      // and finally notify all views to update the "*" mark in them title-bar
      mDocument->notify();
      return true;
    }
    else
      return false;
  }
  
  bool hasFileName() { return mDocument->hasFileName(); }
  String getFileName() { return mDocument->getFileName(); }
  SciEditor &getEditor() { return mEditor; }
  bool isLastView() { return mDocument->getViewCount() == 1; }

  void setFileName(const String &fileName)
  {
    mDocument->setFileName(fileName);
  }

};

//////////////////////////////////////////////////////////////////////
// A MdiFrame with a collection of TextEditor as MdiChild

class MainFrame : public MdiFrame
{
  int mDocCounter; // counter for documents (only to make "Untitled1", "Untitled2", etc.)
  MenuBar mMenuBar;
  Menu mFileMenu;
  Menu mEditMenu;
  Menu mOptionsMenu;
  MdiListMenu mWindowsMenu;
  Font mFont;
  bool mViewEol;
  FindTextDialog *mFindDlg;

public:

  MainFrame()
    : MdiFrame("TextEditor")
    , mDocCounter(0)
    , mFileMenu("&File")
    , mEditMenu("&Edit")
    , mOptionsMenu("&Options")
    , mWindowsMenu("&Windows")
    , mFont("Courier New", 10)
    , mViewEol(false)
    , mFindDlg(NULL)
  {
    initializeMenuBar();
    setMenuBar(&mMenuBar);
    setLayout(new ClientLayout);
    setIcon(IDI_VACA);
  }

private:

  void initializeMenuBar()
  {
    MenuItem *menuItem;

    // File/New
    menuItem = mFileMenu.add("&New\tCtrl+N", Keys::Control | Keys::N);
    menuItem->Action.connect(Bind(&MainFrame::onNew, this));

    // File/Open
    menuItem = mFileMenu.add("&Open\tCtrl+O", Keys::Control | Keys::O);
    menuItem->Action.connect(Bind(&MainFrame::onOpen, this));

    // ---
    mFileMenu.addSeparator();

    // File/Save
    menuItem = mFileMenu.add("&Save\tCtrl+S", Keys::Control | Keys::S);
    menuItem->Action.connect(Bind(&MainFrame::onSave, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_forSave, this));

    // File/Save As
    menuItem = mFileMenu.add("Save &as...");
    menuItem->Action.connect(Bind(&MainFrame::onSaveAs, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // ---
    mFileMenu.addSeparator();

    // File/Exit
    menuItem = mFileMenu.add("E&xit");
    menuItem->Action.connect(Bind(&MainFrame::onExit, this));

    // Edit/Undo
    menuItem = mEditMenu.add("&Undo\tCtrl+Z", Keys::Control | Keys::Z);
    menuItem->Action.connect(Bind(&MainFrame::onUndo, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_canUndo, this));

    // Edit/Redo
    menuItem = mEditMenu.add("&Redo\tCtrl+Y", Keys::Control | Keys::Y);
    menuItem->Action.connect(Bind(&MainFrame::onRedo, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_canRedo, this));

    // ---
    mEditMenu.addSeparator();

    // Edit/Cut
    menuItem = mEditMenu.add("Cu&t\tCtrl+X", Keys::Control | Keys::X);
    menuItem->Action.connect(Bind(&MainFrame::onCut, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Copy
    menuItem = mEditMenu.add("&Copy\tCtrl+C", Keys::Control | Keys::C);
    menuItem->Action.connect(Bind(&MainFrame::onCopy, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Paste
    menuItem = mEditMenu.add("&Paste\tCtrl+V", Keys::Control | Keys::P);
    menuItem->Action.connect(Bind(&MainFrame::onPaste, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Clear
    menuItem = mEditMenu.add("Clea&r");
    menuItem->Action.connect(Bind(&MainFrame::onClear, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // ---
    mEditMenu.addSeparator();

    // Edit/Find
    menuItem = mEditMenu.add("&Find\tCtrl+F", Keys::Control | Keys::F);
    menuItem->Action.connect(Bind(&MainFrame::onFindReplace, this, false));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Edit/Replace
    menuItem = mEditMenu.add("R&eplace\tCtrl+H", Keys::Control | Keys::H);
    menuItem->Action.connect(Bind(&MainFrame::onFindReplace, this, true));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Options/Change Font
    menuItem = mOptionsMenu.add("Change &Font");
    menuItem->Action.connect(Bind(&MainFrame::onChangeFont, this));

    // Options/View EOL
    menuItem = mOptionsMenu.add("View &EOL");
    menuItem->Action.connect(Bind(&MainFrame::onToggleViewEol, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_viewEol, this));

    // Windows/Close
    menuItem = mWindowsMenu.add("&Close\tCtrl+W", Keys::Control | Keys::W);
    menuItem->Action.connect(Bind(&MainFrame::onCloseWindow, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Windows/Duplicate
    menuItem = mWindowsMenu.add("&Duplicate\tCtrl+D", Keys::Control | Keys::D);
    menuItem->Action.connect(Bind(&MainFrame::onDuplicateWindow, this));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Windows/Cascade
    menuItem = mWindowsMenu.add("&Cascade");
    menuItem->Action.connect(Bind(&MdiClient::cascade, &getMdiClient()));
    menuItem->Update.connect(Bind(&MainFrame::onUpdate_getTextEditor_Available, this));

    // Menu bar
    mMenuBar.add(&mFileMenu);
    mMenuBar.add(&mEditMenu);
    mMenuBar.add(&mOptionsMenu);
    mMenuBar.add(&mWindowsMenu);
  }

  //////////////////////////////////////////////////////////////////////
  // signal bindings

  void onNew()
  {
    String title = String("Untitled")+String::fromInt(++mDocCounter);
    addTextEditor(new TextEditor(title, false, this));
  }

  void onOpen()
  {
    OpenFileDialog dlg("Open file", this);
    addFilters(dlg);

    dlg.setMultiselect(true);

    if (dlg.doModal()) {
      // selected file names
      std::vector<String> fileNames = dlg.getFileNames();

      // for each selected file name
      for (std::vector<String>::iterator it=fileNames.begin(); it!=fileNames.end(); ++it) {
	String fileName = *it;

	// check if this file is already opened
	TextEditor *textEditor = getTextEditorByFileName(fileName);

	// active that editor (if exist)
	if (textEditor != NULL) {
	  getMdiClient().activate(textEditor);
	}
	// create the new editor reading the file
	else {
	  textEditor = new TextEditor(fileName, true, this);

	  // open the file
	  if (textEditor->openFile())
	    addTextEditor(textEditor);
	  else {
	    delete textEditor;
	    msgBox("Error opening file '"+fileName+"'",
		   "Error",
		   MB_OK | MB_ICONERROR);
	    break;
	  }
	}
      }
    }
  }

  void onSave()
  {
    saveTextEditor(getTextEditor(), "Save file", false);
  }

  void onUpdate_forSave(MenuItemEvent &ev)
  {
    TextEditor *textEditor = getTextEditor();
    ev.getMenuItem().setEnabled((textEditor != NULL)
				&& (!(textEditor->hasFileName())
				    || (textEditor->getEditor().isModified())));
  }

  void onSaveAs()
  {
    saveTextEditor(getTextEditor(), "Save file as...", true);
  }

  void onUpdate_getTextEditor_Available(MenuItemEvent &ev)
  {
    ev.getMenuItem().setEnabled(getTextEditor() != NULL);
  }

  void onExit()
  {
    CloseEvent ev(this);
    onClose(ev);
    if (!ev.isCanceled())
      dispose();
  }

  void onUndo()
  {
    getTextEditor()->getEditor().undo();
  }

  void onUpdate_canUndo(MenuItemEvent &ev)
  {
    TextEditor *textEditor = getTextEditor();
    ev.getMenuItem().setEnabled((textEditor != NULL)
				&& (textEditor->getEditor().canUndo()));
  }

  void onRedo()
  {
    getTextEditor()->getEditor().redo();
  }

  void onUpdate_canRedo(MenuItemEvent &ev)
  {
    TextEditor *textEditor = getTextEditor();
    ev.getMenuItem().setEnabled((textEditor != NULL)
				&& (textEditor->getEditor().canRedo()));
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
    if (mFindDlg == NULL) {
      mFindDlg = new FindTextDialog(replace, this);
      mFindDlg->FindNext.connect(Bind(&MainFrame::onFindNext, this));
      mFindDlg->Replace.connect(Bind(&MainFrame::onReplace, this));
      mFindDlg->ReplaceAll.connect(Bind(&MainFrame::onReplaceAll, this));
      mFindDlg->Cancel.connect(Bind(&MainFrame::onCancelFind, this));
      mFindDlg->setVisible(true);
    }
    else
      mFindDlg->focus();
  }

  void onFindNext()
  {
    SciEditor &sciEditor(getTextEditor()->getEditor());
    String findWhat = mFindDlg->getFindWhat();
    int flags =
      (mFindDlg->isMatchCase() ? SCFIND_MATCHCASE: 0) |
      (mFindDlg->isWholeWord() ? SCFIND_WHOLEWORD: 0);

    // forward search
    if (mFindDlg->isForward()) {
      sciEditor.goToPos(sciEditor.getSelectionEnd());
      sciEditor.searchAnchor();

      if (!sciEditor.searchNext(flags, findWhat))
	Beep(100, 10);

      int start = sciEditor.getSelectionStart();
      int end = sciEditor.getSelectionEnd();
      sciEditor.goToPos(end);
      sciEditor.setAnchor(start);
    }
    // backward search
    else {
      sciEditor.goToPos(sciEditor.getSelectionStart());
      sciEditor.searchAnchor();

      if (!sciEditor.searchPrev(flags, findWhat))
	Beep(100, 10);

      int start = sciEditor.getSelectionStart();
      int end = sciEditor.getSelectionEnd();
      sciEditor.goToPos(start);
      sciEditor.setAnchor(end);
    }
  }

  void onReplace()
  {
    SciEditor &sciEditor(getTextEditor()->getEditor());
    int start = sciEditor.getSelectionStart();
    int end = sciEditor.getSelectionEnd();

    sciEditor.goToPos(start);

    onFindNext();

    if (start != end &&
	start == sciEditor.getSelectionStart() &&
	end == sciEditor.getSelectionEnd()) {
      sciEditor.replaceSel(mFindDlg->getReplaceWith());
      onFindNext();
    }
  }

  void onReplaceAll()
  {
    SciEditor &sciEditor(getTextEditor()->getEditor());
    do {
      onReplace();
    } while (sciEditor.getSelectionStart() != sciEditor.getSelectionEnd());
  }

  void onCancelFind()
  {
    mFindDlg->deleteAfterEvent();
    mFindDlg = NULL;
  }
  // Find stuff end
  //////////////////////////////////////////////////////////////////////

  void onChangeFont()
  {
    FontDialog dialog(mFont, this);
    if (dialog.doModal()) {
      Widget::Container editors = getMdiClient().getChildren();
      
      for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it)
	static_cast<TextEditor *>(*it)->getEditor().setFont(mFont);
    }
  }

  void onToggleViewEol()
  {
    mViewEol = !mViewEol;

    // set the new state of ViewEol to all editors
    Widget::Container editors = getMdiClient().getChildren();
    for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it)
      static_cast<TextEditor *>(*it)->getEditor().setViewEol(mViewEol);
  }

  void onUpdate_viewEol(MenuItemEvent &ev)
  {
    ev.getMenuItem().setChecked(mViewEol);
  }

  void onCloseWindow()
  {
    TextEditor *textEditor = getTextEditor();
    if (closeTextEditor(textEditor, false))
      delete textEditor;
  }

  void onDuplicateWindow()
  {
    addTextEditor(new TextEditor(*getTextEditor()));
  }

  // when the user press the close button in a MdiChild (TextEditor child)
  void onCloseTextEditor(CloseEvent &ev)
  {
    TextEditor *textEditor = static_cast<TextEditor *>(ev.getWidget());
    // show the warning dialog
    if (closeTextEditor(textEditor, false)) {
      // the user really want to close this...

      // the widget must be deleted but we can't delete it because
      // we're inside an event that was generated by the same widget
      // (see TN006)
      textEditor->deleteAfterEvent();
    }
    // the user cancels the close operation
    else {
      // cancel the close event (don't close the TextEditor)
      ev.cancel();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // events

  // when the user press the close button of the MainFrame we must
  // close all the editors
  virtual void onClose(CloseEvent &ev)
  {
    std::vector<Document *> asked;
    Widget::Container editors = getMdiClient().getChildren();

    // for each children
    for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it) {
      TextEditor *textEditor = static_cast<TextEditor *>(*it);
      Document *document = textEditor->getDocument();

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

    // delete all TextEditors
    for (Widget::Container::iterator it=editors.begin(); it!=editors.end(); ++it) {
      TextEditor *textEditor = static_cast<TextEditor *>(*it);
      delete textEditor;
    }
  }

  //////////////////////////////////////////////////////////////////////
  // logic
private:

  // returns the current text editor
  TextEditor *getTextEditor()
  {
    return static_cast<TextEditor *>(getMdiClient().getActive());
  }

  // adds file filters to the FileDialog
  void addFilters(FileDialog &dlg)
  {
    dlg.addFilter("All (*.*)", "*.*", true);
    dlg.addFilter("Text files (*.txt)", "*.txt");
  }

  // adds a new text editor to the application
  void addTextEditor(TextEditor *textEditor)
  {
    // set the font of the editor to the current selected font
    textEditor->getEditor().setFont(mFont);

    // set the ViewEol state
    textEditor->getEditor().setViewEol(mViewEol);

    // bind the close event
    textEditor->Close.connect(Bind(&MainFrame::onCloseTextEditor, this));
  }

  // Saves the file that is associated with 'textEditor'.  Returns
  // true if the file was saved, or false in case of error or if the
  // user cancels the operation.
  bool saveTextEditor(TextEditor *textEditor, const String &saveDialogTitle,
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
      msgBox("Error saving file '"+textEditor->getFileName()+"'",
	     "Error",
	     MB_OK | MB_ICONERROR);
      return false;
    }
  }

  // returns false if the user don't want to close the text editor
  bool closeTextEditor(TextEditor *textEditor, bool forceAsk)
  {
    if ((textEditor->isLastView() || forceAsk) &&
	textEditor->getEditor().isModified()) {
      int ret = msgBox("Save changes to '"+textEditor->getFileName()+"'?",
		       "TextEditor",
		       MB_YESNOCANCEL | MB_ICONQUESTION);

      if (ret == IDCANCEL) {
	return false;		// "Cancel" in message-box
      }
      else if (ret == IDYES) {
	if (!saveTextEditor(textEditor, "Save file", false))
	  return false;		// "Cancel" in save-file dialog
      }
    }
    return true;
  }

  // returns the first found TextEditor that is editing the "fileName"
  TextEditor *getTextEditorByFileName(const String &fileName)
  {
    Widget::Container listOfTextEditors = getMdiClient().getChildren();
    Widget::Container::iterator it;

    for (it=listOfTextEditors.begin(); it!=listOfTextEditors.end(); ++it) {
      TextEditor *textEditor = static_cast<TextEditor *>(*it);

      if (textEditor->getFileName() == fileName)
	return textEditor;
    }

    return NULL;
  }

};

//////////////////////////////////////////////////////////////////////


class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
