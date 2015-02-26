#include <gtest/gtest.h>

#include "vaca/vaca.h"

using namespace vaca;

TEST(Tab, Base)
{
  Application app;
  Frame frame(L"Temporary");
  TabBase tab(&frame);

  // default side
  assert(tab.getSide() == Side::Top);

  // default multiline
  assert(tab.isMultiline() == false);
  tab.setMultiline(true);
  assert(tab.isMultiline() == true);

  // change side
  tab.setSide(Side::Left);
  assert(tab.getSide() == Side::Left);

  // add/insert/remove pages
  tab.addPage(L"Remove me");
  tab.addPage(L"A");
  tab.addPage(L"C");
  tab.addPage(L"D");
  tab.insertPage(2, L"B");
  tab.removePage(0);
  tab.removePage(3);

  assert(tab.getPageCount() == 3);
  assert(tab.getPageText(0) == L"A");
  assert(tab.getPageText(1) == L"B");
  assert(tab.getPageText(2) == L"C");
}
