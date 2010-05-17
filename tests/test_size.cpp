#include <gtest/gtest.h>

#include "Vaca/Size.h"

using namespace Vaca;

inline std::ostream& operator<<(std::ostream& os, const Size& sz)
{
  return os << "Size(" << sz.w << ", " << sz.h << ")\n";
}

TEST(Size, Basics)
{
  Size s1;
  EXPECT_EQ(0, s1.w);
  EXPECT_EQ(0, s1.h);

  Size s2(32, 40);
  EXPECT_EQ(32, s2.w);
  EXPECT_EQ(40, s2.h);

  EXPECT_TRUE(s1 != s2);

  s1 = s2;
  EXPECT_TRUE(s1 == s2);

  s1 += s2;
  EXPECT_EQ(64, s1.w);
  EXPECT_EQ(80, s1.h);

  s1 /= 4;
  EXPECT_EQ(16, s1.w);
  EXPECT_EQ(20, s1.h);

  s1 += 4;
  EXPECT_EQ(20, s1.w);
  EXPECT_EQ(24, s1.h);

  s1 *= 2;
  EXPECT_EQ(40, s1.w);
  EXPECT_EQ(48, s1.h);

  s1 -= s2;
  EXPECT_EQ(8, s1.w);
  EXPECT_EQ(8, s1.h);
}

TEST(Size, Operators)
{
  EXPECT_EQ(Size(-1, -3), -Size(1, 3));

  EXPECT_EQ(Size(8, 4), Size(3, 3)+Size(5, 1));
  EXPECT_EQ(Size(2, 1), Size(3, 2)-Size(1, 1));
}

TEST(Size, UnionAndIntersection)
{
  EXPECT_EQ(Size(4, 2), Size(1, 2).createUnion(Size(4, 1)));
  EXPECT_EQ(Size(1, 1), Size(1, 2).createIntersect(Size(4, 1)));
}
