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
#include "../resource.h"
#include "convert_to.h"

//////////////////////////////////////////////////////////////////////

class HookedWidget : public Widget
{
  Console* m_console;
public:

  HookedWidget(Widget* parent)
    : Widget(parent, Widget::Styles::Default +
		     Widget::Styles::ClientEdge +
		     Widget::Styles::AcceptFiles +
		     Widget::Styles::Scroll +
		     Widget::Styles::Focusable)
  {
    m_console = NULL;
    setBgColor(Color(255, 255, 220));
  }

  void setConsole(Console* console)
  {
    m_console = console;
  }

protected:

  virtual void onPreferredSize(PreferredSizeEvent& ev)
  {
    ev.setPreferredSize(256, 128);
    m_console->println(L"onPreferredSize()");
  }

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics& g = ev.getGraphics();

    int y=0, h = g.measureString(L" ").h;
    if (hasFocus()) { g.drawString(L"hasFocus", Color::Black, 0, y); y += h; }
    if (hasMouse()) { g.drawString(L"hasMouse", Color::Black, 0, y); y += h; }
    // if (hasMouseAbove()) { g.drawString(L"hasMouseAbove", Color::Black, 0, y); y += h; }
    if (hasCapture()) { g.drawString(L"hasCapture", Color::Black, 0, y); y += h; }

    //m_console->println(L"onPaint()");
  }

  virtual void onResize(ResizeEvent& ev)
  {
    m_console->println(L"onResize()");
    Widget::onResize(ev);
  }

  virtual void onMouseEnter(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseEnter(%d, %d)",
				     ev.getX(), ev.getY()));
    invalidate(true);
    update();

    Widget::onMouseEnter(ev);
  }

  virtual void onMouseLeave(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseLeave(%d, %d)",
				     ev.getX(), ev.getY()));
    invalidate(true);
    update();

    Widget::onMouseLeave(ev);
  }

  virtual void onMouseDown(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseDown(%d, %d, %s)",
				     ev.getX(), ev.getY(),
				     convert_to_string(ev.getButton())));

    if (ev.isRightButtonPressed()) {
      captureMouse();

      invalidate(true);
      update();
    }

    Widget::onMouseDown(ev);
  }

  virtual void onMouseUp(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseUp(%d, %d, %s)",
				     ev.getX(), ev.getY(),
				     convert_to_string(ev.getButton())));

    if (hasCapture()) {
      releaseMouse();

      invalidate(true);
      update();
    }

    // This is necessary if you want to receive wheel events
    requestFocus();

    Widget::onMouseUp(ev);
  }

  virtual void onMouseMove(MouseEvent& ev)
  {
    m_console->println(format_string(L"onMouseMove(%d, %d)",
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

  virtual void onSetCursor(SetCursorEvent& ev)
  {
    m_console->println(format_string(L"onSetCursor(%s)", convert_to_string(ev.getWidgetHit())));
    Widget::onSetCursor(ev);
  }

  virtual void onKeyDown(KeyEvent& ev)
  {
    String mods;
    if (ev.isShift()  ) mods += L" Shift";
    if (ev.isControl()) mods += L" Control";
    if (ev.isAlt()    ) mods += L" Alt";
    m_console->println(format_string(L"onKeyDown(keyCode=%s, modifiers=%s, charCode=%d '%c')",
				     convert_to_string(ev.getKeyCode()), mods.c_str(),
				     ev.getCharCode(), ev.getCharCode()));
    Widget::onKeyDown(ev);
  }

  virtual void onKeyUp(KeyEvent& ev)
  {
    String mods;
    if (ev.isShift()  ) mods += L" Shift";
    if (ev.isControl()) mods += L" Control";
    if (ev.isAlt()    ) mods += L" Alt";
    m_console->println(format_string(L"onKeyUp(keyCode=%s, modifiers=%s, charCode=%d '%d')",
				     convert_to_string(ev.getKeyCode()), mods.c_str(),
				     ev.getCharCode(), ev.getCharCode()));
    Widget::onKeyUp(ev);
  }

  virtual void onFocusEnter(FocusEvent& ev)
  {
    m_console->println(L"onFocusEnter()");

    invalidate(true);
    update();

    Widget::onFocusEnter(ev);
  }

  virtual void onFocusLeave(FocusEvent& ev)
  {
    m_console->println(L"onFocusLeave()");

    invalidate(true);
    update();

    Widget::onFocusLeave(ev);
  }

  virtual void onCommand(CommandEvent& ev)
  {
    if (!ev.isConsumed()) {
      m_console->println(L"onCommand()");
      ev.consume();
    }
    Widget::onCommand(ev);
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

  virtual void onAddChild(ChildEvent& ev)
  {
    m_console->println(L"onAddChild()");
    Widget::onAddChild(ev);
  }

  virtual void onRemoveChild(ChildEvent& ev)
  {
    m_console->println(L"onRemoveChild()");
    Widget::onRemoveChild(ev);
  }

};

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  SplitBar m_splitBar;
  Widget m_widgetContainer;
  HookedWidget m_hookedWidget;
  Console m_console;
public:
  MainFrame()
    : Frame(L"Events")
    , m_splitBar(Orientation::Horizontal, this)
    , m_widgetContainer(&m_splitBar)
    , m_hookedWidget(&m_widgetContainer)
    , m_console(&m_splitBar)
  {
    setLayout(new ClientLayout);
    m_widgetContainer.setLayout(new AnchorLayout(Size(256, 128)));
    m_hookedWidget.setConstraint(new Anchor(Rect(0, 0, 256, 128), Sides::None));

    m_hookedWidget.setConsole(&m_console);
    m_hookedWidget.DoubleClick.connect(&MainFrame::onDoubleClickSignal, this);
  }
private:
  void onDoubleClickSignal(MouseEvent& ev)
  {
    m_console.println(String(L"DoubleClick signal received") +
		      (ev.isConsumed() ? L" (consumed)": L""));
  }
};

int vaca_main()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}

#include "Vaca/main.h"
