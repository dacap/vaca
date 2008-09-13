#include <cassert>
#include <cstdio>

#include "Vaca/Region.h"
#include "Vaca/Rect.h"
#include "Vaca/Size.h"
#include "Vaca/TimePoint.h"

using namespace Vaca;

void test_simple()
{
  Region rgn;
  assert(rgn.isEmpty());

  rgn |= Region::fromRect(Rect(5, 5, 25, 25));
  assert(rgn.isSimple());
  assert(rgn.getBounds() == Rect(5, 5, 25, 25));
}

void test_complex()
{
  Region r1;
  r1 |= Region::fromRect(Rect(5, 5, 25, 25));
  r1 |= Region::fromRect(Rect(20, 20, 20, 20));
  assert(r1.getBounds() == Rect(5, 5, 35, 35));
}

void test_time()
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

void test_geometric_ops()
{
  Region r1 = Region::fromRect(Rect(5, 5, 25, 25));
  Region r2 = Region::fromRect(Rect(20, 20, 20, 20));

  assert((r1 | r2).getBounds() == Rect(5, 5, 35, 35));
  assert((r1 + r2).getBounds() == Rect(5, 5, 35, 35));
  assert((r1 - r2).getBounds() == Rect(5, 5, 25, 25));
  assert((r1 & r2).getBounds() == Rect(20, 20, 10, 10));
  assert((r1 ^ r2).getBounds() == Rect(5, 5, 35, 35));

  assert((r1 | r2) == (r2 | r1));
  assert((r1 | r2) == (r1 + r2));
  assert((r1 & r2) == (r2 & r1));
  assert((r1 ^ r2) == (r2 ^ r1));
  assert((r1 - r2) != (r2 - r1));
  assert((r1 - r2) == (Region::fromRect(Rect(5, 5, 25, 15)) |
		       Region::fromRect(Rect(5, 5, 15, 25))));
  assert((r1 - r2) == (Region::fromRect(Rect(5, 5, 15, 25)) |
		       Region::fromRect(Rect(5, 5, 25, 15))));

  assert((r1 | r2) == ((r1 ^ r2) | (r1 | r2)));
}

void test_references()
{
  Region a = Region::fromRect(Rect(5, 5, 25, 25));
  Region b = a;
  Region c = a.clone();

  a |= Region::fromRect(Rect(20, 20, 20, 20));

  assert(a == b);
  assert(a != c);
  assert(b != c);
}

int main()
{
  test_simple();
  test_complex();
  test_geometric_ops();
  test_references();
  return 0;
}
