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

#include "ToroTab.h"
#include "View.h"

ToroTab::ToroTab(Widget *parent)
  : TabBase(parent)
{
}

void ToroTab::addView(View *view)
{
  assert(view != NULL);

  mViews.push_back(view);
  
  addPage(view->getTitle());

  if (!isVisible()) {
    setVisible(true);
    getParent()->layout();
  }
}

void ToroTab::removeView(View *view)
{
  int index = convertViewToIndex(view);
  
  removePage(index);
  mViews.erase(mViews.begin()+index);

  if (getPageCount() == 0) {
    setVisible(false);
    getParent()->layout();
  }
}

void ToroTab::selectView(View *view)
{
  int index = convertViewToIndex(view);

  setActivePage(index);
}

void ToroTab::onPageChange(Event &ev)
{
  TabBase::onPageChange(ev);

  int index = getActivePage();

  assert(index >= 0 && index < static_cast<int>(mViews.size()));

  SelectView(mViews[index]);
}

int ToroTab::convertViewToIndex(View *view)
{
  assert(view != NULL);

  int index = 0;
  
  for (std::vector<View *>::iterator
	 it = mViews.begin(); it != mViews.end(); ++it) {
    if (*it == view)
      return index;

    index++;
  }

  assert(false);
}

void ToroTab::onPreferredSize(Size &sz)
{
  sz = Size(0, ScreenGraphics().measureString("").h+8);
}
