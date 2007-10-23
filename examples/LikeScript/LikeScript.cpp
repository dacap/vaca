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

#include <Vaca/Vaca.h>

using namespace Vaca;

void configure_frame(Frame &frame);
void configure_editor(Edit &edit, Font &normalFont, Font &hotFont, int preferredWidth);
void configure_num_editor(Edit &edit);
void limit_frame_resizing(Frame &frame, int edge, Rect &rc);
void filter_num_keys(KeyEvent &ev);

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  Application application;

  // creates a Dialog that looks like a Frame
  Dialog frame("Like a script", NULL,
	       DialogStyle
	       + ResizableFrameStyle
	       + MinimizableFrameStyle
	       + MaximizableFrameStyle);

  Label firstNameLabel("First name:", &frame);
  Label lastNameLabel("Last name:", &frame);
  Label ageLabel("Age:", &frame);
  Edit firstName("", &frame);
  Edit lastName("", &frame);
  Edit age("", &frame, EditStyle + RightAlignedEditStyle);
  Button ok("&OK", &frame);
  Button cancel("&Cancel", &frame);
  Font font1("Courier New", 10);
  Font font2("Courier New", 10, FontStyle::Bold);

  try {
    frame.setLayout(Bix::parse("Y[XY[%,fx%;%,fx%;%,fxX[%,fX[]]],X[f,exX[%,%]]]",
			       &firstNameLabel, &firstName,
			       &lastNameLabel, &lastName,
			       &ageLabel, &age,
			       &ok, &cancel));
  } catch (ParseException &e) {
    frame.msgBox(e.what(), "Error in the Bix::format string", MB_OK);
    return 0;
  }

  configure_frame(frame);
  configure_editor(firstName, font1, font2, 256);
  configure_editor(lastName, font1, font2, 256);
  configure_editor(age, font1, font2, 64);

  configure_num_editor(age);

  frame.setSize(frame.getPreferredSize());
  frame.center();
  frame.setVisible(true);

  application.run();
  return 0;
}


void configure_frame(Frame &frame)
{
  using namespace boost; // because 'bind', 'ref', '_1' and '_2' are in boost namespace
  frame.Resizing.connect(bind(&limit_frame_resizing, ref(frame), _1, _2));
}


void configure_editor(Edit &edit, Font &normalFont, Font &hotFont, int preferredWidth)
{
  edit.GotFocus.connect  (Bind(&Widget::setFont, &edit, &hotFont));
  edit.LostFocus.connect (Bind(&Widget::setFont, &edit, &normalFont));

  edit.MouseEnter.connect(Bind(&Widget::setBgColor, &edit, Color(255, 255, 240)));
  edit.MouseLeave.connect(Bind(&Widget::setBgColor, &edit, Color::White));
  edit.MouseEnter.connect(Bind(&Widget::invalidate, &edit, true));
  edit.MouseLeave.connect(Bind(&Widget::invalidate, &edit, true));

  edit.setFont(&normalFont);
  edit.setPreferredSize(Size(preferredWidth, edit.getPreferredSize().h));
}


void configure_num_editor(Edit &edit)
{
  edit.KeyTyped.connect(&filter_num_keys);
}

// limit to horizontal resizing only
void limit_frame_resizing(Frame &frame, int edge, Rect &rc)
{
  Rect bounds = frame.getBounds();
  rc = Rect(rc.x, bounds.y, rc.w, bounds.h);
}

void filter_num_keys(KeyEvent &ev)
{
  if (!(ev.getCharCode() >= '0' && ev.getCharCode() <= '9') &&
      (ev.getCharCode() >= ' ')) {
    ev.consume();
  }
}
