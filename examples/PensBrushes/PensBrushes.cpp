// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, 2008, David A. Capello
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

//////////////////////////////////////////////////////////////////////
// Preview

class Preview : public Panel
{
  Pen m_pen;
  Brush m_brush;
  bool m_closeFigure;
  
public:

  Preview(int penWidth, Widget* parent)
    : Panel(parent, PanelStyle + ClientEdgeStyle)
    , m_pen(Color::Red, penWidth)
    , m_brush(Color(0, 0, 200))
    , m_closeFigure(false)
  {
    setBgColor(Color::White);
    setDoubleBuffered(true);
  }

  Pen getPen() const
  {
    return m_pen;
  }

  void setPen(Pen pen)
  {
    m_pen = pen;
    invalidate(true);
  }

  Brush getBrush() const
  {
    return m_brush;
  }

  void setBrush(Brush brush)
  {
    m_brush = brush;
    invalidate(true);
  }

  void setCloseFigure(bool closeFigure)
  {
    m_closeFigure = closeFigure;
    invalidate(true);
  }

protected:
  
  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();

    GraphicsPath path;
    path.moveTo(rc.x+64, rc.y+64);
    path.lineTo(rc.x+rc.w/4, rc.y+rc.h*3/4);
    path.lineTo(rc.x+rc.w-64, rc.y+rc.h-64);
    if (m_closeFigure)
      path.closeFigure();
    
    g.setMiterLimit(10000.0f);
    g.fillPath(path, m_brush, Point(0, 0));
    g.strokePath(path, m_pen, Point(0, 0));
  }

  virtual void onResize(const Size &sz)
  {
    Panel::onResize(sz);
    invalidate(true);
  }
  
};

//////////////////////////////////////////////////////////////////////
// MainFrame

class MainFrame : public Frame
{
  Preview m_preview;
  CheckBox m_closeFigure;
  GroupBox m_penPanel;
  GroupBox m_brushPanel;
  // for Pen
  Panel m_penLeftPanel;
  Label m_penWidthLabel;
  Slider m_penWidth;
  Button m_penColor;
  GroupBox m_endCapPanel;
  RadioGroup m_endCapGroup;
  RadioButton m_roundEndCap;
  RadioButton m_squareEndCap;
  RadioButton m_flatEndCap;
  GroupBox m_joinPanel;
  RadioGroup m_joinGroup;
  RadioButton m_roundJoin;
  RadioButton m_bevelJoin;
  RadioButton m_miterJoin;
  // for Brush
  Button m_brushColor;
  
public:

  MainFrame()
    : Frame("Pens & Brushes (WIP)")
    , m_preview(32, this)
    , m_closeFigure("Close Figure", this)
    , m_penPanel("Pen", this)
    , m_brushPanel("Brush", this)
    , m_penLeftPanel(&m_penPanel)
    , m_penWidthLabel("Width:", &m_penLeftPanel)
    , m_penWidth(1, 64, 32, &m_penLeftPanel)
    , m_penColor("Color", &m_penLeftPanel)
    , m_endCapPanel("End Cap", &m_penPanel)
    , m_roundEndCap("Round", m_endCapGroup, &m_endCapPanel)
    , m_squareEndCap("Square", m_endCapGroup, &m_endCapPanel)
    , m_flatEndCap("Flat", m_endCapGroup, &m_endCapPanel)
    , m_joinPanel("Join", &m_penPanel)
    , m_roundJoin("Round", m_joinGroup, &m_joinPanel)
    , m_bevelJoin("Bevel", m_joinGroup, &m_joinPanel)
    , m_miterJoin("Miter", m_joinGroup, &m_joinPanel)
    , m_brushColor("Color", &m_brushPanel)
  {
    setLayout(new BoxLayout(Orientation::Vertical, false));
    m_preview.setConstraint(new BoxConstraint(true));
    m_penPanel.setLayout(new BoxLayout(Orientation::Horizontal, true, 0));
    m_penLeftPanel.setLayout(new BoxLayout(Orientation::Vertical, false, 0));
    m_endCapPanel.setLayout(new BoxLayout(Orientation::Vertical, true, 0));
    m_joinPanel.setLayout(new BoxLayout(Orientation::Vertical, true, 0));
    m_brushPanel.setLayout(new BoxLayout(Orientation::Horizontal, true, 0));

    m_roundEndCap.setSelected(true);
    m_roundJoin.setSelected(true);

    m_penWidth.Change.connect(Bind(&MainFrame::regenerate, this));
    m_endCapGroup.Change.connect(Bind(&MainFrame::regenerate, this));
    m_joinGroup.Change.connect(Bind(&MainFrame::regenerate, this));

    m_penColor.Action.connect(Bind(&MainFrame::onSelectPenColor, this));
    m_brushColor.Action.connect(Bind(&MainFrame::onSelectBrushColor, this));
    m_closeFigure.Action.connect(Bind(&MainFrame::onCloseFigure, this));
  }

private:

  void onSelectPenColor()
  {
    ColorDialog dlg(m_preview.getPen().getColor(), this);
    if (dlg.doModal())
      m_preview.setPen(createPen(dlg.getColor()));
  }

  void onSelectBrushColor()
  {
    ColorDialog dlg(m_preview.getBrush().getColor(), this);
    if (dlg.doModal())
      m_preview.setBrush(Brush(dlg.getColor()));
  }

  void onCloseFigure()
  {
    m_preview.setCloseFigure(m_closeFigure.isSelected());
  }

  void regenerate()
  {
    m_preview.setPen(createPen(m_preview.getPen().getColor()));
  }

  Pen createPen(const Color& color)
  {
    PenEndCap endCap;
    PenJoin join;
    switch (m_endCapGroup.getSelectedIndex()) {
      case 0: endCap = PenEndCap::Round; break;
      case 1: endCap = PenEndCap::Square; break;
      case 2: endCap = PenEndCap::Flat; break;
    }
    switch (m_joinGroup.getSelectedIndex()) {
      case 0: join = PenJoin::Round; break;
      case 1: join = PenJoin::Bevel; break;
      case 2: join = PenJoin::Miter; break;
    }
    return Pen(color,
	       m_penWidth.getValue(),
	       PenStyle::Solid,
	       endCap, join);
  }

};

//////////////////////////////////////////////////////////////////////
// Example

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main() {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example app;
  app.run();
  return 0;
}
