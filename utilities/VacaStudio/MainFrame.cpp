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

#include "Vaca/System.h"
#include "Vaca/Bind.h"
#include "Vaca/ClientLayout.h"
#include "MainFrame.h"
#include "FRFiles.h"

using namespace Vaca;

MainFrame::MainFrame()
//   : MdiFrame("VacaStudio", NULL,
// 	     MdiFrameStyle + InitiallyMaximizedFrameStyle)
  : Frame("VacaStudio", NULL,
	  FrameStyle + InitiallyMaximizedFrameStyle)
  , mFileMenu("&File")
  , mToolsMenu("&Tools")
  , mOutputBar(this)
{
  setBgColor(Color::Red);
  
  //////////////////////////////////////////////////////////////////////
  // layout

  setLayout(new ClientLayout);
  setMenuBar(&mMenuBar);

  //////////////////////////////////////////////////////////////////////
  // menus

  MenuItem *menuItem;

  menuItem = mFileMenu.add("&New");
  menuItem->Action.connect(Bind(&MainFrame::onNewFile, this));

  menuItem = mFileMenu.add("&Exit");
  menuItem->Action.connect(Bind(&MainFrame::onExit, this));

  menuItem = mToolsMenu.add("&Find/Replace in Files");
  menuItem->Action.connect(Bind(&MainFrame::onFindReplaceInFiles, this));

  mMenuBar.add(&mFileMenu);
  mMenuBar.add(&mToolsMenu);

  //////////////////////////////////////////////////////////////////////
  // dock bars

  defaultDockAreas();
  mOutputBar.dockIn(getDockArea(BottomSide));
}

void MainFrame::onNewFile()
{
//   new MdiChild("New file", this);
}

void MainFrame::onExit()
{
  dispose();
}

void MainFrame::onFindReplaceInFiles()
{
  FRFiles dlg(this);
  dlg.doModal();
}
