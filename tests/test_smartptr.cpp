#include <cassert>

#include "Vaca/SmartPtr.h"
#include "Vaca/Referenceable.h"

using namespace Vaca;

class Int : public Referenceable
{
public:
  int value;
  Int(int value) : value(value) { }
};

void test_equal()
{
  SmartPtr<Int> a(new Int(5));
  SmartPtr<Int> b;
  assert(a->value == 5);
  assert(a != b);
  b = a;
  assert(a == b);
  assert(&a->value == &b->value);
}

int main()
{
  test_equal();
  return 0;
}
