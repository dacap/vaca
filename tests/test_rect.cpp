#include <gtest/gtest.h>

#include "vaca/Rect.h"
#include "vaca/Point.h"
#include "vaca/Size.h"

using namespace vaca;

inline std::ostream& operator<<(std::ostream& os, const Rect& rc)
{
  return os << "Rect(" 
	    << rc.x << ", "
	    << rc.y << ", "
	    << rc.w << ", "
	    << rc.h << ")\n";
}

TEST(Rect, Constructors)
{
  Rect r1;
  Rect r2(32, 64);
  Rect r3(Size(32, 64));
  Rect r4(Point(2, 2), Size(32, 64));
  Rect r5(Point(2, 2), Point(34, 66));
  Rect r6(2, 3, 32, 65);

  EXPECT_EQ(Rect(0, 0, 0, 0), r1);
  EXPECT_EQ(Rect(0, 0, 32, 64), r2);
  EXPECT_EQ(Rect(0, 0, 32, 64), r3);
  EXPECT_EQ(Rect(2, 2, 32, 64), r4);
  EXPECT_EQ(Rect(2, 2, 32, 64), r5);
  EXPECT_EQ(Rect(2, 3, 32, 65), r6);

  EXPECT_TRUE(r1.isEmpty());
  EXPECT_TRUE(r1.x == 0 && r1.y == 0 && r1.w == 0 && r1.h == 0);
  EXPECT_TRUE(r2.x == 0 && r2.y == 0 && r2.w == 32 && r2.h == 64);
  EXPECT_TRUE(r3.x == 0 && r3.y == 0 && r3.w == 32 && r3.h == 64);
  EXPECT_TRUE(r6.x == 2 && r6.y == 3 && r6.w == 32 && r6.h == 65);
  EXPECT_TRUE(r2 == r3);
  EXPECT_TRUE(r4 == r5);
  EXPECT_TRUE(r1 != r2);
  EXPECT_TRUE(r3 != r4);
  EXPECT_TRUE(r2.getSize() == r4.getSize());
  EXPECT_TRUE(r4.getOrigin() == r5.getOrigin());
  EXPECT_TRUE(r4.getCenter() == Point(18, 34));
  EXPECT_TRUE(r5.getPoint2() == Point(34, 66));
}

TEST(Rect, Setters)
{
  Rect r;
  
  r.setOrigin(Point(1, 2));
  r.setSize(Size(3, 4));
  EXPECT_EQ(Rect(1, 2, 3, 4), r);

  r.offset(Point(2, 3));
  EXPECT_EQ(Rect(3, 5, 3, 4), r);

  r.inflate(Size(3, 2));
  EXPECT_EQ(Rect(3, 5, 6, 6), r);

  r.enlarge(2);
  EXPECT_EQ(Rect(1, 3, 10, 10), r);

  r.shrink(4);
  EXPECT_EQ(Rect(5, 7, 2, 2), r);
}

TEST(Rect, Contains)
{
  Rect r(5, 10, 15, 20);

  EXPECT_TRUE(r == Rect(5, 10, 15, 20));
  EXPECT_TRUE(r != Rect(0, 10, 15, 20));
  EXPECT_TRUE(r != Rect(5,  0, 15, 20));
  EXPECT_TRUE(r != Rect(5, 10,  0, 20));
  EXPECT_TRUE(r != Rect(5, 10, 15,  0));

  EXPECT_TRUE(!r.contains(Point(4, 10)));
  EXPECT_TRUE(!r.contains(Point(5, 9)));
  EXPECT_TRUE(!r.contains(Point(20, 29)));
  EXPECT_TRUE(!r.contains(Point(19, 30)));
  EXPECT_TRUE(r.contains(Point(5, 10)));
  EXPECT_TRUE(r.contains(Point(19, 29)));

  EXPECT_TRUE(r.contains(Rect(5, 10, 1, 1)));
  EXPECT_TRUE(r.contains(Rect(5, 10, 15, 20)));
  EXPECT_TRUE(r.contains(Rect(19, 29, 1, 1)));
  EXPECT_TRUE(!r.contains(Rect(4, 10, 1, 1)));
  EXPECT_TRUE(!r.contains(Rect(5, 9, 1, 1)));
  EXPECT_TRUE(!r.contains(Rect(19, 30, 1, 1)));
  EXPECT_TRUE(!r.contains(Rect(20, 29, 1, 1)));
}

TEST(Rect, UnionAndIntersection)
{
  Rect r1, r2;

  r1 = Rect(5, 10, 15, 20);
  r2 = Rect(4, 5, 6, 6);
  EXPECT_TRUE(r1.intersects(r2));
  EXPECT_TRUE(r1.createIntersect(r2) == Rect(5, 10, 5, 1));

  r1 = Rect(5, 10, 15, 20);
  r2 = Rect(2, 7, 3, 3);
  EXPECT_TRUE(!r1.intersects(r2));
  EXPECT_TRUE(r1.createIntersect(r2) == Rect(0, 0, 0, 0));

  r1 = Rect(5, 10, 15, 20);
  r2 = Rect(2, 7, 3, 3);
  EXPECT_TRUE(r1.createUnion(r2) == Rect(2, 7, 18, 23));
}
