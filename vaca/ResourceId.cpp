// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "Vaca/ResourceId.h"
#include "Vaca/String.h"

using namespace Vaca;

ResourceId::ResourceId(int id)
{
  m_id = id;
}

ResourceId::ResourceId(const ResourceId& rc)
{
  m_id = rc.m_id;
}

ResourceId::~ResourceId()
{
}

int ResourceId::getId() const
{
  return m_id;
}

String ResourceId::toString()
{
  return convert_to<String>(m_id);
}

LPTSTR ResourceId::toLPTSTR()
{
  return MAKEINTRESOURCE(m_id);
}
