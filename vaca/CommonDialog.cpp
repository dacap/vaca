// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/CommonDialog.h"
#include "Vaca/Widget.h"
#include "Vaca/Debug.h"
#include "Vaca/Application.h"

using namespace Vaca;

CommonDialog::CommonDialog(Widget* parent)
{
  m_parent = parent;
}

CommonDialog::~CommonDialog()
{
}

Widget* CommonDialog::getParent()
{
  return m_parent;
}

HWND CommonDialog::getParentHandle()
{
  return m_parent != NULL ? m_parent->getHandle(): NULL;
}
