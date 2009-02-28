// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
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
// * Neither the name of the author nor the names of its contributors
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

#include "../Console.h"

String convert_to_string(MouseButton but)
{
  switch (but) {
    case MouseButton::None: return L"None";
    case MouseButton::Left: return L"Left";
    case MouseButton::Right: return L"Right";
    case MouseButton::Middle: return L"Middle";
  }
  return L"Unknown";
}

//////////////////////////////////////////////////////////////////////

class HookedWidget : public Widget
{
  Console* m_console;
public:
  HookedWidget(Widget* parent)
    : Widget(parent, Widget::Styles::Default +
		     Widget::Styles::AcceptFiles +
		     Widget::Styles::Scroll +
		     Widget::Styles::Focusable)
  {
    m_console = NULL;
  }
  void setConsole(Console* console)
  {
    m_console = console;
  }
protected:
  virtual void onPreferredSize(Size& sz)
  {
    sz = Size(64, 64);
    m_console->println(L"onPreferredSize()");
  }
  virtual void onPaint(Graphics& g)
  {
    Brush yellow(Color::Yellow);
    g.fillRect(yellow, getBounds());
    m_console->println(L"onPaint()");
  }
  virtual void onResize(const Size& sz)
  {
    m_console->println(L"onResize()");
    Widget::onResize(sz);
  }
  virtual void onMouseEnter(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseEnter(%d, %d)",
				     ev.getX(), ev.getY()));
    Widget::onMouseEnter(ev);
  }
  virtual void onMouseLeave()
  {
    m_console->println(L"onMouseLeave()");
    Widget::onMouseLeave();
  }
  virtual void onMouseDown(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseDown(%d, %d, %s)",
				     ev.getX(), ev.getY(),
				     convert_to_string(ev.getButton()).c_str()));
    Widget::onMouseDown(ev);
  }
  virtual void onMouseUp(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseUp(%d, %d, %s) (I will request the focus...)",
				     ev.getX(), ev.getY(),
				     convert_to_string(ev.getButton()).c_str()));

    // This is necessary to receive wheel events
    requestFocus();

    Widget::onMouseUp(ev);
  }
  virtual void onMouseMove(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseMove(%d, %d) (I will request the focus...)",
				     ev.getX(), ev.getY()));
    Widget::onMouseMove(ev);
  }
  virtual void onMouseWheel(MouseEvent& ev)
  {
    m_console->println(L"onMouseWheel()");
    Widget::onMouseWheel(ev);
  }
  virtual void onDoubleClick(MouseEvent& ev)
  {
    if (ev.getButton() == MouseButton::Right) {
      m_console->println(L"onDoubleClick() consumed");
      ev.consume();
    }
    else
      m_console->println(L"onDoubleClick() not consumed (use right-button to consume it)");
    Widget::onDoubleClick(ev);
  }
  virtual void onCancelMode()
  {
    m_console->println(L"onCancelMode()");
    Widget::onCancelMode();
  }
  virtual void onSetCursor(WidgetHitTest hitTest)
  {
    m_console->println(L"onSetCursor()");
    Widget::onSetCursor(hitTest);
  }
  virtual void onKeyDown(KeyEvent& ev)
  {
    m_console->println(L"onKeyDown()");
    Widget::onKeyDown(ev);
  }
  virtual void onKeyUp(KeyEvent& ev)
  {
    m_console->println(L"onKeyUp()");
    Widget::onKeyUp(ev);
  }
  virtual void onFocusEnter(Event& ev)
  {
    m_console->println(L"onFocusEnter()");
    Widget::onFocusEnter(ev);
  }
  virtual void onFocusLeave(Event& ev)
  {
    m_console->println(L"onFocusLeave()");
    Widget::onFocusLeave(ev);
  }
  virtual bool onCommand(CommandId id)
  {
    m_console->println(L"onCommand()");
    Widget::onCommand(id);
    return false;
  }
  virtual void onUpdateIndicators()
  {
    //m_console->println(L"onUpdateIndicators()");
    Widget::onUpdateIndicators();
  }
  virtual void onScroll(ScrollEvent& ev)
  {
    m_console->println(L"onScroll()");
    Widget::onScroll(ev);
  }
  virtual void onDropFiles(DropFilesEvent& ev)
  {
    m_console->println(L"onDropFiles()");
    Widget::onDropFiles(ev);
  }
  virtual void onAddChild(Widget* child)
  {
    m_console->println(L"onAddChild()");
    Widget::onAddChild(child);
  }
  virtual void onRemoveChild(Widget* child)
  {
    m_console->println(L"onRemoveChild()");
    Widget::onRemoveChild(child);
  }
};

class EventsTabPage : public TabPage
{
  SplitBar m_splitBar;
  Widget m_widgetContainer;
  HookedWidget m_hookedWidget;
  Console m_console;
public:
  EventsTabPage(Tab* parent)
    : TabPage(L"Events && Signals", parent)
    , m_splitBar(Orientation::Horizontal, this)
    , m_widgetContainer(&m_splitBar)
    , m_hookedWidget(&m_widgetContainer)
    , m_console(&m_splitBar)
  {
    setLayout(new ClientLayout);
    m_widgetContainer.setLayout(new ClientLayout(8));

    m_hookedWidget.setConsole(&m_console);
    m_hookedWidget.DoubleClick.connect(&EventsTabPage::onDoubleClickSignal, this);
  }
private:
  void onDoubleClickSignal(MouseEvent& ev)
  {
  }
};

//////////////////////////////////////////////////////////////////////

class ButtonTabPage : public TabPage
{
public:
  ButtonTabPage(Tab* parent)
    : TabPage(L"Buttons", parent)
  {
  }
};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Tab m_tabs;
  EventsTabPage m_eventsTabPage;
  ButtonTabPage m_buttonTabPage;
public:
  MainFrame()
    : Frame(L"Full")
    , m_tabs(this)
    , m_eventsTabPage(&m_tabs)
    , m_buttonTabPage(&m_tabs)
  {
    setLayout(new ClientLayout);
  }
};

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setVisible(true);
  app.run();
  return 0;
}
