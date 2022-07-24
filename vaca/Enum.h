// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_ENUM_H
#define VACA_ENUM_H

namespace vaca {

/**
   This class is used to define sets of enumerated values.

   "Base" policy must be like this:
   @code
   struct Base {
     enum { ... };
   };
   @endcode

   Example of how to use this:
   @code
   struct ColorsEnumSet
   {
     enum {
       Red = 1,
       Blue = 2,
       Yellow = 4,
       Magenta = Red | Blue
     };
   };
   typedef EnumSet<ColorsEnumSet> Colors;

   main() {
     Colors red, blue, magenta;
     red = Colors::Red;
     blue = Colors::Blue;
     magenta = red | blue;
     if (magenta == Colors::Magenta) { ... }
   }
   @endcode
*/
template<typename Base>
struct EnumSet : public Base
{
  EnumSet() : m_value(0)
  { }

  EnumSet(int value) : m_value(value)
  { }

  operator int() const
  { return m_value; }

  EnumSet<Base> operator|(int value)
  { return m_value | value; }

  EnumSet<Base> operator&(int value)
  { return m_value & value; }

  EnumSet<Base> operator^(int value)
  { return m_value ^ value; }

  EnumSet<Base> &operator=(int value)
  { m_value = value;
    return *this; }

  EnumSet<Base> &operator|=(int value)
  { m_value |= value;
    return *this; }

  EnumSet<Base> &operator&=(int value)
  { m_value &= value;
    return *this; }

  EnumSet<Base> &operator^=(int value)
  { m_value ^= value;
    return *this; }

private:
  int m_value;
};

} // namespace vaca

#endif // VACA_ENUM_H
