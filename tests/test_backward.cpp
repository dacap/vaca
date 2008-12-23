#include <cassert>

#include "Vaca/Vaca.h"
#include "Vaca/backward.h"

using namespace Vaca;

void test_styles()
{
  assert(BandedDockArea::Styles::Default == BandedDockAreaStyle);
  assert(BasicDockArea::Styles::Default == BasicDockAreaStyle);
  assert(Button::Styles::Default == ButtonStyle);
  assert(CheckBox::Styles::Default == CheckBoxStyle);
  assert(ComboBox::Styles::Default == ComboBoxStyle);
  assert(CustomButton::Styles::Default == CustomButtonStyle);
  assert(CustomLabel::Styles::Default == CustomLabelStyle);
  assert(Dialog::Styles::Default == DialogStyle);
  assert(DockArea::Styles::Default == DockAreaStyle);
  assert(DockBar::Styles::Default == DockBarStyle);
  assert(DockFrame::Styles::Default == DockFrameStyle);
  assert(Frame::Styles::Default == FrameStyle);
  assert(GroupBox::Styles::Default == GroupBoxStyle);
  assert(Label::Styles::Default == LabelStyle);
  assert(LinkLabel::Styles::Default == LinkLabelStyle);
  assert(ListBox::Styles::Default == ListBoxStyle);
  assert(ListView::Styles::Default == ListViewStyle);
  assert(MdiChild::Styles::Default == MdiChildStyle);
  assert(MdiClient::Styles::Default == MdiClientStyle);
  assert(MdiFrame::Styles::Default == MdiFrameStyle);
  assert(Panel::Styles::Default == PanelStyle);
  assert(ProgressBar::Styles::Default == ProgressBarStyle);
  assert(RadioButton::Styles::Default == RadioButtonStyle);
  assert(SciEdit::Styles::Default == SciEditorStyle);
  assert(Separator::Styles::Default == SeparatorStyle);
  assert(Slider::Styles::Default == SliderStyle);
  assert(SpinButton::Styles::Default == SpinButtonStyle);
  assert(Spinner::Styles::Default == SpinnerStyle);
  assert(StatusBar::Styles::Default == StatusBarStyle);
  assert(TabBase::Styles::Default == TabBaseStyle);
  assert(Tab::Styles::Default == TabStyle);
  assert(TabPage::Styles::Default == TabPageStyle);
  assert(TextEdit::Styles::Default == EditStyle);
  assert(ToggleButton::Styles::Default == ToggleButtonStyle);
  assert(ToolSet::Styles::Default == ToolSetStyle);
  assert(ToolBar::Styles::Default == ToolBarStyle);
  assert(TreeView::Styles::Default == TreeViewStyle);
}

void test_textedit()
{
  Frame frm("test");
  TextEdit e1("e1", &frm);
  TextEdit e2("e2", &frm, TextEdit::Styles::Password);
  TextEdit e3("e3", &frm, TextEdit::Styles::TextArea);
  Edit be1("e1", &frm);
  PasswordEdit be2("e2", &frm);
  MultilineEdit be3("e3", &frm);

  assert(e1.getText() == be1.getText());
  assert(e2.getText() == be2.getText());
  assert(e3.getText() == be3.getText());

  assert(e1.getStyle() == be1.getStyle());
  assert(e2.getStyle() == be2.getStyle());
  assert(e3.getStyle() == be3.getStyle());
}

int main()
{
  Application app;
  test_styles();
  test_textedit();
  return 0;
}
