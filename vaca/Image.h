// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_IMAGE_H
#define VACA_IMAGE_H

#include "vaca/base.h"
#include "vaca/ResourceId.h"
#include "vaca/Size.h"
#include "vaca/GdiObject.h"
#include "vaca/SharedPtr.h"
#include "vaca/ImagePixels.h"

namespace vaca {

/**
   Used to destroy the HBITMAP handle of a Image.

   @internal
*/
class VACA_DLL ImageHandle : public GdiObject<HBITMAP>
{
  friend class Image;
  HDC m_hdc;
  Graphics* m_graphics;
public:
  ImageHandle();
  ImageHandle(HBITMAP handle);
  virtual ~ImageHandle();
};

/**
   A shared pointer to an image.

   This is a SharedPtr, so if you copy instances of images they will
   be referencing to the same place. You have to use Image#clone
   member function to create real copies of the Image.

   Example
   @code
   Image img1(32, 32);
   Image img2 = img1;		// img1 and img2 references the same Image
   Image img3 = img1.clone();
   assert(img1 == img2);
   assert(img1 != img3);
   @endcode

   @win32
     This is a @msdn{HBITMAP} wrapper.
   @endwin32
*/
class VACA_DLL Image : private SharedPtr<ImageHandle>
{
public:
  Image();
  explicit Image(ResourceId imageId);
  explicit Image(const String& fileName);
  explicit Image(const Size& sz);
  Image(int width, int height);
  Image(int width, int height, int depth);
  Image(const Size& sz, int depth);
  Image(const Size& sz, Graphics& g);
  Image(const Image& image);
  virtual ~Image();

  bool isValid() const { return get()->isValid(); }

  int getWidth() const;
  int getHeight() const;
  Size getSize() const;
  int getDepth() const;

  Graphics& getGraphics();

  ImagePixels getPixels() const;
  void setPixels(ImagePixels imagePixels);

  HBITMAP getHandle() const;

  Image& operator=(const Image& image);

  Image clone() const;

  bool operator==(const Image& image) const { return get() == image.get(); }
  bool operator!=(const Image& image) const { return get() != image.get(); }

private:

  void init(int width, int height);
  void init(int width, int height, int depth);
  void copyTo(Image& image) const;

};

} // namespace vaca

#endif // VACA_IMAGE_H
