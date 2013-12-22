// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

// This class is a little widget to simulate a "console".
// It is mainly used to show messages in some examples.
class Console : public TextEdit
{
public:

  Console(Widget* parent)
    : TextEdit(L"", parent, TextEdit::Styles::TextArea +
			    TextEdit::Styles::ReadOnly +
			    Widget::Styles::Scroll)
  {
    setFont(Font(L"Courier New", 10));
    setBgColor(Color::White);
    setFgColor(Color(0, 100, 0));
  }

  void println(const String& str)
  {
    // append the text to the end
    setText(getText() + str + L"\r\n");

    // scroll to the end
    scrollLines(getLineCount());
  }
  
};
