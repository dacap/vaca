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

#ifndef TORO_TOROFRAME_H
#define TORO_TOROFRAME_H

#include "Vaca/Mdi.h"
#include "Vaca/Command.h"

using namespace Vaca;

class Document;
class View;
class ToroTab;

class ToroFrame : public MdiFrame
{
  ToroTab *mTabs;
  int mDocCounter;

public:

  ToroFrame();
  virtual ~ToroFrame();

  View *addDocument(Document *doc);

  View *getActiveView();
  void setActiveView(View *view);
  std::vector<View *> getAllViews();

protected:

  void onFileNewText();
  void onFileNewForm();
  void onFileNewProject();
  void onFileOpen();
  void onFileClose();
  void onFileSave();
  void onFileSaveAs();
  void onFileSaveAll();
  void onFileExit();
  void onEditUndo();
  void onEditRedo();
  void onEditCut();
  void onEditCopy();
  void onEditPaste();
  void onEditClear();
  void onEditFind();
  void onEditReplace();
  void onWindowClose();
  void onWindowDuplicate();
  void onWindowDuplicateUpdate(CommandState &cmdState);
  void onWindowCascade();
  
private:

  void createCommands();
  void onSelectViewFrame();
  void onSelectViewTab(View *view);
  void onCloseViewFrame(View *view);

  void onNotImplemented(CommandState &cmdState);

};

#endif
