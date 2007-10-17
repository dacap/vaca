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

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// EditableListBox

class EditableListBox : public ListBox
{
  Edit *mEdit;
  int mEditingItem;

public:

  EditableListBox(Widget *parent)
    : ListBox(parent)
    , mEdit(NULL)
  {
  }

protected:

  virtual void onKeyDown(KeyEvent &ev)
  {
    ListBox::onKeyDown(ev);
    if (ev.getKeyCode() == Keys::Enter)
      beginEdit(getCurrentItem());
  }

  virtual void onAction(Event &ev)
  {
    ListBox::onAction(ev);
    beginEdit(getCurrentItem());
  }

  virtual void onBeforePosChange()
  {
    if (mEdit != NULL)
      delete endEdit();
  }

  virtual void onScroll(Orientation orientation, int code)
  {
//     if (orientation == Vertical)
    if (mEdit != NULL)
      delete endEdit();
  }

private:

  void onEditKeyDown(KeyEvent &ev)
  {
    if (ev.getKeyCode() == Keys::Enter)
      onEditLostFocus();
  }

  void onEditLostFocus()
  {
    if (mEdit != NULL)
      endEdit()->deleteAfterEvent();
  }

  void beginEdit(int index)
  {
    if (index >= 0 && mEdit == NULL) {
      // create an Edit widget without client-edge
      mEdit = new Edit(getItemText(index), this, EditStyle - ClientEdgeStyle);
      mEdit->KeyDown.connect(Bind(&EditableListBox::onEditKeyDown, this));
      mEdit->LostFocus.connect(Bind(&EditableListBox::onEditLostFocus, this));
      mEdit->selectAll();
      mEdit->focus();

      mEditingItem = index;

      layoutEdit();
    }
  }

  Edit *endEdit()
  {
    setItemText(mEditingItem, mEdit->getText());

    Edit *widget = mEdit;
    mEdit = NULL;
    return widget;
  }

  void layoutEdit()
  {
    if (mEdit != NULL)
      mEdit->setBounds(getItemBounds(mEditingItem));
  }

};

//////////////////////////////////////////////////////////////////////
// MainFrame

class MainFrame : public Frame
{
  EditableListBox mListBox;
  Panel mBottomPanel;
  Button mAddButton;
  Button mRemoveButton;
  int mCounter;

public:

  MainFrame()
    : Frame("EditableListBox")
    , mListBox(this)
    , mBottomPanel(this)
    , mAddButton("Add", &mBottomPanel)
    , mRemoveButton("Remove", &mBottomPanel)
    , mCounter(0)
  {
    setLayout(new BoxLayout(Vertical, false)); // no-homogeneous
    mListBox.setConstraint(new BoxConstraint(true)); // expansive
    mBottomPanel.setLayout(new BoxLayout(Horizontal, true, 0)); // homogeneous, border=0

    for (int c=0; c<10; c++)
      onAdd();

    mAddButton.Action.connect(Bind(&MainFrame::onAdd, this));
    mRemoveButton.Action.connect(Bind(&MainFrame::onRemove, this));

    setSize(preferredSize());
  }

private:

  void onAdd()
  {
    int index = mListBox.addItem("Item number "+String::fromInt(++mCounter));
    mListBox.setCurrentItem(index);
    mListBox.focus();
  }

  void onRemove()
  {
    int index = mListBox.getCurrentItem();
    if (index >= 0)
      mListBox.removeItem(mListBox.getCurrentItem());

    mListBox.setCurrentItem(index);
    if (mListBox.getCurrentItem() < 0)
      mListBox.setCurrentItem(mListBox.getItemCount()-1);

    mListBox.focus();
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
