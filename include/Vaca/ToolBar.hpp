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

#ifndef VACA_TOOLBAR_HPP
#define VACA_TOOLBAR_HPP

#include <commctrl.h>
#include <vector>

#include "Vaca/base.hpp"
#include "Vaca/DockBar.hpp"

namespace Vaca {

#if (_WIN32_IE >= 0x0300)

  #define ToolSetStyle	(ChildStyle + Style(CCS_NODIVIDER | CCS_NOPARENTALIGN | \
					    CCS_NOMOVEY | CCS_NORESIZE |	\
					    CCS_NOMOVEX, 0))

#else

  #define ToolSetStyle	(ChildStyle + Style(CCS_NODIVIDER | CCS_NOPARENTALIGN | \
					    CCS_NOMOVEY | CCS_NORESIZE, 0))

#endif

#define FlatToolSetStyle (Style(TBSTYLE_FLAT, 0))

#define ToolBarStyle	DockBarStyle

class ImageList;

/**
 * A Windows's tool bar of Comctl32.dll.
 *
 * @warning This is not a dockable tool bar. This is only a toolbar
 *          as Microsoft defines like "a window that contains one or
 *          more buttons". If you want a really dockable tool-bar,
 *          you should use the ToolBar class.
 */
class VACA_DLL ToolSet : public Widget
{
  std::vector<Size> m_preferredSizes;
  HIMAGELIST m_loadedImageList;

public:

  ToolSet(Widget *parent, Style style = ToolSetStyle);
  virtual ~ToolSet();

  int getButtonCount();
  int getRows();
  Rect setRows(int rows, bool expand);

  void setImageList(ImageList &imageList);
  void loadStandardImageList(int imageListId = IDB_STD_SMALL_COLOR);

  void addButton(int imageIndex, int commandId, int buttonState);
  void addToggleButton(int imageIndex, int commandId, int buttonState);
  // void addRadioButton(int imageIndex, int commandId, int buttonState);
  void addDropDownButton(int imageIndex, int commandId, int buttonState);
  void addWholeDropDownButton(int imageIndex, int commandId, int buttonState);
  void addSeparator(int width = 6);

  void setButtonCommandId(int buttonIndex, int newCommandId);

  int hitTest(const Point &pt);

  std::vector<Size> getPreferredSizes();
  void updatePreferredSizes();

protected:
  // events
  virtual void onPreferredSize(Size &sz);
  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT &lResult);
//   virtual void onMouseDown(MouseEvent &ev);
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult);

};

/**
 * A dockable tool bar. If you are looking for the original Win32
 * tool bar control, you should look at the ToolSet.
 */
class VACA_DLL ToolBar : public DockBar
{
  ToolSet mSet;
  int mRowsWhenFloating;

public:

  ToolBar(const String &title, Frame *parent,
	  Style toolSetStyle = ToolSetStyle,
	  Style style = ToolBarStyle);
  virtual ~ToolBar();

  ToolSet &getSet();

  virtual Size getDockedSize(Side side);
  virtual Size getFloatingSize();

protected:
  // events
  virtual bool onActionById(int actionId);
  virtual void onDocking();
  virtual void onFloating();
  virtual void onResizingFrame(DockFrame *frame, int edge, Rect &rc);

};

} // namespace Vaca

#endif
