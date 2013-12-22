// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_FONTDIALOG_H
#define VACA_FONTDIALOG_H

#include "Vaca/base.h"
#include "Vaca/CommonDialog.h"
#include "Vaca/Font.h"

namespace Vaca {

/**
   A common dialog box to select fonts.

   @see Font
*/
class VACA_DLL FontDialog : public CommonDialog
{
  Font m_font;
  LOGFONT m_logFont;

public:

  FontDialog(const Font& font, Widget* parent);
  virtual ~FontDialog();

  virtual bool doModal();

  Font getFont() const;
  bool getLogFont(LPLOGFONT lplf) const;

};

} // namespace Vaca

#endif // VACA_FONTDIALOG_H
