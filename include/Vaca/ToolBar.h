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

#ifndef VACA_TOOLBAR_H
#define VACA_TOOLBAR_H

#include <vector>

#include "Vaca/base.h"
#include "Vaca/DockBar.h"
#include "Vaca/ImageList.h"

namespace Vaca {

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

/// A button inside a ToolSet.
/// 
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

/// A set of tool buttons.
/// 
/// @image html ToolSet.png
/// 
/// @warning
///   This is not a dockable tool bar. This is only a toolbar
///   as Microsoft defines like "a window that contains one or
///   more buttons". If you want a really dockable tool bar,
///   you should use the ToolBar class.
/// 
/// @see ToolButton, ToolBar
/// 
class VACA_DLL ToolSet : public Widget
{
  std::vector<Size> m_preferredSizes;
  ImageList m_imageList;

public:

  struct VACA_DLL Styles {
    static const Style Default;
    static const Style Flat;
  };

  ToolSet(Widget* parent, Style style = Styles::Default);
  virtual ~ToolSet();

  int getButtonCount() const;
  int getRows() const;
  Rect setRows(int rows, bool expand);

  void setImageList(ImageList& imageList);

  void addButton(ToolButton* button);
  void addSeparator(int width = 6);

  void updateButton(ToolButton* button);

  ToolButton* getButtonById(CommandId id) const;
  ToolButton* getButtonByIndex(int index) const;

  int hitTest(const Point& pt) const;

  std::vector<Size> getPreferredSizes() const;
  void updatePreferredSizes();

protected:
  // events
  virtual void onPreferredSize(Size& sz);
  virtual void onUpdateIndicators();
  // reflection
  virtual bool onReflectedCommand(int id, int code, LRESULT& lResult);
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

};

/// A dockable tool bar.
/// 
/// @image html ToolBar.png
/// 
/// @warning
///   This class uses a ToolSet to show @link ToolButton ToolButtons@endlink,
///   but a ToolSet is static, is just a set of buttons, ToolBar offers you
///   a dockable tool bar.
/// 
class VACA_DLL ToolBar : public DockBar
{
  ToolSet m_set;
  int m_rowsWhenFloating;

public:

  struct VACA_DLL Styles {
    static const Style Default;
  };

  ToolBar(const String& title, Frame* parent,
	  Style toolSetStyle = ToolSet::Styles::Default,
	  Style style = ToolBar::Styles::Default);
  virtual ~ToolBar();

  virtual Size getDockedSize(Side side) const;
  virtual Size getFloatingSize() const;

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

  /// @see ToolSet#getButtonCount
  int getButtonCount() {
    return m_set.getButtonCount();
  }

  /// @see ToolSet#getRows
  int getRows() {
    return m_set.getRows();
  }

  /// @see ToolSet#setRows
  Rect setRows(int rows, bool expand) {
    return m_set.setRows(rows, expand);
  }

  /// @see ToolSet#setImageList
  void setImageList(ImageList& imageList) {
    return m_set.setImageList(imageList);
  }

  /// @see ToolSet#addButton
  void addButton(ToolButton* button) {
    m_set.addButton(button);
  }

  /// @see ToolSet#addSeparator
  void addSeparator(int width = 6) {
    m_set.addSeparator(width);
  }

  /// @see ToolSet#updateButton
  void updateButton(ToolButton* button) {
    m_set.updateButton(button);
  }

  /// @see ToolSet#getButtonById
  ToolButton* getButtonById(CommandId id) {
    return m_set.getButtonById(id);
  }

  /// @see ToolSet#getButtonByIndex
  ToolButton* getButtonByIndex(int index) {
    return m_set.getButtonByIndex(index);
  }

  /// @see ToolSet#hitTest
  int hitTest(const Point& pt) {
    return m_set.hitTest(pt);
  }

};

} // namespace Vaca

#endif // VACA_TOOLBAR_H
