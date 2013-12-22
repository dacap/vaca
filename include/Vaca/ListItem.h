// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_LISTITEM_H
#define VACA_LISTITEM_H

#include "Vaca/base.h"
#include "Vaca/Component.h"

#include <vector>

namespace Vaca {

/**
   An item in a ListView.
*/
class VACA_DLL ListItem : public Component
{
  friend class ListView;

  int			m_index;
  std::vector<String>	m_text;
  int			m_image;
  ListView*		m_owner;

public:

  ListItem(const String& text = L"", int imageIndex = -1);
  virtual ~ListItem();

  ListView* getListView();
  int getIndex();
  Rect getBounds() const;

  virtual String getText(size_t columnIndex = 0);
  virtual int getImage();

  void setText(const String& text, size_t columnIndex = 0);
  void setImage(int image);

  void update();

  bool isSelected() const;
  void setSelected(bool state);

  void ensureVisible();

private:
  void addToListView(ListView* listView);
  void removeFromListView();

};

} // namespace Vaca

#endif // VACA_LISTITEM_H
