#include <cassert>

#include "Vaca/SharedPtr.h"
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
  SharedPtr<Int> a(new Int(5));
  SharedPtr<Int> b;
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
