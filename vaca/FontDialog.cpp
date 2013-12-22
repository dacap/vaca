// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/FontDialog.h"
#include "Vaca/Font.h"
#include "Vaca/Widget.h"
#include "Vaca/Debug.h"
#include "Vaca/Application.h"

using namespace Vaca;

FontDialog::FontDialog(const Font& font, Widget* parent)
  : CommonDialog(parent)
  , m_font(font)
{
}

FontDialog::~FontDialog()
{
}

bool FontDialog::doModal()
{
  CHOOSEFONT cf;

  cf.lStructSize = sizeof(CHOOSEFONT);
  cf.hwndOwner = getParentHandle();
  cf.hDC = NULL;
  cf.lpLogFont = &m_logFont;
  cf.iPointSize = 0; 
  cf.Flags = CF_SCREENFONTS | CF_LIMITSIZE;
  cf.rgbColors = RGB(0,0,0); 
  cf.lCustData = 0L; 
  cf.lpfnHook = (LPCFHOOKPROC)NULL; 
  cf.lpTemplateName = (LPTSTR)NULL; 
  cf.hInstance = (HINSTANCE)Application::getHandle();
  cf.lpszStyle = (LPTSTR)NULL; 
  cf.nFontType = SCREEN_FONTTYPE; 
  cf.nSizeMin = 4; 
  cf.nSizeMax = 72; 

  if (m_font.getLogFont(&m_logFont))
    cf.Flags |= CF_INITTOLOGFONTSTRUCT;

  if (ChooseFont(&cf)) {
    m_font = Font(&m_logFont);
    return true;
  }
  else
    return false;
}

Font FontDialog::getFont() const
{
  return m_font;
}

bool FontDialog::getLogFont(LPLOGFONT lplf) const
{
  CopyMemory(lplf, &m_logFont, sizeof(LOGFONT));
  return true;
}
