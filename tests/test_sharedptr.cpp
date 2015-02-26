#include <gtest/gtest.h>

#include "vaca/SharedPtr.h"
#include "vaca/Referenceable.h"

using namespace vaca;

class Int : public Referenceable
{
public:
  int value_;
  Int(int value) : value_(value) { }
};

TEST(SharedPtr, Equal)
{
  SharedPtr<Int> a(new Int(5));
  SharedPtr<Int> b(new Int(0));
  SharedPtr<Int> c;

  EXPECT_EQ(5, a->value_);
  EXPECT_EQ(0, b->value_);

  b->value_ = a->value_;	// Same values

  EXPECT_TRUE(a != b);		// References are different
  EXPECT_TRUE(b != c);

  b = a;			// Same reference

  EXPECT_TRUE(a == b);
  EXPECT_TRUE(b != c);
  EXPECT_TRUE(&a->value_ == &b->value_); // Same address
}
