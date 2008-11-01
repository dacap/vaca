// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

#include <Vaca/Vaca.h>

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// EditableListBox

class EditableListBox : public ListBox
{
  Edit *m_edit;
  int m_editingItem;

public:

  EditableListBox(Widget* parent)
    : ListBox(parent)
  {
    m_edit = NULL;
  }

protected:

  virtual void onKeyDown(KeyEvent &ev)
  {
    ListBox::onKeyDown(ev);
    if (ev.getKeyCode() == Keys::Enter) {
      beginEdit(getCurrentItem());
      ev.consume();
    }
  }

  virtual void onAction(Event &ev)
  {
    ListBox::onAction(ev);
    beginEdit(getCurrentItem());
  }

  virtual void onBeforePosChange()
  {
    ListBox::onBeforePosChange();
    endEdit();
  }

  virtual void onScroll(ScrollEvent& ev)
  {
    ListBox::onScroll(ev);
    endEdit();
  }

private:

  void onEditKeyDown(KeyEvent &ev)
  {
    if (ev.getKeyCode() == Keys::Enter)
      onEditLostFocus(ev);
  }

  void onEditLostFocus(Event& ev)
  {
    endEdit();
  }

  void beginEdit(int index)
  {
    if (index >= 0 && m_edit == NULL) {
      // create an Edit widget without client-edge
      m_edit = new Edit(getItemText(index), this, EditStyle - ClientEdgeStyle);
      m_edit->KeyDown.connect(&EditableListBox::onEditKeyDown, this);
      m_edit->LostFocus.connect(&EditableListBox::onEditLostFocus, this);
      m_edit->selectAll();
      m_edit->requestFocus();

      m_editingItem = index;

      layoutEdit();
    }
  }

  void endEdit()
  {
    if (m_edit != NULL) {
      // we need to reset the m_edit variable to avoid calling two
      // times this destruction process
      int editingItem = m_editingItem;
      Edit *edit = m_edit;
      m_edit = NULL;

      // this could produce a "onBeforePosChange" event
      setItemText(editingItem, edit->getText());

      // delete the edit control
      delete_widget(edit);
    }
  }

  void layoutEdit()
  {
    m_edit->setBounds(getItemBounds(m_editingItem));
  }

};

//////////////////////////////////////////////////////////////////////
// MainFrame

class MainFrame : public Frame
{
  EditableListBox m_listBox;
  Panel m_bottomPanel;
  Button m_addButton;
  Button m_removeButton;
  int m_counter;

public:

  MainFrame()
    : Frame("EditableListBox")
    , m_listBox(this)
    , m_bottomPanel(this)
    , m_addButton("Add", &m_bottomPanel)
    , m_removeButton("Remove", &m_bottomPanel)
    , m_counter(0)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false)); // no-homogeneous
    m_listBox.setConstraint(new BoxConstraint(true)); // expansive
    m_bottomPanel.setLayout(new BoxLayout(Orientation::Horizontal, true, 0)); // homogeneous, border=0

    for (int c=0; c<10; c++)
      onAdd();

    m_addButton.Action.connect(Bind(&MainFrame::onAdd, this));
    m_removeButton.Action.connect(Bind(&MainFrame::onRemove, this));

    setSize(getPreferredSize());
    center();
  }

private:

  void onAdd()
  {
    int index = m_listBox.addItem("Item number "+String::fromInt(++m_counter));
    m_listBox.setCurrentItem(index);
    m_listBox.requestFocus();
  }

  void onRemove()
  {
    int index = m_listBox.getCurrentItem();
    if (index >= 0) {
      m_listBox.removeItem(m_listBox.getCurrentItem());
    }

    m_listBox.setCurrentItem(index);
    if (m_listBox.getCurrentItem() < 0)
      m_listBox.setCurrentItem(m_listBox.getItemCount()-1);

    m_listBox.requestFocus();
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main() {
    m_mainFrame.setVisible(true);
  }
};

int VACA_MAIN()
{
  Example app;
  app.run();
  return 0;
}
