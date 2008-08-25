#include <cassert>

#include "Vaca/Vaca.h"

using namespace Vaca;

void test_label()
{
  MenuItem mi("MenuItem label", 1);
  assert(mi.getText() == "MenuItem label");

  Menu m("Menu label");
  assert(m.getText() == "Menu label");
}

void test_enabled()
{
  MenuItem mi("a", 1);

  assert(mi.isEnabled());
  mi.setEnabled(false);
  assert(!mi.isEnabled());

  Menu m("b");

  m.add(&mi);
  assert(!mi.isEnabled());
  mi.setEnabled(true);
  assert(mi.isEnabled());
  m.remove(&mi);
}

void test_checked()
{
  MenuItem mi("a", 1);

  assert(!mi.isChecked());
  mi.setChecked(true);
  assert(mi.isChecked());

  Menu m("b");

  m.add(&mi);
  assert(mi.isChecked());
  mi.setChecked(false);
  assert(!mi.isChecked());
  m.remove(&mi);
}

int main()
{
  Application app;
  test_label();
  test_enabled();
  test_checked();
  return 0;
}
