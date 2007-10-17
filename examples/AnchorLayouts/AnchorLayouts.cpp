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

#include "Vaca/Vaca.h"

using namespace Vaca;

//////////////////////////////////////////////////////////////////////

class MyMdiChild : public MdiChild
{
public:

  MyMdiChild(String title, MdiFrame *parent)
    : MdiChild(title, parent)
  {
    // this child can't be maximized (we can't specify this style in
    // the MdiChild() constructor, see TN005)
    removeStyle(MaximizableFrameStyle);
  }

};

//////////////////////////////////////////////////////////////////////

class AnchorChild : public MyMdiChild
{
  Button mButton;

public:

  AnchorChild(String title, Borders borders, MdiFrame *parent)
    : MyMdiChild(title, parent)
    , mButton("", this)
  {
    setLayout(new AnchorLayout(Size(40, 40)));
    mButton.setConstraint(new Anchor(Rect(10, 10, 20, 20), borders));
  }

};

//////////////////////////////////////////////////////////////////////

class MixedChild : public MyMdiChild
{
  Button mButtonCenter;
  Button mButtonLeft;
  Button mButtonTop;
  Button mButtonRight;
  Button mButtonBottom;

public:

  MixedChild(String title, MdiFrame *parent)
    : MyMdiChild(title, parent)
    , mButtonCenter("", this)
    , mButtonLeft("", this)
    , mButtonTop("", this)
    , mButtonRight("", this)
    , mButtonBottom("", this)
  {
    setLayout(new AnchorLayout(Size(90, 90)));
    mButtonCenter.setConstraint(new Anchor(Rect(30, 30, 30, 30), AllBorders));
    mButtonLeft.setConstraint(new Anchor(Rect(10, 30, 15, 30), LeftBorder | TopBorder | BottomBorder));
    mButtonTop.setConstraint(new Anchor(Rect(30, 10, 30, 15), TopBorder | LeftBorder | RightBorder));
    mButtonRight.setConstraint(new Anchor(Rect(70, 30, 15, 30), RightBorder | TopBorder | BottomBorder));
    mButtonBottom.setConstraint(new Anchor(Rect(30, 70, 30, 15), BottomBorder | LeftBorder | RightBorder));
  }

};

//////////////////////////////////////////////////////////////////////

class RefRectChild : public MyMdiChild
{
  GroupBox mGroupBox1;
  GroupBox mGroupBox2;

public:

  RefRectChild(String title, MdiFrame *parent)
    : MyMdiChild(title, parent)
    , mGroupBox1("All borders", this)
    , mGroupBox2("None", this)
  {
    setLayout(new AnchorLayout(Size(40, 40)));
    mGroupBox1.setConstraint(new Anchor(Rect(0, 0, 40, 40), AllBorders));
    mGroupBox2.setConstraint(new Anchor(Rect(0, 0, 40, 40), NoBorder));
  }

};

//////////////////////////////////////////////////////////////////////

class MainFrame : public MdiFrame
{
  AnchorChild child1;
  AnchorChild child2;
  AnchorChild child3;
  AnchorChild child4;
  AnchorChild child5;
  AnchorChild child6;
  MixedChild child7;
  RefRectChild child8;

public:

  MainFrame()
    : MdiFrame("AnchorLayouts")
    , child1("Without anchors", NoBorder, this)
    , child2("With left, top, and bottom anchors", LeftBorder | TopBorder | BottomBorder, this)
    , child3("With right, top, and bottom anchors", RightBorder | TopBorder | BottomBorder, this)
    , child4("With top, left, and right anchors", TopBorder | LeftBorder | RightBorder, this)
    , child5("With bottom, left, and right anchors", BottomBorder | LeftBorder | RightBorder, this)
    , child6("With all anchors", AllBorders, this)
    , child7("Mixed anchors", this)
    , child8("References", this)
  {
  }

};

//////////////////////////////////////////////////////////////////////

class Example : public Application
{
  MainFrame mMainWnd;
public:
  virtual void main(std::vector<String> args) {
    mMainWnd.setVisible(true);
    mMainWnd.getMdiClient().tileHorizontal();
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
