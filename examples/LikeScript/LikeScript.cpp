// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <Vaca/Vaca.h>

using namespace Vaca;

void configure_frame(Frame& frame);
void configure_editor(TextEdit& edit, Font& normalFont, Font& hotFont, int preferredWidth);
void configure_num_editor(TextEdit& edit);
void configure_buttons(Button& ok, Button& cancel);
void filter_num_keys(KeyEvent& ev);
void msg_ok(Widget* owner);

int VACA_MAIN()
{
  Application application;

  // creates a Dialog that looks like a Frame
  Dialog frame(L"Like a script", NULL,
	       Dialog::Styles::Default
	       + Frame::Styles::Resizable
	       + Frame::Styles::Minimizable
	       + Frame::Styles::Maximizable);

  Label firstNameLabel(L"First name:", &frame);
  Label lastNameLabel(L"Last name:", &frame);
  Label ageLabel(L"Age:", &frame);
  TextEdit firstName(L"", &frame);
  TextEdit lastName(L"", &frame);
  TextEdit age(L"", &frame, TextEdit::Styles::Default +
			    TextEdit::Styles::RightAligned);
  Button ok(L"&OK", &frame);
  Button cancel(L"&Cancel", &frame);
  Font font1(L"Courier New", 10);
  Font font2(L"Courier New", 10, FontStyle::Bold);

  try {
    frame.setLayout(Bix::parse(L"Y[XY[%,fx%;%,fx%;%,fxX[%,fX[]]],X[f,exX[%,%]]]",
			       &firstNameLabel, &firstName,
			       &lastNameLabel, &lastName,
			       &ageLabel, &age,
			       &ok, &cancel));
  } catch (ParseException &e) {
    MsgBox::show(&frame, L"Bix::parse Error",
		 convert_to<String>(e.what()));
    return 0;
  }

  configure_frame(frame);
  configure_editor(firstName, font1, font2, 256);
  configure_editor(lastName, font1, font2, 256);
  configure_editor(age, font1, font2, 64);
  configure_num_editor(age);
  configure_buttons(ok, cancel);

  frame.setSize(frame.getPreferredSize());
  frame.center();
  frame.setVisible(true);

  application.run();
  return 0;
}

struct limit_frame_resizing
{
  Frame* f;
  limit_frame_resizing(Frame* f) : f(f) { }
  void operator()(CardinalDirection dir, Rect& rc) {
    Rect bounds = f->getBounds();
    rc = Rect(rc.x, bounds.y, rc.w, bounds.h);
  }
};

void configure_frame(Frame &frame)
{
  frame.Resizing.connect(limit_frame_resizing(&frame));
}

void configure_editor(TextEdit &edit, Font &normalFont, Font &hotFont, int preferredWidth)
{
  edit.FocusEnter.connect(Bind(&Widget::setFont, &edit, hotFont));
  edit.FocusLeave.connect(Bind(&Widget::setFont, &edit, normalFont));

  edit.MouseEnter.connect(Bind(&Widget::setBgColor, &edit, Color(255, 255, 190)));
  edit.MouseLeave.connect(Bind(&Widget::setBgColor, &edit, Color::White));
  edit.MouseEnter.connect(Bind(&Widget::invalidate, &edit, true));
  edit.MouseLeave.connect(Bind(&Widget::invalidate, &edit, true));

  edit.setFont(normalFont);
  edit.setPreferredSize(Size(preferredWidth, edit.getPreferredSize().h));
}

void configure_num_editor(TextEdit &edit)
{
  edit.KeyDown.connect(&filter_num_keys);
}

void configure_buttons(Button& ok, Button& cancel)
{
  ok.Click.connect(Bind<void>(&msg_ok, ok.getParent()));
  ok.Click.connect(Bind(&Widget::setVisible, ok.getParent(), false));
  cancel.Click.connect(Bind(&Widget::setVisible, cancel.getParent(), false));
}

void filter_num_keys(KeyEvent &ev)
{
  if ((ev.getCharCode() != 0) &&
      !(ev.getCharCode() >= '0' && ev.getCharCode() <= '9') &&
      (ev.getCharCode() >= ' ')) {
    ev.consume();
  }
}

void msg_ok(Widget* owner)
{
  MsgBox::show(owner, L"OK", L"You press the OK button");
}
