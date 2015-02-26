// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

// we include the entire Vaca namespace, in this way we don't worry
// about what classes are accesible: we have all the classes available
#include <vaca/vaca.h>

// here is defined IDI_VACA = 1
#include "../resource.h"

// we can use the Vaca namespace by default to avoid writting things
// like vaca::Frame, vaca::LinkLabel, vaca::Font, etc.
using namespace vaca;

// the main window
class MainFrame : public Frame
{
  // a label to represent a link to Internet
  LinkLabel m_label;

  // a font to use with the link
  Font m_linkFont;

public:

  // constructs the main frame
  MainFrame()
    // call the parent class constructor, with the title "HelloWorld"
    // why we use L"..."? because Vaca strings are wide character to support
    // Unicode, so you have to use the L prefix in all string literals
    : Frame(L"HelloWorld")
    // construct the link with the URL and the label to show,
    // every Widget receive the parent, in this case, "this" pointer
    , m_label(L"http://vaca.sourceforge.net/", L"Hello World!", this)
    // construct a font to use in the link label
    , m_linkFont(L"Verdana", 14)
  {
    // set the frame icon
    setIcon(ResourceId(IDI_VACA));

    // set the layout to the client area, because the link'll use the
    // entire client area
    setLayout(new ClientLayout);

    // set the horizontal label text alignment to the center
    m_label.setTextAlign(TextAlign::Center);

    // set the link font to the loaded font
    m_label.setFont(m_linkFont);

    // finally we set the main frame's size to its preferred size
    setSize(getPreferredSize());
  }

};

// the example application, every Vaca application must to have one
// instance of Application (or a class derived from it)
class Example : public Application
{
  // the main frame
  MainFrame m_mainFrame;

  // it's called when we use Application::run...
  virtual void main() {
    // at this point the frame is created (the constructor was called
    // from the Example constructor)...

    // ...so we can make it visible to the user
    m_mainFrame.setVisible(true);

    // After Application::main the Application::doMessageLoop method
    // is called automatically. That routine gets and dispatches
    // messages from/to any visible Frame.
  }

};

// the WinMain, every Windows application should have this function
int VACA_MAIN()
{
  try {
    // create the example application
    Example app;

    // run the application (calling Example::main)
    app.run();

    // the destructor of "Example" is called here
  }
  catch (Exception& e) {
    // if some errors come from Vaca show it (this method shows the
    // error as a MsgBox)
    MsgBox::showException(NULL, e);
  }
  return 0;
}
