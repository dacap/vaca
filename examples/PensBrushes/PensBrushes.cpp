// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
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

#include <Vaca/Vaca.hpp>

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// Preview

class Preview : public Panel
{
  Pen m_pen;
  Brush m_brush;
  bool m_closeFigure;
  
public:

  Preview(int penWidth, Widget *parent)
    : Panel(parent, PanelStyle + ClientEdgeStyle)
    , m_pen(Color::Red, penWidth)
    , m_brush(Color(0, 0, 200))
  {
    setBgColor(Color::White);
    setDoubleBuffered(true);
  }

  void setPenWidth(int penWidth)
  {
    m_pen.setWidth(penWidth);
    invalidate(true);
  }

  void setPenEndCap(PenEndCap penEndCap)
  {
    m_pen.setEndCap(penEndCap);
    invalidate(true);
  }

  void setPenJoin(PenJoin penJoin)
  {
    m_pen.setJoin(penJoin);
    invalidate(true);
  }

  void setCloseFigure(bool closeFigure)
  {
    m_closeFigure = closeFigure;
    invalidate(true);
  }

  Color getPenColor() const
  {
    return m_pen.getColor();
  }

  void setPenColor(Color color)
  {
    m_pen.setColor(color);
    invalidate(true);
  }

  Color getBrushColor() const
  {
    return m_brush.getColor();
  }

  void setBrushColor(Color color)
  {
    m_brush.setColor(color);
    invalidate(true);
  }

protected:
  
  virtual void onPaint(Graphics &g)
  {
    Rect rc = getClientBounds();

    // TODO fix this, we have two times the same thing, so we can use
    // a GraphicsPath here instead...
    
    g.beginPath();
    g.moveTo(rc.x+64, rc.y+64);
    g.lineTo(m_pen, rc.x+rc.w/4, rc.y+rc.h*3/4);
    g.lineTo(m_pen, rc.x+rc.w-64, rc.y+rc.h-64);
    if (m_closeFigure)
      g.closeFigure();
    g.endPath();
    g.setMiterLimit(10000.0f);
    g.fillPath(m_brush);
    
    g.beginPath();
    g.moveTo(rc.x+64, rc.y+64);
    g.lineTo(m_pen, rc.x+rc.w/4, rc.y+rc.h*3/4);
    g.lineTo(m_pen, rc.x+rc.w-64, rc.y+rc.h-64);
    if (m_closeFigure)
      g.closeFigure();
    g.endPath();
    g.setMiterLimit(10000.0f);
    g.strokePath(m_pen);
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

    m_penWidth.Change.connect(Bind(&MainFrame::onChangePenWidth, this));
    m_penColor.Action.connect(Bind(&MainFrame::onSelectPenColor, this));
    m_brushColor.Action.connect(Bind(&MainFrame::onSelectBrushColor, this));
    m_closeFigure.Action.connect(Bind(&MainFrame::onCloseFigure, this));
    m_roundEndCap.Action.connect(Bind(&MainFrame::onEndCap, this, PenEndCap::Round));
    m_squareEndCap.Action.connect(Bind(&MainFrame::onEndCap, this, PenEndCap::Square));
    m_flatEndCap.Action.connect(Bind(&MainFrame::onEndCap, this, PenEndCap::Flat));
    m_roundJoin.Action.connect(Bind(&MainFrame::onJoin, this, PenJoin::Round));
    m_bevelJoin.Action.connect(Bind(&MainFrame::onJoin, this, PenJoin::Bevel));
    m_miterJoin.Action.connect(Bind(&MainFrame::onJoin, this, PenJoin::Miter));
  }

private:

  void onChangePenWidth()
  {
    m_preview.setPenWidth(m_penWidth.getValue());
  }

  void onSelectPenColor()
  {
    ColorDialog dlg(m_preview.getPenColor(), this);
    if (dlg.doModal())
      m_preview.setPenColor(dlg.getColor());
  }

  void onSelectBrushColor()
  {
    ColorDialog dlg(m_preview.getBrushColor(), this);
    if (dlg.doModal())
      m_preview.setBrushColor(dlg.getColor());
  }

  void onCloseFigure()
  {
    m_preview.setCloseFigure(m_closeFigure.isSelected());
  }

  void onEndCap(PenEndCap::enumeration value)
  {
    m_preview.setPenEndCap(value);
  }

  void onJoin(PenJoin::enumeration value)
  {
    m_preview.setPenJoin(value);
  }
  
};

//////////////////////////////////////////////////////////////////////
// Example

class Example : public Application
{
  MainFrame m_mainFrame;
public:
  virtual void main(std::vector<String> args) {
    m_mainFrame.setVisible(true);
  }
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nCmdShow)
{
  Example *app(new Example);
  app->run();
  delete app;
  return 0;
}
