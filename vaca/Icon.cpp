// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#include "vaca/Icon.h"
#include "vaca/Application.h"
#include "vaca/Debug.h"
#include "vaca/ResourceException.h"
#include "vaca/String.h"

using namespace vaca;

#define GdiObj GdiObject<HCURSOR, Win32DestroyIcon>

/**
   Creates a NULL icon.
*/
Icon::Icon()
{
}

/**
   Makes a new pointer to the specified icon.

   @param icon
     Icon to be referenced.
*/
Icon::Icon(const Icon& icon)
  : SharedPtr<GdiObj>(icon)
{
}

/**
   Loads an @msdn{ICON} resource.

   @param iconId
     The ID of the resource to be loaded.
   @param sz
     The size of the specific icon that you want to load.

   @throw ResourceException
     When the resource with ID @a iconId wasn't found.
*/
Icon::Icon(ResourceId iconId, const Size& sz)
  : SharedPtr<GdiObj>(new GdiObj)
{
  HICON handle = reinterpret_cast<HICON>
    (::LoadImage(Application::getHandle(),
		 MAKEINTRESOURCE(iconId.getId()),
		 IMAGE_ICON,
		 sz.w, sz.h, 0));
  if (handle == NULL)
    throw ResourceException(format_string(L"Can't load the icon resource %d", iconId.getId()));

  get()->setHandle(handle);
}

/**
   Loads an icon from a file.

   @param fileName
     The name of the file to be loaded.
   @param sz
     The size of the specific icon that you want to load.

   @throw ResourceException
     When the icon couldn't be loaded from the specified @a fileName.
*/
Icon::Icon(const String& fileName, const Size& sz)
  : SharedPtr<GdiObj>(new GdiObj)
{
  HICON handle = reinterpret_cast<HICON>
    (::LoadImage(Application::getHandle(),
		 fileName.c_str(),
		 IMAGE_ICON,
		 sz.w, sz.h, LR_LOADFROMFILE));
  if (handle == NULL)
    throw ResourceException(L"Can't load icon from file " + fileName);

  get()->setHandle(handle);
}

/**
   Wraps the specified handle.

   @warning The @a handle will be deleted automatically.

   @param handle
     A @msdn{HICON} handle.
*/
Icon::Icon(HICON handle)
  : SharedPtr<GdiObj>(new GdiObj(handle))
{
}

/**
   Deletes the wrapped @msdn{HICON} handle.
*/
Icon::~Icon()
{
}

/**
   Returns the @msdn{HICON} handle.

   @warning Can be NULL if you create
*/
HICON Icon::getHandle() const
{
  return get()->getHandle();
}
