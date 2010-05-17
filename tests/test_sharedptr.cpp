#include <gtest/gtest.h>

#include "Vaca/SharedPtr.h"
#include "Vaca/Referenceable.h"

using namespace Vaca;

class Int : public Referenceable
{
public:
  int value_;
  Int(int value) : value_(value) { }
};

TEST(SharedPtr, Equal)
{
  SharedPtr<Int> a(new Int(5));
  SharedPtr<Int> b;
  EXPECT_EQ(5, a->value_);
  EXPECT_TRUE(a != b);
  b = a;
  EXPECT_TRUE(a == b);
  EXPECT_TRUE(&a->value_ == &b->value_); // Same address
}
