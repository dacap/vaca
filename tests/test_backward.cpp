#include <cassert>

#include "Vaca/Vaca.h"
#include "Vaca/backward.h"

using namespace Vaca;

void test_styles()
{
  Style sub(WS_CHILD, 0);

  assert(BandedDockArea::Styles::Default == BandedDockAreaStyle-sub);
  assert(BasicDockArea::Styles::Default == BasicDockAreaStyle-sub);
  assert(Button::Styles::Default == ButtonStyle-sub);
  assert(CheckBox::Styles::Default == CheckBoxStyle-sub);
  assert(ComboBox::Styles::Default == ComboBoxStyle-sub);
  assert(CustomButton::Styles::Default == CustomButtonStyle-sub);
  assert(CustomLabel::Styles::Default == CustomLabelStyle-sub);
  assert(Dialog::Styles::Default == DialogStyle-sub);
  assert(DockArea::Styles::Default == DockAreaStyle-sub);
  assert(DockBar::Styles::Default == DockBarStyle-sub);
  assert(DockFrame::Styles::Default == DockFrameStyle-sub);
  assert(Frame::Styles::Default == FrameStyle-sub);
  assert(GroupBox::Styles::Default == GroupBoxStyle-sub);
  assert(Label::Styles::Default == LabelStyle-sub);
  assert(LinkLabel::Styles::Default == LinkLabelStyle-sub);
  assert(ListBox::Styles::Default == ListBoxStyle-sub);
  assert(ListView::Styles::Default == ListViewStyle-sub);
  assert(MdiChild::Styles::Default == MdiChildStyle);
  assert(MdiClient::Styles::Default == MdiClientStyle-sub);
  assert(MdiFrame::Styles::Default == MdiFrameStyle-sub);
  assert(Panel::Styles::Default == PanelStyle-sub);
  assert(ProgressBar::Styles::Default == ProgressBarStyle-sub);
  assert(RadioButton::Styles::Default == RadioButtonStyle-sub);
  assert(SciEdit::Styles::Default == SciEditorStyle-sub);
  assert(Separator::Styles::Default == SeparatorStyle-sub);
  assert(Slider::Styles::Default == SliderStyle-sub);
  assert(SpinButton::Styles::Default == SpinButtonStyle-sub);
  assert(Spinner::Styles::Default == SpinnerStyle-sub);
  assert(StatusBar::Styles::Default == StatusBarStyle-sub);
  assert(TabBase::Styles::Default == TabBaseStyle-sub);
  assert(Tab::Styles::Default == TabStyle-sub);
  assert(TabPage::Styles::Default == TabPageStyle-sub);
  assert(TextEdit::Styles::Default == EditStyle-sub);
  assert(ToggleButton::Styles::Default == ToggleButtonStyle-sub);
  assert(ToolSet::Styles::Default == ToolSetStyle-sub);
  assert(ToolBar::Styles::Default == ToolBarStyle-sub);
  assert(TreeView::Styles::Default == TreeViewStyle-sub);
}

void test_textedit()
{
  Frame frm(L"test");
  TextEdit e1(L"e1", &frm);
  TextEdit e2(L"e2", &frm, TextEdit::Styles::Password);
  TextEdit e3(L"e3", &frm, TextEdit::Styles::TextArea);
  Edit be1(L"e1", &frm);
  PasswordEdit be2(L"e2", &frm);
  MultilineEdit be3(L"e3", &frm);

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
