// Vaca - Visual Application Components Abstraction
// Copyright (c) 2008, 2009, Jie Zhang, David Capello
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

#ifndef VACA_REBAR_H
#define VACA_REBAR_H

#include "Vaca/base.h"
#include "Vaca/ImageList.h"
#include "Vaca/Widget.h"

namespace Vaca {

struct ReBarBandStyleEnumSet
{
  enum {
    Break		/*RBBS_BREAK*/		= 0x0001,
    FixedSize		/*RBBS_FIXEDSIZE*/	= 0x0002,
    ChildEdge		/*RBBS_CHILDEDGE*/	= 0x0004,
    Hidden		/*RBBS_HIDDEN*/		= 0x0008,
    NoVert		/*RBBS_NOVERT*/		= 0x0010,
    FixedBmp		/*RBBS_FIXEDBMP*/	= 0x0020,
    VariableHeight	/*RBBS_VARIABLEHEIGHT*/	= 0x0040,
    GripperAlways	/*RBBS_GRIPPERALWAYS*/	= 0x0080,
    NoGripper		/*RBBS_NOGRIPPER*/	= 0x0100,
    UseChevron		/*RBBS_USECHEVRON*/	= 0x0200,
    HideTitle		/*RBBS_HIDETITLE*/	= 0x0400,
    TopAlign		/*RBBS_TOPALIGN*/	= 0x0800,

    Simple = ChildEdge | GripperAlways
  };
};

typedef EnumSet<ReBarBandStyleEnumSet> ReBarBandStyle;

class ReBarException : public Exception
{
public:
  ReBarException() : Exception() { }
  ReBarException(const String& message) : Exception(message) { }
  virtual ~ReBarException() throw() { }
};

class ReBar;

class VACA_DLL ReBarBand
{
  ReBar* m_rebar;
  int m_index;

public:
  ReBarBand(ReBar* parent, int index);
  ReBarBand(const ReBarBand& band);
  virtual ~ReBarBand();

  ReBarBandStyle getStyle();
  void setStyle(ReBarBandStyle style);

  void getColors(Color& fg, Color& bg);
  void setColors(Color fg, Color bg);

  // String getText() const;
  void setText(const String& text);

  int getImageIndex();
  void setImageIndex(int index);

  Widget* getChild();
  void setChild(Widget* widget);

  inline void operator=(const ReBarBand& rbb) {
    m_rebar = rbb.m_rebar;
    m_index = rbb.m_index;
  }

protected:
  void setBand(REBARBANDINFO* rbbi);
  void getBand(REBARBANDINFO* rbbi);
};

class VACA_DLL ReBar : public Widget
{
  ImageList m_imageList;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };
  
  ReBar(Widget* parent, Style style = ReBar::Styles::Default);
  virtual ~ReBar();

  void setImageList(ImageList& imageList);

  int getBandCount();
  int getBarHeight();

  ReBarBand addBand(Widget* item, ReBarBandStyle style = ReBarBandStyle::Simple, int position=-1);
  ReBarBand getBand(int index);
  void removeBand(int index);

  Color getBgColor();
  Color getFgColor();

  virtual void setBgColor(const Color& color);
  virtual void setFgColor(const Color& color);

  Rect getBandRect(int index);
  int getRowCount();
  int getRowHeight(int nRow);
  
  void showBand(int index, bool show);

  void maximizeBand(int index, bool ideal = false);
  void minimizeBand(int index);

public:
  Signal1<void, Event&> AutoSize;

protected:
  virtual bool onReflectedNotify(LPNMHDR lpnmhdr, LRESULT& lResult);
  virtual void onAutoSize(Event& ev);

};

} // namespace Vaca

#endif // VACA_REBAR_H
