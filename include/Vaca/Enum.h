// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2010 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_ENUM_H
#define VACA_ENUM_H

namespace Vaca {

/**
   This class is used to define enumerations "a la" C++0x.

   "Base" policy must be like this:
   @code
   struct Base {
     enum enumeration { ... };
     static const enumeration default_value = ...;
   };
   @endcode

   Example of how to use this:
   @code
   struct NumbersEnum
   {
     enum enumeration {
       Zero,
       One,
       Two,
       Three,
     };
     static const enumeration default_value = Zero;
   };
   typedef Enum<NumbersEnum> Numbers;

   main() {
     Numbers n1, n2 = Numbers::One;
     n1 = n2;
     n2 = Numbers::Two;
   }
   @endcode
*/
template<typename Base>
struct Enum : public Base
{
  typedef typename Base::enumeration enumeration;

  Enum() : m_value(Base::default_value)
  { }

  Enum(enumeration value) : m_value(value)
  { }

  operator enumeration() const
  { return m_value; }

  Enum<Base> &operator=(enumeration value)
  { m_value = value;
    return *this; }

private:
  enumeration m_value;
};

/**
   This class is used to define sets of enumerated values.

   "Base" policy must be like this:
   @code
   struct Base {
     enum { ... };
   };
   @endcode

   A EnumSet doesn't need a @c default_value like a Enum because the
   default value is zero which means: a empty set.

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

} // namespace Vaca

#endif // VACA_ENUM_H
