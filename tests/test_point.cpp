#include <cassert>

#include "Vaca/Point.h"

using namespace Vaca;

void test_point()
{
  Point p1;
  assert(p1.x == 0 && p1.y == 0);

  Point p2(32, 40);
  assert(p2.x == 32 && p2.y == 40);

  assert(p1 != p2);

  p1 = p2;
  assert(p1 == p2);

  p1 += p2;
  assert(p1.x = 64 && p1.y == 80);

  p1 /= 4;
  assert(p1.x = 16 && p1.y == 20);

  p1 += 4;
  assert(p1.x = 20 && p1.y == 24);

  p1 *= 2;
  assert(p1.x = 40 && p1.y == 48);

  p1 -= p2;
  assert(p1.x = 8 && p1.y == 8);

  assert(Point(3, 3)+Point(5, 1) == Point(8, 4));
  assert(Point(3, 2)-Point(1, 1) == Point(2, 1));
  assert(-Point(1, 3) == Point(-1, -3));
}

int main()
{
  test_point();
  return 0;
}
