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

#include "Vaca/ClientLayout.h"
#include "OutputBar.h"

OutputBar::OutputBar(Frame *parent)
  : DockBar("Output", parent)
  , mOutputTab(this)
  , mCompilePage("Compile", &mOutputTab)
  , mShellPage("Shell", &mOutputTab)
  , mCompile(&mCompilePage)
  , mShell(&mShellPage)
{
  setLayout(new ClientLayout);
  mCompilePage.setLayout(new ClientLayout);
  mShellPage.setLayout(new ClientLayout);
}

Side OutputBar::getGripperSide(bool docked, Side dockSide)
{
  return TopSide;
}

Size OutputBar::getDockedSize(Side side)
{
  DockArea *dockArea = getOwnerFrame()->getDockArea(side);
  assert(dockArea != NULL);
  
  Size sz = Size(200, 200);

  switch (side) {
    case LeftSide:
    case RightSide:
      sz = sz.createUnion(Size(0, dockArea->getBounds().h));
      break;
    case TopSide:
    case BottomSide:
      sz = sz.createUnion(Size(dockArea->getBounds().w, 0));
      break;
  }

  return sz;
}

Size OutputBar::getFloatingSize()
{
  return DockBar::getFloatingSize();
//   return setSize(Size(300, 300));
}

void OutputBar::layout()
{
  DockBar::layout();

  // TODO
}
