#include <cassert>

#include "Vaca/Vaca.h"

using namespace Vaca;

void test_double_wrapper_fail()
{
  Frame a(L"first wrapper");

  try {
    Widget b(a.getHandle());	// Second wrapper

    // CreateWidgetException wasn't throw
    assert(false);
  }
  catch (const CreateWidgetException&) {
    // Expected behavior
  }
  catch (...) {
    assert(false);		// Other kind of exception!!
  }
}

int main()
{
  Application app;
  test_double_wrapper_fail();
  return 0;
}
