#include <cassert>

#include "Vaca/Rect.h"
#include "Vaca/Point.h"
#include "Vaca/Size.h"

using namespace Vaca;

void test_rect_ctors_getters()
{
  Rect r1;
  Rect r2(32, 64);
  Rect r3(Size(32, 64));
  Rect r4(Point(2, 2), Size(32, 64));
  Rect r5(Point(2, 2), Point(34, 66));
  Rect r6(2, 2, 32, 64);

  assert(r1.isEmpty());
  assert(r1.x == 0 && r1.y == 0 && r1.w == 0 && r1.h == 0);
  assert(r2.x == 0 && r2.y == 0 && r2.w == 32 && r2.h == 64);
  assert(r3.x == 0 && r3.y == 0 && r3.w == 32 && r3.h == 64);
  assert(r2 == r3);
  assert(r4 == r5);
  assert(r5 == r6);
  assert(r1 != r2);
  assert(r3 != r4);
  assert(r2.getSize() == r4.getSize());
  assert(r4.getOrigin() == r5.getOrigin());
  assert(r4.getCenter() == Point(18, 34));
  assert(r5.getPoint2() == Point(34, 66));
}

void test_rect_setters()
{
  Rect r;
  
  r.setOrigin(Point(1, 2));
  r.setSize(Size(3, 4));
  assert(r.x == 1 && r.y == 2 && r.w == 3 && r.h == 4);

  r.offset(Point(2, 3));
  assert(r.x == 3 && r.y == 5 && r.w == 3 && r.h == 4);

  r.inflate(Size(3, 2));
  assert(r.x == 3 && r.y == 5 && r.w == 6 && r.h == 6);

  r.enlarge(2);
  assert(r.x == 1 && r.y == 3 && r.w == 10 && r.h == 10);

  r.shrink(4);
  assert(r.x == 5 && r.y == 7 && r.w == 2 && r.h == 2);
}

void test_rect_conditions()
{
  Rect r(5, 10, 15, 20);

  assert(r == Rect(5, 10, 15, 20));
  assert(r != Rect(0, 10, 15, 20));
  assert(r != Rect(5,  0, 15, 20));
  assert(r != Rect(5, 10,  0, 20));
  assert(r != Rect(5, 10, 15,  0));

  assert(!r.contains(Point(4, 10)));
  assert(!r.contains(Point(5, 9)));
  assert(!r.contains(Point(20, 29)));
  assert(!r.contains(Point(19, 30)));
  assert(r.contains(Point(5, 10)));
  assert(r.contains(Point(19, 29)));

  assert(r.contains(Rect(5, 10, 1, 1)));
  assert(r.contains(Rect(5, 10, 15, 20)));
  assert(r.contains(Rect(19, 29, 1, 1)));
  assert(!r.contains(Rect(4, 10, 1, 1)));
  assert(!r.contains(Rect(5, 9, 1, 1)));
  assert(!r.contains(Rect(19, 30, 1, 1)));
  assert(!r.contains(Rect(20, 29, 1, 1)));
}

void test_rect_operations()
{
  Rect r1, r2;

  r1 = Rect(5, 10, 15, 20);
  r2 = Rect(4, 5, 6, 6);
  assert(r1.intersects(r2));
  assert(r1.createIntersect(r2) == Rect(5, 10, 5, 1));

  r1 = Rect(5, 10, 15, 20);
  r2 = Rect(2, 7, 3, 3);
  assert(!r1.intersects(r2));
  assert(r1.createIntersect(r2) == Rect(0, 0, 0, 0));

  r1 = Rect(5, 10, 15, 20);
  r2 = Rect(2, 7, 3, 3);
  assert(r1.createUnion(r2) == Rect(2, 7, 18, 23));
}

int main()
{
  test_rect_ctors_getters();
  test_rect_setters();
  test_rect_conditions();
  test_rect_operations();
  return 0;
}
