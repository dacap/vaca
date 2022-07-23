// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "../resource.h"

using namespace vaca;

//////////////////////////////////////////////////////////////////////
// EditableListBox

class EditableListBox : public ListBox
{
  TextEdit* m_edit;
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
      beginEdit(getSelectedItem());
      ev.consume();
    }
  }

  // When the user double-clicks in a item of the ListBox
  virtual void onItemDoubleClick(Event &ev)
  {
    ListBox::onItemDoubleClick(ev);
    beginEdit(getSelectedItem());
  }

  virtual void onResize(ResizeEvent& ev)
  {
    endEdit();
    ListBox::onResize(ev);
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
      onEditFocusLeave();
  }

  void onEditFocusLeave()
  {
    endEdit();
  }

  void beginEdit(int index)
  {
    if (index >= 0 && m_edit == NULL) {
      // create an Edit widget without client-edge
      m_edit = new TextEdit(getItemText(index), this, TextEdit::Styles::Default -
						      Widget::Styles::ClientEdge);
      m_edit->KeyDown.connect(&EditableListBox::onEditKeyDown, this);
      m_edit->FocusLeave.connect([this]{ onEditFocusLeave(); });
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
      // times this destruction process because...
      int editingItem = m_editingItem;
      TextEdit *edit = m_edit;
      m_edit = NULL;

      // ...this could produce a "onBeforeBoundsChange" event
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
  Widget m_bottomPanel;
  Button m_addButton;
  Button m_removeButton;
  int m_counter;

public:

  MainFrame()
    : Frame(L"EditableListBox")
    , m_listBox(this)
    , m_bottomPanel(this)
    , m_addButton(L"Add", &m_bottomPanel)
    , m_removeButton(L"Remove", &m_bottomPanel)
    , m_counter(0)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false)); // no-homogeneous
    m_listBox.setConstraint(new BoxConstraint(true)); // expansive
    m_bottomPanel.setLayout(new BoxLayout(Orientation::Horizontal, true, 0)); // homogeneous, border=0

    for (int c=0; c<10; c++)
      onAdd();

    m_addButton.Click.connect([this]{ onAdd(); });
    m_removeButton.Click.connect([this]{ onRemove(); });

    setSize(getPreferredSize());
    center();
  }

private:

  void onAdd()
  {
    int index = m_listBox.addItem(format_string(L"Item number %d", ++m_counter));
    m_listBox.setSelectedItem(index);
    m_listBox.requestFocus();
  }

  void onRemove()
  {
    int index = m_listBox.getSelectedItem();
    if (index >= 0) {
      m_listBox.removeItem(m_listBox.getSelectedItem());
    }

    m_listBox.setSelectedItem(index);
    if (m_listBox.getSelectedItem() < 0)
      m_listBox.setSelectedItem(m_listBox.getItemCount()-1);

    m_listBox.requestFocus();
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
