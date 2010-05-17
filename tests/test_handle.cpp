#include <gtest/gtest.h>

#include "Vaca/Vaca.h"

using namespace Vaca;

TEST(Handle, DoubleWrapperFail)
{
  Application app;

  // First wrapper
  Frame a(L"Title");

  // Second wrapper
  EXPECT_THROW(Widget b(a.getHandle()),
	       CreateWidgetException);
}
