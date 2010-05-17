#include <gtest/gtest.h>

#include "Vaca/Point.h"

using namespace Vaca;

inline std::ostream& operator<<(std::ostream& os, const Point& pt)
{
  return os << "Point(" << pt.x << ", " << pt.y << ")\n";
}

TEST(Point, Basics)
{
  Point p1;
  EXPECT_EQ(0, p1.x);
  EXPECT_EQ(0, p1.y);

  Point p2(32, 40);
  EXPECT_EQ(32, p2.x);
  EXPECT_EQ(40, p2.y);

  EXPECT_TRUE(p1 != p2);

  p1 = p2;
  EXPECT_TRUE(p1 == p2);

  p1 += p2;
  EXPECT_EQ(64, p1.x);
  EXPECT_EQ(80, p1.y);

  p1 /= 4;
  EXPECT_EQ(16, p1.x);
  EXPECT_EQ(20, p1.y);

  p1 += 4;
  EXPECT_EQ(20, p1.x);
  EXPECT_EQ(24, p1.y);

  p1 *= 2;
  EXPECT_EQ(40, p1.x);
  EXPECT_EQ(48, p1.y);

  p1 -= p2;
  EXPECT_EQ(8, p1.x);
  EXPECT_EQ(8, p1.y);
}

TEST(Point, Operators)
{
  EXPECT_EQ(Point(-1, -3), -Point(1, 3));

  EXPECT_EQ(Point(8, 4), Point(3, 3)+Point(5, 1));
  EXPECT_EQ(Point(2, 1), Point(3, 2)-Point(1, 1));
}
