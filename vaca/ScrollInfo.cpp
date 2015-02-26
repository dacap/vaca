// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/ScrollInfo.h"

using namespace vaca;

ScrollInfo::ScrollInfo()
{
  m_minPos = m_maxPos = m_pageSize = 0;
}

ScrollInfo::ScrollInfo(int minPos, int maxPos, int pageSize)
  : m_minPos(minPos)
  , m_maxPos(maxPos)
  , m_pageSize(pageSize)
{
}

ScrollInfo::~ScrollInfo()
{
}

int ScrollInfo::getMinPos() const
{
  return m_minPos;
}

int ScrollInfo::getMaxPos() const
{
  return m_maxPos;
}

int ScrollInfo::getPageSize() const
{
  return m_pageSize;
}

void ScrollInfo::setMinPos(int minPos)
{
  m_minPos = minPos;
}

void ScrollInfo::setMaxPos(int maxPos)
{
  m_maxPos = maxPos;
}

void ScrollInfo::setPageSize(int pageSize)
{
  m_pageSize = pageSize;
}
