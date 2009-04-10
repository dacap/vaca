// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2009 David Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the author nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Vaca/ChildEvent.h"
#include "Vaca/Widget.h"

using namespace Vaca;

/// Creates a new ChildEvent specifying that @a child was added or
/// removed in @a source.
///
/// @param source
///   Parent widget.
/// @param child
///   Child widget that was added or removed from the parent @a source widget.
/// @param add
///   True if the @a child was added, or false if it was removed from @a source.
///
ChildEvent::ChildEvent(Widget* source, Widget* child, bool add)
  : Event(source)
  , m_child(child)
  , m_add(add)
{
}

/// Destroys the ChildEvent.
///
ChildEvent::~ChildEvent()
{
}

/// Returns true if this event specifies that child widget was added in
/// the source.
/// 
/// @see Event#getSource, #getChild
///
bool ChildEvent::isAdd() const
{
  return m_add;
}

/// Returns true if this event specifies that child widget was removed
/// from the source.
///
/// @see Event#getSource, #getChild
///
bool ChildEvent::isRemove() const
{
  return !m_add;
}

/// Returns the widget that was added or removed from the source of the event.
///
/// @see Event#getSource
///
Widget* ChildEvent::getChild() const
{
  return m_child;
}
