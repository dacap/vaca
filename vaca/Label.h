// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_LABEL_H
#define VACA_LABEL_H

#include "Vaca/base.h"
#include "Vaca/Widget.h"

namespace Vaca {

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
    static const Style Default;
    static const Style WordEllipsis;
    static const Style EndEllipsis;
    static const Style PathEllipsis;
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

} // namespace Vaca

#endif // VACA_LABEL_H
