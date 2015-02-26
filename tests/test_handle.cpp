#include <gtest/gtest.h>

#include "vaca/vaca.h"

using namespace vaca;

TEST(Handle, DoubleWrapperFail)
{
  Application app;

  // First wrapper
  Frame a(L"Title");

  // Second wrapper
  EXPECT_THROW(Widget b(a.getHandle()),
	       CreateWidgetException);
}
