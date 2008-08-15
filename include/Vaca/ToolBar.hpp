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

#ifndef VACA_TOOLBAR_HPP
#define VACA_TOOLBAR_HPP

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
class ToolSet;

//////////////////////////////////////////////////////////////////////

struct ToolButtonStateEnumSet
{
  enum enumeration {
    None    = 0,
    Enabled = 1,
    Checked = 2,
    Hidden  = 4
  };
};

typedef EnumSet<ToolButtonStateEnumSet> ToolButtonState;

//////////////////////////////////////////////////////////////////////

class VACA_DLL ToolButton
{
  // ToolSet* m_set;
  int m_buttonIndex;
  int m_imageIndex;
  CommandId m_commandId;
  ToolButtonState m_state;

public:
  ToolButton(CommandId commandId, int imageIndex, ToolButtonState state = ToolButtonState::Enabled);
  ToolButton(const ToolButton& button);
  virtual ~ToolButton();

  // ToolSet* getSet() const { return m_set; }
  int getButtonIndex() const { return m_buttonIndex; }
  int getImageIndex() const { return m_imageIndex; }
  CommandId getCommandId() const { return m_commandId; }
  ToolButtonState getState() const { return m_state; }

  // void setSet(ToolSet* set);
  void setButtonIndex(int buttonIndex) { m_buttonIndex = buttonIndex; }
  void setImageIndex(int imageIndex) { m_imageIndex = imageIndex; }
  void setCommandId(CommandId commandId) { m_commandId = commandId; }
  void setState(ToolButtonState state);

  int getTBSTATE() const;
  // void setTBSTATE(int tbstate);

};
  
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

  ToolSet(Widget* parent, Style style = ToolSetStyle);
  virtual ~ToolSet();

  int getButtonCount();
  int getRows();
  Rect setRows(int rows, bool expand);

  void setImageList(ImageList& imageList);
  void loadStandardImageList(int imageListId = IDB_STD_SMALL_COLOR);

  void addButton(ToolButton* button);
  void addSeparator(int width = 6);

  void updateButton(ToolButton* button);

  ToolButton* getButtonById(CommandId id);
  ToolButton* getButtonByIndex(int index);

  int hitTest(const Point& pt);

  std::vector<Size> getPreferredSizes();
  void updatePreferredSizes();

protected:
  // events
  virtual void onPreferredSize(Size& sz);
  virtual void onUpdateIndicators();
  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

};

/**
 * A dockable tool bar. If you are looking for the original Win32
 * tool bar control, you should look at the ToolSet.
 */
class VACA_DLL ToolBar : public DockBar
{
  ToolSet m_set;
  int m_rowsWhenFloating;

public:

  ToolBar(const String& title, Frame* parent,
	  Style toolSetStyle = ToolSetStyle,
	  Style style = ToolBarStyle);
  virtual ~ToolBar();

  virtual Size getDockedSize(Side side);
  virtual Size getFloatingSize();

protected:
  // events
  virtual bool onCommand(CommandId id);
  virtual void onUpdateIndicators();
  virtual void onDocking();
  virtual void onFloating();
  virtual void onResizingFrame(DockFrame* frame, CardinalDirection dir, Rect& rc);

public:
  //////////////////////////////////////////////////////////////////////
  // Wrapper for the ToolSet

  /** @see ToolSet#getButtonCount */
  int getButtonCount() {
    return m_set.getButtonCount();
  }

  /** @see ToolSet#getRows */
  int getRows() {
    return m_set.getRows();
  }

  /** @see ToolSet#setRows */
  Rect setRows(int rows, bool expand) {
    return m_set.setRows(rows, expand);
  }

  /** @see ToolSet#setImageList */
  void setImageList(ImageList& imageList) {
    return m_set.setImageList(imageList);
  }

  /** @see ToolSet#loadStandardImageList */
  void loadStandardImageList(int imageListId = IDB_STD_SMALL_COLOR) {
    m_set.loadStandardImageList(imageListId);
  }

  /** @see ToolSet#addButton */
  void addButton(ToolButton* button) {
    m_set.addButton(button);
  }

  /** @see ToolSet#addSeparator */
  void addSeparator(int width = 6) {
    m_set.addSeparator(width);
  }

  /** @see ToolSet#updateButton */
  void updateButton(ToolButton* button) {
    m_set.updateButton(button);
  }

  /** @see ToolSet#getButtonById */
  ToolButton* getButtonById(CommandId id) {
    return m_set.getButtonById(id);
  }

  /** @see ToolSet#getButtonByIndex */
  ToolButton* getButtonByIndex(int index) {
    return m_set.getButtonByIndex(index);
  }

  /** @see ToolSet#hitTest */
  int hitTest(const Point& pt) {
    return m_set.hitTest(pt);
  }

};

} // namespace Vaca

#endif
