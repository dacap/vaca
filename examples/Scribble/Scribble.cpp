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

#include <Vaca/Vaca.hpp>

using namespace Vaca;

class Scribble : public Panel
{
  Image m_image;
  Point m_point[3];
  bool m_erasing;

public:

  Scribble(Widget* parent)
    : Panel(parent)
  {
    setBgColor(Color::White);
  }

protected:

  virtual void onResize(const Size &size)
  {
    Size imageSize = getClientBounds().getSize();
    if (imageSize.w == 0 || imageSize.h == 0)
      return;
    
    Image newImage(imageSize);

    Graphics &g = *newImage.getGraphics();

    // clear the new image with a white background
    Brush whiteBrush(Color::White);
    g.fillRect(whiteBrush, 0, 0, imageSize.w, imageSize.h);

    // draw the old image in the center of the new image
    if (m_image.isValid()) {
      g.drawImage(m_image, Point(imageSize/2 - m_image.getSize()/2));
    }

    // clone the image
    m_image = newImage; // TODO I don't like this clone idiom (maybe m_image = newImage.clone() would be better)

    // redraw all the image
    invalidate(false);
  }

  virtual void onPaint(Graphics &g)
  {
    // draw only the clip area
    Rect rc = g.getClipBounds();
    g.drawImage(m_image, rc.getOrigin(), rc);
  }

  virtual void onMouseDown(MouseEvent &ev)
  {
    if (!hasCapture()) {
      captureMouse();

      for (int c=0; c<3; ++c)
	m_point[c] = ev.getPoint();

      // with right mouse button we erase 
      m_erasing = ev.getButton() == MouseButtons::Right;
    }
  }

  virtual void onMouseMove(MouseEvent &ev)
  {
    if (hasCapture()) {
      // get the graphics from the to draw into the image
      Graphics &g = *m_image.getGraphics();

      // rotate points
      m_point[0] = m_point[1];
      m_point[1] = m_point[2];
      m_point[2] = ev.getPoint();

      // pen
      Color color(m_erasing ? Color::White: Color::Black);
      Pen pen(color, m_erasing ? 64: 1);

      // draw (or erase)
      g.drawLine(pen, m_point[1], m_point[2]);

      // we are drawing?
      if (!m_erasing) {
	// this adds an extra style to the trace (more realistic)
	g.beginPath();		// TODO remove this, use GraphicsPath
	g.moveTo(m_point[0]);
	g.lineTo(pen, m_point[1]);
	g.lineTo(pen, m_point[2]);
	g.endPath();

	Brush brush(color);
	g.fillPath(brush);
      }

      // invalidate the drawn area
      Point minPoint = m_point[0];
      Point maxPoint = m_point[0];
      for (int c = 1; c < 3; ++c) {
	if (minPoint.x > m_point[c].x) minPoint.x = m_point[c].x;
	if (minPoint.y > m_point[c].y) minPoint.y = m_point[c].y;
	if (maxPoint.x < m_point[c].x) maxPoint.x = m_point[c].x;
	if (maxPoint.y < m_point[c].y) maxPoint.y = m_point[c].y;
      }
      invalidate(Rect(minPoint-pen.getWidth(),
		      maxPoint+pen.getWidth()), false);
    }
  }

  virtual void onMouseUp(MouseEvent &ev)
  {
    if (hasCapture()) {
      // we can release the capture (remember to check hasCapture()
      // before to release the mouse capture)
      releaseMouse();
    }
  }

};

class MainFrame : public Frame
{
  Scribble m_scrible;

public:
  
  MainFrame()
    : Frame("Scribble")
    , m_scrible(this)
  {
    setLayout(new ClientLayout);
  }
  
};

//////////////////////////////////////////////////////////////////////

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
  Example* app(new Example);
  app->run();
  delete app;
  return 0;
}
