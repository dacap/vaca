// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_LINKLABEL_H
#define VACA_LINKLABEL_H

#include "vaca/base.h"
#include "vaca/CustomLabel.h"

namespace vaca {

/**
   A link to Internet (or whatever you want).
*/
class VACA_DLL LinkLabel : public CustomLabel
{
  enum State {
    Outside,
    Inside,
    Hover
  };

  State m_state;
  String m_url;
  Font m_underlineFont;
  Image* m_image;

public:

  struct VACA_DLL Styles {
    /**
       Default style for LinkLabel widget.

       It contains the following styles:
       @li CustomLabel::Styles::Default
       @li Widget::Styles::Focusable

       @win32
       It is
       @endwin32
    */
    static constexpr Style Default =
      CustomLabel::Styles::Default |
      Widget::Styles::Focusable;
  };

  LinkLabel(const String& urlOrText, Widget* parent, Style style = Styles::Default);
  LinkLabel(const String& url, const String& text, Widget* parent, Style style = Styles::Default);
  LinkLabel(const String& url, Image& image, Widget* parent, Style style = Styles::Default);
  virtual ~LinkLabel();

  virtual void setFont(Font font);

  virtual Color getLinkColor();
  virtual Color getHoverColor();

  Signal<void(Event&)> Click; ///< @see onClick

protected:

  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onPaint(PaintEvent& ev);
  virtual void onMouseEnter(MouseEvent& ev);
  virtual void onMouseMove(MouseEvent& ev);
  virtual void onMouseLeave(MouseEvent& ev);
  virtual void onMouseDown(MouseEvent& ev);
  virtual void onSetCursor(SetCursorEvent& ev);
//   virtual void onResize(ResizeEvent& ev);
  virtual void onFocusEnter(FocusEvent& ev);
  virtual void onFocusLeave(FocusEvent& ev);
  virtual void onKeyDown(KeyEvent& ev);

  // New events
  virtual void onClick(Event& ev);

private:

  void init(String text, Image* image = NULL);
  void click();
  void updateFont(const Font& font);
  Rect getLinkBounds(Graphics& g);

};

} // namespace vaca

#endif // VACA_LINKLABEL_H
