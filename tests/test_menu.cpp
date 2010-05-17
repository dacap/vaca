#include <gtest/gtest.h>

#include "Vaca/Vaca.h"

using namespace Vaca;

class MenuTest : public testing::Test {
  Application app;
};

TEST_F(MenuTest, Label)
{
  MenuItem mi(L"MenuItem label", 1);
  EXPECT_EQ(L"MenuItem label", mi.getText());

  Menu m(L"Menu label");
  EXPECT_EQ(L"Menu label", m.getText());
}

TEST_F(MenuTest, Enabled)
{
  MenuItem mi(L"a", 1);
  EXPECT_TRUE(mi.isEnabled());
  mi.setEnabled(false);
  EXPECT_FALSE(mi.isEnabled());

  Menu m(L"b");

  m.add(&mi);
  EXPECT_FALSE(mi.isEnabled());
  mi.setEnabled(true);
  EXPECT_TRUE(mi.isEnabled());
  m.remove(&mi);
}

TEST_F(MenuTest, Checked)
{
  MenuItem mi(L"a", 1);
  EXPECT_FALSE(mi.isChecked());
  mi.setChecked(true);
  EXPECT_TRUE(mi.isChecked());

  Menu m(L"b");

  m.add(&mi);
  EXPECT_TRUE(mi.isChecked());
  mi.setChecked(false);
  EXPECT_FALSE(mi.isChecked());
  m.remove(&mi);
}
