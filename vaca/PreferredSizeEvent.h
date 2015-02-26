// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_PREFERREDSIZEEVENT_H
#define VACA_PREFERREDSIZEEVENT_H

#include "vaca/base.h"
#include "vaca/Event.h"
#include "vaca/Size.h"

namespace vaca {

class VACA_DLL PreferredSizeEvent : public Event
{
  Size m_fitIn;
  Size m_preferredSize;

public:

  PreferredSizeEvent(Widget* source, const Size& fitIn);
  virtual ~PreferredSizeEvent();

  Size fitInSize() const;
  int fitInWidth() const;
  int fitInHeight() const;

  Size getPreferredSize() const;
  void setPreferredSize(const Size& preferredSize);
  void setPreferredSize(int w, int h);

};

} // namespace vaca

#endif // VACA_PREFERREDSIZEEVENT_H
