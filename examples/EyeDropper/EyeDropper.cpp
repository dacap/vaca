// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include <vaca/vaca.h>
#include "resource.h"

using namespace vaca;

// the eye-dropper widget
class EyeDropper : public Widget
{
  Font m_font;
  Cursor m_cursor;
  bool m_isHexFormat;

public:

  EyeDropper(Widget* parent)
    : Widget(parent)
    , m_font(L"Courier", 10)
    , m_cursor(ResourceId(IDC_EYEDROPPER))
    , m_isHexFormat(false)
  {
    setBgColor(Color::Black);
    setPreferredSize(Size(64, 64));
  }

  void setHexFormat(bool hexFormat)
  {
    m_isHexFormat = hexFormat;
    invalidate(true);
  }

protected:

  virtual void onPaint(PaintEvent& ev)
  {
    // remember not to call the base implementation of onPaint (Widget::onPaint)

    Graphics& g = ev.getGraphics();
    Rect rc = getClientBounds();
    Color bg = getBgColor();
    Pen blackPen(Color::Black);
    Pen whitePen(Color::White);

    g.drawRect(blackPen, rc);

    rc.shrink(1);
    g.drawRect(whitePen, rc);

    rc.shrink(1);
    g.drawRect(blackPen, rc);

    g.setFont(m_font);

    // draw the text

    String str;

    if (m_isHexFormat)
      str = format_string(L"#%02x%02x%02x", bg.getR(), bg.getG(), bg.getB());
    else
      str = format_string(L"(%d, %d, %d)", bg.getR(), bg.getG(), bg.getB());

    g.drawString(str, bg.toBlackAndWhite().negative(),
		 rc.getCenter() - Point(g.measureString(str)/2));

    // copy the string to the clipboard
    Clipboard(this).setString(str);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    // remember to call the base implementation to generated the MouseDown signal
    Widget::onMouseDown(ev);

    captureMouse();
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    // remember to call the base implementation to generated the MouseMove signal
    Widget::onMouseMove(ev);

    if (hasCapture()) {
      ScreenGraphics g;
      setBgColor(g.getPixel(System::getCursorPos()));
      invalidate(true);
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    // remember to call the base implementation to generated the MouseUp signal
    Widget::onMouseUp(ev);

    if (hasCapture())
      releaseMouse();
  }

  virtual void onSetCursor(SetCursorEvent &ev)
  {
    if (!ev.isConsumed() && hasCapture())
      ev.setCursor(m_cursor);
    Widget::onSetCursor(ev);
  }

};

// the main window
class MainFrame : public Frame
{
  Label m_label;
  EyeDropper m_eyeDropper;
  ToggleButton m_hexFormat;

public:

  MainFrame()
    : Frame(L"Eye Dropper", NULL, Frame::Styles::Default
				  - Frame::Styles::Resizable
				  - Frame::Styles::Maximizable)
    , m_label(L"Drag from the box to the pixel\r\n"
	      L"that you want to get its color", this)
    , m_eyeDropper(this)
    , m_hexFormat(L"Hex", this)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_hexFormat.Click.connect(&MainFrame::onHexToggle, this);
    setSize(getPreferredSize());
  }

protected:

  void onHexToggle(Event& ev)
  {
    m_eyeDropper.setHexFormat(m_hexFormat.isSelected());
  }

};

int VACA_MAIN()
{
  Application app;
  MainFrame frm;
  frm.setIcon(ResourceId(IDI_VACA));
  frm.setVisible(true);
  app.run();
  return 0;
}
