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
#include "resource.h"

#include "ToroFrame.h"
#include "ToroTab.h"
#include "TextDocument.h"
#include "FormDocument.h"
#include "View.h"

using namespace Vaca;

ToroFrame::ToroFrame()
  : MdiFrame("Toro", NULL, MdiFrameStyle + InitiallyMaximizedFrameStyle, true)
  , mTabs(new ToroTab(this))
{
  setIcon(IDI_TORO);
  
  mDocCounter = 0;

  setMdiClient(new MdiClient(this));
  setMenuBar(new MenuBar(IDM_MENUBAR));
  //     setLayout(new ClientLayout);
  // setIcon(IDI_VACA);

  setLayout(new BoxLayout(Vertical, false));
  getMdiClient()->setConstraint(new BoxConstraint(true));
//   getMdiClient()->removeStyle(ClientEdgeStyle);

  //     mTabs->addPage("Welcome to Toro");
  //     mTabs->bringToTop();

  //     MdiChild *child = new MdiChild("Welcome to Toro", this);
  //     ToroBrowser *browser = new ToroBrowser(child);
  //     child->setLayout(new ClientLayout);
  //     getMdiClient()->activate(child);

  createCommands();
  

  mTabs->SelectView.connect(Bind(&ToroFrame::onSelectViewTab, this));

  onFileNewText();
//   onFileNewForm();
}

ToroFrame::~ToroFrame()
{
  delete mTabs;

  // delete all Views
  Widget::Container views = getMdiClient()->getChildren();

  for (Widget::Container::iterator it=views.begin(); it!=views.end(); ++it) {
    View *view = dynamic_cast<View *>(*it);
    delete_widget(view);
  }
  
}

void ToroFrame::createCommands()
{
  Command *command;

  command = new Command(IDM_FILE_NEW_TEXT);
  command->Action.connect(Bind(&ToroFrame::onFileNewText, this));
  addCommand(command);

  command = new Command(IDM_FILE_NEW_FORM);
  command->Action.connect(Bind(&ToroFrame::onFileNewForm, this));
  addCommand(command);

  command = new Command(IDM_FILE_NEW_PROJECT);
  command->Action.connect(Bind(&ToroFrame::onFileNewProject, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_FILE_OPEN);
  command->Action.connect(Bind(&ToroFrame::onFileOpen, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_FILE_CLOSE);
  command->Action.connect(Bind(&ToroFrame::onFileClose, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_FILE_SAVE);
  command->Action.connect(Bind(&ToroFrame::onFileSave, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_FILE_SAVE_AS);
  command->Action.connect(Bind(&ToroFrame::onFileSaveAs, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_FILE_SAVE_ALL);
  command->Action.connect(Bind(&ToroFrame::onFileSaveAll, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_FILE_EXIT);
  command->Action.connect(Bind(&ToroFrame::onFileExit, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_UNDO);
  command->Action.connect(Bind(&ToroFrame::onEditUndo, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_REDO);
  command->Action.connect(Bind(&ToroFrame::onEditRedo, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_CUT);
  command->Action.connect(Bind(&ToroFrame::onEditCut, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_COPY);
  command->Action.connect(Bind(&ToroFrame::onEditCopy, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_PASTE);
  command->Action.connect(Bind(&ToroFrame::onEditPaste, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_CLEAR);
  command->Action.connect(Bind(&ToroFrame::onEditClear, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_FIND);
  command->Action.connect(Bind(&ToroFrame::onEditFind, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_EDIT_REPLACE);
  command->Action.connect(Bind(&ToroFrame::onEditReplace, this));
  command->Update.connect(Bind(&ToroFrame::onNotImplemented, this));
  addCommand(command);

  command = new Command(IDM_WINDOW_CLOSE);
  command->Action.connect(Bind(&ToroFrame::onWindowClose, this));
  addCommand(command);

  command = new Command(IDM_WINDOW_DUPLICATE);
  command->Action.connect(Bind(&ToroFrame::onWindowDuplicate, this));
  command->Update.connect(Bind(&ToroFrame::onWindowDuplicateUpdate, this));
  addCommand(command);

  command = new Command(IDM_WINDOW_CASCADE);
  command->Action.connect(Bind(&ToroFrame::onWindowCascade, this));
  addCommand(command);
}

View *ToroFrame::getActiveView()
{
  MdiChild *mdiChild = getMdiClient()->getActive();

  if (mdiChild != NULL)
    return dynamic_cast<View *>(mdiChild);
  else
    return NULL;
}

void ToroFrame::setActiveView(View *view)
{
  getMdiClient()->activate(view);
}

View *ToroFrame::addDocument(Document *doc)
{
//   mDocuments.push_back(doc);

  View *view = doc->createView(this);
  view->Activate.connect(Bind(&ToroFrame::onSelectViewFrame, this));
  view->Close.connect(Bind(&ToroFrame::onCloseViewFrame, this, view));

  mTabs->addView(view);
  setActiveView(view);

  refreshMenuBar();

  return view;
}

void ToroFrame::onFileNewText()
{
  mDocCounter++;
  addDocument(new TextDocument("Text" + String::fromInt(mDocCounter)));
}

void ToroFrame::onFileNewForm()
{
  mDocCounter++;
  addDocument(new FormDocument("Form" + String::fromInt(mDocCounter)));
}

void ToroFrame::onFileNewProject()
{
  mDocCounter++;
  addDocument(new Document("Project" + String::fromInt(mDocCounter)));
}

void ToroFrame::onFileOpen()
{
}

/**
 * Closes all the View related with the active document.
 */
void ToroFrame::onFileClose()
{
}

void ToroFrame::onFileSave()
{
}

void ToroFrame::onFileSaveAs()
{
}

void ToroFrame::onFileSaveAll()
{
}

void ToroFrame::onFileExit()
{
  setVisible(false);
}

void ToroFrame::onEditUndo()
{
}

void ToroFrame::onEditRedo()
{
}

void ToroFrame::onEditCut()
{
}

void ToroFrame::onEditCopy()
{
}

void ToroFrame::onEditPaste()
{
}

void ToroFrame::onEditClear()
{
}

void ToroFrame::onEditFind()
{
}

void ToroFrame::onEditReplace()
{
}

/**
 * Closes the current View (just one view, the file could have two or
 * more views).
 */
void ToroFrame::onWindowClose()
{
  View *view = getActiveView();

  assert(view != NULL);
  
  view->setVisible(false);
  onCloseViewFrame(view);
}

void ToroFrame::onWindowDuplicate()
{
  View *view = getActiveView();

  assert(view != NULL);
  assert(view->canDuplicate());

  view->duplicate();
}

void ToroFrame::onWindowDuplicateUpdate(CommandState &cmdState)
{
  View *view = getActiveView();

  cmdState.setEnabled((view != NULL) && (view->canDuplicate()));
}

void ToroFrame::onWindowCascade()
{
  getMdiClient()->cascade();
}

// std::vector<Document *> ToroFrame::getDocuments()
// {
//   return mDocuments;
// }

/** 
 * When a View frame is activated we must to select the corresponding
 * TabPage in the ToroTab.
 */
void ToroFrame::onSelectViewFrame()
{
  mTabs->selectView(getActiveView());
}

/** 
 * When a TabPage is selected in ToroTab we must to activate the
 * corresponding View.
 * 
 * @param view 
 */
void ToroFrame::onSelectViewTab(View *view)
{
  setActiveView(view);
}

void ToroFrame::onCloseViewFrame(View *view)
{
  mTabs->removeView(view);
  delete_widget(view);
}

void ToroFrame::onNotImplemented(CommandState &cmdState)
{
  cmdState.setEnabled(false);
}
