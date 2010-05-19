#include "Vaca/Pen.h"
#include "Vaca/Color.h"
#include <gtest/gtest.h>

using namespace Vaca;

inline std::ostream& operator<<(std::ostream& os, const Color& color)
{
  return os << "Color(" 
	    << color.getR() << ", "
	    << color.getG() << ", "
	    << color.getB() << ")\n";
}

inline std::ostream& operator<<(std::ostream& os, PenStyle penStyle)
{
  return os << "PenStyle(" << (int)penStyle << ")\n";
}

inline std::ostream& operator<<(std::ostream& os, PenEndCap penEndCap)
{
  return os << "PenEndCap(" << (int)penEndCap << ")\n";
}

inline std::ostream& operator<<(std::ostream& os, PenJoin penJoin)
{
  return os << "PenJoin(" << (int)penJoin << ")\n";
}

TEST(Pen, DefaultContructor)
{
  Pen pen;
  EXPECT_EQ(Color::Black, pen.getColor());
  EXPECT_EQ(1, pen.getWidth());
  EXPECT_EQ(PenStyle::Solid, pen.getStyle());
  EXPECT_EQ(PenEndCap::Round, pen.getEndCap());
  EXPECT_EQ(PenJoin::Round, pen.getJoin());
}

TEST(Pen, Constructors)
{
  Pen p0(Color::White,	 0);
  Pen p1(Color::Red,	 1);
  Pen p2(Color::Green,	 500);
  Pen p3(Color::Blue,	 1000);

  EXPECT_EQ(Color::White,	 p0.getColor());
  EXPECT_EQ(Color::Red,		 p1.getColor());
  EXPECT_EQ(Color::Green,	 p2.getColor());
  EXPECT_EQ(Color::Blue,	 p3.getColor());

  EXPECT_EQ(0,		 p0.getWidth());
  EXPECT_EQ(1,		 p1.getWidth());
  EXPECT_EQ(500,	 p2.getWidth());
  EXPECT_EQ(1000,	 p3.getWidth());

  EXPECT_EQ(PenStyle::Solid,  p0.getStyle());
  EXPECT_EQ(PenEndCap::Round, p0.getEndCap());
  EXPECT_EQ(PenJoin::Round,   p0.getJoin());
  EXPECT_EQ(PenStyle::Solid,  p3.getStyle());
  EXPECT_EQ(PenEndCap::Round, p3.getEndCap());
  EXPECT_EQ(PenJoin::Round,   p3.getJoin());

  Pen p2_copy(p2);
  EXPECT_TRUE(p2.getColor()  == p2_copy.getColor());
  EXPECT_TRUE(p2.getWidth()  == p2_copy.getWidth());
  EXPECT_TRUE(p2.getStyle()  == p2_copy.getStyle());
  EXPECT_TRUE(p2.getEndCap() == p2_copy.getEndCap());
  EXPECT_TRUE(p2.getJoin()   == p2_copy.getJoin());
}
