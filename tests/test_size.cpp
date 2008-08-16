#include <cassert>

#include "Vaca/Size.h"

using namespace Vaca;

void test_size()
{
  Size s1;
  assert(s1.w == 0 && s1.h == 0);

  Size s2(32, 40);
  assert(s2.w == 32 && s2.h == 40);

  assert(s1 != s2);

  s1 = s2;
  assert(s1 == s2);

  s1 += s2;
  assert(s1.w = 64 && s1.h == 80);

  s1 /= 4;
  assert(s1.w = 16 && s1.h == 20);

  s1 += 4;
  assert(s1.w = 20 && s1.h == 24);

  s1 *= 2;
  assert(s1.w = 40 && s1.h == 48);

  s1 -= s2;
  assert(s1.w = 8 && s1.h == 8);

  assert(Size(3, 3)+Size(5, 1) == Size(8, 4));
  assert(Size(3, 2)-Size(1, 1) == Size(2, 1));
  assert(-Size(1, 3) == Size(-1, -3));

  assert(Size(1, 2).createUnion(Size(4, 1)) == Size(4, 2));
  assert(Size(1, 2).createIntersect(Size(4, 1)) == Size(1, 1));
}

int main()
{
  test_size();
  return 0;
}
