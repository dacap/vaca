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
#include "vector2d.h"
#include <cmath>
#include <ctime>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

// ticks per second
#define TPS 60

using namespace Vaca;

//////////////////////////////////////////////////////////////////////
// simulation model

namespace simulation_model {

v2dfloat uniform_dist(v2dfloat a, v2dfloat b)
{
  return a + (b-a) * ((std::rand() % 10001) / 10000.0);
}

struct segment
{
  vector2d pos, dir;

  segment(const vector2d &pos, const vector2d &dir)
  {
    this->pos = pos;
    this->dir = dir;
  }

  void draw(Graphics &g, double a, double b)
  {
    // segment
    g.drawLine(Pen(Color::White, 3),
	       static_cast<int>(pos.x*a),
	       static_cast<int>(pos.y*b),
	       static_cast<int>((pos.x+dir.x)*a),
	       static_cast<int>((pos.y+dir.y)*b));

    // normal
    vector2d normal = 8*dir.get_normal().normalize();
    g.drawLine(Pen(Color::White, 1),
	       static_cast<int>((pos.x+dir.x/2)*a),
	       static_cast<int>((pos.y+dir.y/2)*b),
	       static_cast<int>((pos.x+dir.x/2+normal.x)*a),
	       static_cast<int>((pos.y+dir.y/2+normal.y)*b));
  }

  // TODO this is wrong
  bool collided_circumference(const vector2d &v, v2dfloat radius)
  {
    vector2d a = pos;
    vector2d b = pos+dir;

    if (a.x > b.x) std::swap(a.x, b.x);
    if (a.y > b.y) std::swap(a.y, b.y);

    // outside the box?
    if (v.x < a.x-radius || v.x > b.x+radius ||
	v.y < a.y-radius || v.y > b.y+radius)
      return false;

    return project_on_normal(v).magnitude() <= radius;
  }

  vector2d project_on_normal(const vector2d &v)
  {
    return (v-pos).project_on(dir.get_normal());
  }
  
};

struct ball;

std::vector<segment> segments;
std::vector<ball*> balls;

v2dfloat friction_with_air = 1.0;
v2dfloat friction_with_ball = 1.0;
v2dfloat friction_with_seg = 1.0;

struct ball
{
  vector2d pos;
  vector2d vel;
  vector2d acc;
  v2dfloat mass;
  v2dfloat radius;
  Color color;

  ball(const vector2d &pos,
       const vector2d &vel,
       const vector2d &acc)
  {
    this->pos = pos;
    this->vel = vel;
    this->acc = acc;
    radius    = uniform_dist(8, 20);
    mass      = 100.0f * M_PI*radius*radius;

    switch (std::rand()%5) {
      case 0: color = Color(255, 255, 255); break;
      case 1: color = Color(255, 0, 0); break;
      case 2: color = Color(0, 192, 0); break;
      case 3: color = Color(255, 255, 0); break;
      case 4: color = Color(0, 0, 255); break;
    }
  }

  void move(const vector2d &dir)
  {
    vector2d pos_t    = pos;
    int      steps    = static_cast<int>(ceil(dir.magnitude()+1));
    vector2d delta    = dir / steps;
    bool     collided_with_seg = false;

    for (int c=0; c<steps; ++c) {

      // collision with balls
      for (std::vector<ball*>::iterator
	     it = balls.begin(); it != balls.end(); ++it) {
	ball *other = *it;

	if (this != other) {
	  vector2d u = other->pos - pos_t; // vector between two centers
	  v2dfloat u_mag = u.magnitude();

	  // both balls are colliding?
	  if (u_mag <= this->radius+other->radius) {
	    u = u.normalize();
	    vector2d n = u.get_normal();

	    // separate the two balls
	    pos_t -= u * (this->radius+other->radius-u_mag+1.0f);

	    v2dfloat cor  = 1; // coefficient of restitution = 1 (elastic collision)
	    v2dfloat m1   = this ->mass;
	    v2dfloat m2   = other->mass;
 	    v2dfloat v1_u = this ->vel * u;
	    v2dfloat v2_u = other->vel * u;
 	    v2dfloat v1_n = this ->vel * n;
	    v2dfloat v2_n = other->vel * n;

	    // equations for collisions between elastic particles
	    // using COR, where v1_f and v2_f are the final velocities
	    v2dfloat v1_f = (v1_u*(m1-cor*m2)+(cor+1)*m2*v2_u) / (m1+m2);
	    v2dfloat v2_f = (v2_u*(m2-cor*m1)+(cor+1)*m1*v1_u) / (m1+m2);

	    // total momentum remains constant: m1*v1_u + m2*v2_u == m1*v1_f + m2*v2_f
  
	    this ->vel = (u * v1_f) + (n * v1_n);
	    other->vel = (u * v2_f) + (n * v2_n);

	    this ->vel *= friction_with_ball;
	    other->vel *= friction_with_ball;

	    collided_with_seg = true;
	  }
	}
      }
      
      // collision with segments
      for (std::vector<segment>::iterator
	     seg = segments.begin(); seg != segments.end(); ++seg) {

	if (seg->collided_circumference(pos_t, radius)) {
	  // modify position
	  vector2d u = seg->project_on_normal(pos_t);
	  
	  v2dfloat r = radius - u.magnitude() + 1;
	  pos_t += r * u.normalize();

	  // modify direction
	  this->vel =
	    dir.project_on(seg->dir) +
	    (-dir).project_on(seg->dir.get_normal());

	  this->vel *= friction_with_seg;

	  collided_with_seg = true;
	}
      }

      if (collided_with_seg)
	break;

      pos_t += delta;
    }

    pos = pos_t;
  }

};

void move_balls()
{
  for (std::vector<ball*>::iterator
	 it = balls.begin(); it != balls.end(); ++it) {
    ball* bal = *it;
    
    // move balls
    bal->vel += bal->acc;
    bal->move(bal->vel);

    bal->vel *= friction_with_air;
  }
  
}

void draw_balls(Graphics &g, double a, double b)
{
  for (std::vector<ball*>::iterator
	 it = balls.begin(); it != balls.end(); ++it) {
    ball* bal = *it;
    Brush brush(bal->color);

    g.fillEllipse(brush,
		  static_cast<int>((bal->pos.x-bal->radius)*a),
		  static_cast<int>((bal->pos.y-bal->radius)*b),
		  static_cast<int>((bal->radius*2+1)*a),
		  static_cast<int>((bal->radius*2+1)*b));
  }
}

void draw_segments(Graphics &g, double a, double b)
{
  for (std::vector<segment>::iterator
	 it = segments.begin(); it != segments.end(); ++it) {
    it->draw(g, a, b);
  }
}

void add_ball()
{
  double x = uniform_dist(50, 500);
  double y = uniform_dist(100, 200);
  double a = uniform_dist(-M_PI, M_PI);
  double v = uniform_dist(1 * 60/TPS, 2 * 60/TPS);
  balls.push_back(new ball(vector2d(x, y),
			   vector2d(v*cos(a), v*sin(a)),
			   vector2d(0, 0.5)));
}

void init_balls()
{
  // create one ball
  add_ball();
}

void init_segments()
{
#if 0
  // create scene
  segments.push_back(segment(vector2d( 50,  50), vector2d(  0, 250)));
  segments.push_back(segment(vector2d( 50, 300), vector2d(150, 250)));
  segments.push_back(segment(vector2d(200, 550), vector2d(100, 50)));
  segments.push_back(segment(vector2d(300, 600), vector2d(70, -50)));
  segments.push_back(segment(vector2d(370, 550), vector2d(30, -70)));
  segments.push_back(segment(vector2d(400, 480), vector2d(150, -80)));
  segments.push_back(segment(vector2d(550, 400), vector2d(0, -300)));
  segments.push_back(segment(vector2d(550, 100), vector2d(-500, -50)));

  segments.push_back(segment(vector2d(200, 300), vector2d( 100, 10)));
  segments.push_back(segment(vector2d(300, 310), vector2d(  20, 50)));
  segments.push_back(segment(vector2d(320, 360), vector2d(-120,-60)));
#else
  // create scene
  segments.push_back(segment(vector2d( 50,  50), vector2d(  0, 250)));
  segments.push_back(segment(vector2d( 50, 300), vector2d(150, 250)));
  segments.push_back(segment(vector2d(200, 550), vector2d(100, 50)));
  segments.push_back(segment(vector2d(300, 600), vector2d(70, -50)));
  segments.push_back(segment(vector2d(370, 550), vector2d(30, -70)));
  segments.push_back(segment(vector2d(400, 480), vector2d(150, 0)));
  segments.push_back(segment(vector2d(550, 480), vector2d(100, 80)));
  segments.push_back(segment(vector2d(650, 560), vector2d(100, -40)));
  segments.push_back(segment(vector2d(750, 520), vector2d(100, -80)));
  segments.push_back(segment(vector2d(850, 440), vector2d(-50, -340)));
  segments.push_back(segment(vector2d(800, 100), vector2d(-750, -50)));

  segments.push_back(segment(vector2d(200, 300), vector2d( 100, 10)));
  segments.push_back(segment(vector2d(300, 310), vector2d(  20, 50)));
  segments.push_back(segment(vector2d(320, 360), vector2d(-120,-60)));
#endif
}

}

//////////////////////////////////////////////////////////////////////

class MainFrame : public Frame
{
  Timer m_timer;
  
public:

  MainFrame()
    : Frame(L"BouncingBall", NULL, Frame::Styles::Default +
				   Widget::Styles::ClientEdge)
    , m_timer(1000/TPS)
  {
    setDoubleBuffered(true);
    setBgColor(Color::Black);
    setFgColor(Color::White);

    simulation_model::init_balls();
    simulation_model::init_segments();

    m_timer.Tick.connect(&MainFrame::onTick, this);
    m_timer.start();

    setSize(400, 300);
    center();
  }

protected:

//   void onResize(ResizeEvent& ev)
//   {
//     Timer::pollTimers();
//     update();
//     Frame::onResize(ev);
//   }

  virtual void onResizing(CardinalDirection dir, Rect &rc)
  {
    Frame::onResizing(dir, rc);

    if (dir == CardinalDirection::North ||
	dir == CardinalDirection::South) {
      int w = rc.h*4/3;
      rc = Rect(rc.x+rc.w/2-w/2, rc.y, w, rc.h);
    }
    else if (dir == CardinalDirection::West ||
	     dir == CardinalDirection::East) {
      int h = rc.w*3/4;
      rc = Rect(rc.x, rc.y+rc.h/2-h/2, rc.w, h);
    }
    else if (dir == CardinalDirection::Southeast) {
      int w = rc.w > rc.h*4/3 ? rc.w: rc.h*4/3;
      int h = rc.w > rc.h*4/3 ? rc.w*3/4: rc.h;
      rc = Rect(rc.x, rc.y, w, h);
    }
    else if (dir == CardinalDirection::Southwest) {
      int w = rc.w > rc.h*4/3 ? rc.w: rc.h*4/3;
      int h = rc.w > rc.h*4/3 ? rc.w*3/4: rc.h;
      rc = Rect(rc.x+rc.w-w, rc.y, w, h);
    }
    else if (dir == CardinalDirection::Northeast) {
      int w = rc.w > rc.h*4/3 ? rc.w: rc.h*4/3;
      int h = rc.w > rc.h*4/3 ? rc.w*3/4: rc.h;
      rc = Rect(rc.x, rc.y+rc.h-h, w, h);
    }
    else if (dir == CardinalDirection::Northwest) {
      int w = rc.w > rc.h*4/3 ? rc.w: rc.h*4/3;
      int h = rc.w > rc.h*4/3 ? rc.w*3/4: rc.h;
      rc = Rect(rc.x+rc.w-w, rc.y+rc.h-h, w, h);
    }
    else
      rc = getBounds();
    
//     Timer::pollTimers();
//     update();
  }

  virtual void onPaint(PaintEvent& ev)
  {
    Graphics &g = ev.getGraphics();
    Rect rc = getClientBounds();
    double a = static_cast<double>(rc.w) / static_cast<double>(900);
    double b = static_cast<double>(rc.h) / static_cast<double>(650);

    g.drawString(L"Press 'A' key to add balls", getFgColor(), 0, 0);

    simulation_model::draw_balls   (g, a, b);
    simulation_model::draw_segments(g, a, b);
  }

  void onKeyDown(KeyEvent &ev)
  {
    Frame::onKeyDown(ev);

    switch (ev.getKeyCode()) {

      case Keys::A:
	simulation_model::add_ball();
	break;

      case Keys::Escape:
	// close the frame
	setVisible(false);
	break;
	
    }
  }

  void onMouseDown(MouseEvent &ev)
  {
    Frame::onMouseDown(ev);
    simulation_model::add_ball();
  }
  
private:

  void onTick()
  {
    simulation_model::move_balls();
    invalidate(true);
  }

};

//////////////////////////////////////////////////////////////////////

int VACA_MAIN()
{
  std::srand(std::time(NULL));

  Application app;
  MainFrame mainFrame;
  mainFrame.setVisible(true);
  app.run();

  return 0;
}
