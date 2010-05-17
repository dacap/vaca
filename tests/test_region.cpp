#include <gtest/gtest.h>
#include <cstdio>

#include "Vaca/Region.h"
#include "Vaca/Rect.h"
#include "Vaca/Size.h"
#include "Vaca/TimePoint.h"

using namespace Vaca;

inline std::ostream& operator<<(std::ostream& os, const Rect& rc)
{
  return os << "Rect(" 
	    << rc.x << ", "
	    << rc.y << ", "
	    << rc.w << ", "
	    << rc.h << ")\n";
}

TEST(Region, Simple)
{
  Region rgn;
  EXPECT_TRUE(rgn.isEmpty());

  rgn |= Region::fromRect(Rect(5, 5, 25, 25));
  EXPECT_TRUE(rgn.isSimple());
  EXPECT_TRUE(rgn.getBounds() == Rect(5, 5, 25, 25));
}

TEST(Region, Complex)
{
  Region r1;
  r1 |= Region::fromRect(Rect(5, 5, 25, 25));
  r1 |= Region::fromRect(Rect(20, 20, 20, 20));
  EXPECT_TRUE(r1.getBounds() == Rect(5, 5, 35, 35));
}

TEST(Region, Time)
{
  double seconds_accum = 0.0;

  for (int c=0; c<1000; ++c) {
    TimePoint pt;
    {
      Region r1;
      r1 |= Region::fromRect(Rect(5, 5, 25, 25));
      r1 |= Region::fromRect(Rect(20, 20, 20, 20));
      r1 += Region::fromRect(Rect(20, 20, 25, 25));
      r1 -= Region::fromRect(Rect(20, 20, 20, 20));
      r1 ^= Region::fromEllipse(Rect(0, 0, 100, 100));
      r1 &= Region::fromRoundRect(Rect(0, 0, 100, 100), Size(16, 16));
    }
    seconds_accum += pt.elapsed();
  }

  std::printf("seconds_accum = %.16g\n", seconds_accum / 1000.0);
}

TEST(Region, GeometricOperations)
{
  Region r1 = Region::fromRect(Rect(5, 5, 25, 25));
  Region r2 = Region::fromRect(Rect(20, 20, 20, 20));

  EXPECT_TRUE((r1 | r2).getBounds() == Rect(5, 5, 35, 35));
  EXPECT_TRUE((r1 + r2).getBounds() == Rect(5, 5, 35, 35));
  EXPECT_TRUE((r1 - r2).getBounds() == Rect(5, 5, 25, 25));
  EXPECT_TRUE((r1 & r2).getBounds() == Rect(20, 20, 10, 10));
  EXPECT_TRUE((r1 ^ r2).getBounds() == Rect(5, 5, 35, 35));

  EXPECT_TRUE((r1 | r2) == (r2 | r1));
  EXPECT_TRUE((r1 | r2) == (r1 + r2));
  EXPECT_TRUE((r1 & r2) == (r2 & r1));
  EXPECT_TRUE((r1 ^ r2) == (r2 ^ r1));
  EXPECT_TRUE((r1 - r2) != (r2 - r1));
  EXPECT_TRUE((r1 - r2) == (Region::fromRect(Rect(5, 5, 25, 15)) |
		       Region::fromRect(Rect(5, 5, 15, 25))));
  EXPECT_TRUE((r1 - r2) == (Region::fromRect(Rect(5, 5, 15, 25)) |
		       Region::fromRect(Rect(5, 5, 25, 15))));

  EXPECT_TRUE((r1 | r2) == ((r1 ^ r2) | (r1 | r2)));
}

TEST(Region, References)
{
  Region a = Region::fromRect(Rect(5, 5, 25, 25));
  Region b = a;
  Region c = a.clone();

  a |= Region::fromRect(Rect(20, 20, 20, 20));

  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a != c);
  EXPECT_TRUE(b != c);
}
