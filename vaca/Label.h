// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_LABEL_H
#define VACA_LABEL_H

#include "vaca/base.h"
#include "vaca/Widget.h"

namespace vaca {

/**
   A static label of text. It's just an informative label. Remember to
   pass the LabelStyle to @link Label#Label Label's constructor@endlink
   if you derived this class.

   @warning
     For people that want to mix up Win32 @c SS_ styles with the ones
     in @c LabelStyles, you should be aware to use them because some
     can overlap the @c SS_CENTER or @c SS_RIGHT, so you should override
     #getTextAlign and #getTextAlign member functions (like CustomLabel does).
*/
class VACA_DLL Label : public Widget
{
public:

  struct VACA_DLL Styles {

    /**
       Default style for Label.

       @win32
       It is WS_VISIBLE | SS_NOTIFY
       @endwin32
    */
    static constexpr Style Default =
      Widget::Styles::Visible |
      Style(SS_NOTIFY, 0);

    /**
       If the label is too long, ellipsis ("...") is used at the end of
       each word instead of wrap them.

       @win32
       @msdn{SS_WORDELLIPSIS}
       @endwin32
    */
    static constexpr Style WordEllipsis = Style(SS_WORDELLIPSIS, 0);

    /**
       If the label is too long, ellipsis ("...") is used at the end of
       the text. The text is not wrapped.

       @win32
       @msdn{SS_ENDELLIPSIS}
       @endwin32
    */
    static constexpr Style EndEllipsis = Style(SS_ENDELLIPSIS, 0);

    /**
       If the label is too long, ellipsis ("...") is used in the middle of
       the text. It is useful to show file name paths, because the
       ellipsis is used trying to show the file name part
       (String#getFileName). The text is not wrapped.

       @win32
       @msdn{SS_PATHELLIPSIS}
       @endwin32
    */
    static constexpr Style PathEllipsis = Style(SS_PATHELLIPSIS, 0);

  };

  Label(const String& text, Widget* parent, Style style = Styles::Default);
  explicit Label(HWND handle);
  virtual ~Label();

  virtual TextAlign getTextAlign() const;
  virtual void setTextAlign(TextAlign align);

protected:

  bool useWordWrap();
  int getFlagsForDrawString();

  // Events
  virtual void onPreferredSize(PreferredSizeEvent& ev);
  virtual void onResize(ResizeEvent& ev);

};

} // namespace vaca

#endif // VACA_LABEL_H
