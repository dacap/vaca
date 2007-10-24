// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, 2007, David A. Capello
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Vaca nor the names of its contributors
//   may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef VACA_BIND_HPP
#define VACA_BIND_HPP

#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>

namespace Vaca {

//////////////////////////////////////////////////////////////////////

/**
 * @fn Bind 
 * @brief It's like boost::bind, but with automatic boost::arg<1>, boost::arg<2>, etc.
 *
 * You can use directly boost::bind to connect methods in
 * signals. Also, if you are using an overrided method, you should use
 * the original boost::bind to avoid compile errors by ambiguity.
 */

//////////////////////////////////////////////////////////////////////

// Bind(&T::f, a1)
// R T::f()
// a1->f()

template<class R, class T, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf0<R, T>, typename boost::_bi::list_av_1<A1>::type>
Bind(R (T::*f) (), A1 a1)
{
  return boost::bind(f, a1);
}

// Bind(&T::f, a1)
// R T::f() const
// a1->f()

template<class R, class T, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf0<R, T>, typename boost::_bi::list_av_1<A1>::type>
Bind(R (T::*f) () const, A1 a1)
{
  return boost::bind(f, a1);
}

//////////////////////////////////////////////////////////////////////

// Bind(&T::f, a1, a2)
// R T::f(b1)
// a1->f(a2)

template<class R, class T, class B1, class A1, class A2>
boost::_bi::bind_t<R, boost::_mfi::mf1<R, T, B1>, typename boost::_bi::list_av_2<A1, A2>::type>
Bind(R (T::*f) (B1), A1 a1, A2 a2)
{
  return boost::bind(f, a1, a2);
}

// Bind(&T::f, a1, a2)
// R T::f(b1) const
// a1->f(a2)

template<class R, class T, class B1, class A1, class A2>
boost::_bi::bind_t<R, boost::_mfi::cmf1<R, T, B1>, typename boost::_bi::list_av_2<A1, A2>::type>
Bind(R (T::*f) (B1) const, A1 a1, A2 a2)
{
  return boost::bind(f, a1, a2);
}

// Bind(&T::f, a1)
// R T::f(b1)
// a1->f(_1)

template<class R, class T, class B1, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf1<R, T, B1>, typename boost::_bi::list_av_2<A1, boost::arg<1> >::type>
Bind(R (T::*f) (B1), A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>());
}

// Bind(&T::f, a1)
// R T::f(b1) const
// a1->f(_1)

template<class R, class T, class B1, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf1<R, T, B1>, typename boost::_bi::list_av_2<A1, boost::arg<1> >::type>
Bind(R (T::*f) (B1) const, A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>());
}

//////////////////////////////////////////////////////////////////////

// Bind(&T::f, a1, a2, a3)
// R T::f(b1, b2)
// a1->f(a2, a3)

template<class R, class T, class B1, class B2, class A1, class A2, class A3>
boost::_bi::bind_t<R, boost::_mfi::mf2<R, T, B1, B2>, typename boost::_bi::list_av_3<A1, A2, A3>::type>
Bind(R (T::*f) (B1, B2), A1 a1, A2 a2, A3 a3)
{
  return boost::bind(f, a1, a2, a3);
}

// Bind(&T::f, a1, a2, a3)
// R T::f(b1, b2) const
// a1->f(a2, a3)

template<class R, class T, class B1, class B2, class A1, class A2, class A3>
boost::_bi::bind_t<R, boost::_mfi::cmf2<R, T, B1, B2>, typename boost::_bi::list_av_3<A1, A2, A3>::type>
Bind(R (T::*f) (B1, B2) const, A1 a1, A2 a2, A3 a3)
{
  return boost::bind(f, a1, a2, a3);
}

// Bind(&T::f, a1)
// R T::f(b1, b2)
// a1->f(_1, _2)

template<class R, class T, class B1, class B2, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf2<R, T, B1, B2>, typename boost::_bi::list_av_3<A1, boost::arg<1>, boost::arg<2> >::type>
Bind(R (T::*f) (B1, B2), A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>());
}

// Bind(&T::f, a1)
// R T::f(b1, b2) const
// a1->f(_1, _2)

template<class R, class T, class B1, class B2, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf2<R, T, B1, B2>, typename boost::_bi::list_av_3<A1, boost::arg<1>, boost::arg<2> >::type>
Bind(R (T::*f) (B1, B2) const, A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>());
}

//////////////////////////////////////////////////////////////////////

// Bind(&T::f, a1, a2, a3, a4)
// R T::f(b1, b2, b3)
// a1->f(a2, a3, a4)

template<class R, class T, class B1, class B2, class B3, class A1, class A2, class A3, class A4>
boost::_bi::bind_t<R, boost::_mfi::mf3<R, T, B1, B2, B3>, typename boost::_bi::list_av_4<A1, A2, A3, A4>::type>
Bind(R (T::*f) (B1, B2, B3), A1 a1, A2 a2, A3 a3, A4 a4)
{
  return boost::bind(f, a1, a2, a3, a4);
}

// Bind(&T::f, a1, a2, a3, a4)
// R T::f(b1, b2, b3) const
// a1->f(a2, a3, a4)

template<class R, class T, class B1, class B2, class B3, class A1, class A2, class A3, class A4>
boost::_bi::bind_t<R, boost::_mfi::cmf3<R, T, B1, B2, B3>, typename boost::_bi::list_av_4<A1, A2, A3, A4>::type>
Bind(R (T::*f) (B1, B2, B3) const, A1 a1, A2 a2, A3 a3, A4 a4)
{
  return boost::bind(f, a1, a2, a3, a4);
}

// Bind(&T::f, a1)
// R T::f(b1, b2, b3)
// a1->f(_1, _2, _3)

template<class R, class T, class B1, class B2, class B3, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf3<R, T, B1, B2, B3>, typename boost::_bi::list_av_4<A1, boost::arg<1>, boost::arg<2>, boost::arg<3> >::type>
Bind(R (T::*f) (B1, B2, B3), A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>(), boost::arg<3>());
}

// Bind(&T::f, a1)
// R T::f(b1, b2, b3) const
// a1->f(_1, _2, _3)

template<class R, class T, class B1, class B2, class B3, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf3<R, T, B1, B2, B3>, typename boost::_bi::list_av_4<A1, boost::arg<1>, boost::arg<2>, boost::arg<3> >::type>
Bind(R (T::*f) (B1, B2, B3) const, A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>(), boost::arg<3>());
}

//////////////////////////////////////////////////////////////////////

// Bind(&T::f, a1, a2, a3, a4, a5)
// R T::f(b1, b2, b3, b4)
// a1->f(a2, a3, a4, a5)

template<class R, class T, class B1, class B2, class B3, class B4, class A1, class A2, class A3, class A4, class A5>
boost::_bi::bind_t<R, boost::_mfi::mf4<R, T, B1, B2, B3, B4>, typename boost::_bi::list_av_5<A1, A2, A3, A4, A5>::type>
Bind(R (T::*f) (B1, B2, B3, B4), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
  return boost::bind(f, a1, a2, a3, a4, a5);
}

// Bind(&T::f, a1, a2, a3, a4, a5)
// R T::f(b1, b2, b3, b4) const
// a1->f(a2, a3, a4, a5)

template<class R, class T, class B1, class B2, class B3, class B4, class A1, class A2, class A3, class A4, class A5>
boost::_bi::bind_t<R, boost::_mfi::cmf4<R, T, B1, B2, B3, B4>, typename boost::_bi::list_av_5<A1, A2, A3, A4, A5>::type>
Bind(R (T::*f) (B1, B2, B3, B4) const, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
  return boost::bind(f, a1, a2, a3, a4, a5);
}

// Bind(&T::f, a1)
// R T::f(b1, b2, b3, b4)
// a1->f(_1, _2, _3, _4)

template<class R, class T, class B1, class B2, class B3, class B4, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf4<R, T, B1, B2, B3, B4>, typename boost::_bi::list_av_5<A1, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4> >::type>
Bind(R (T::*f) (B1, B2, B3, B4), A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>(), boost::arg<3>(), boost::arg<4>());
}

// Bind(&T::f, a1)
// R T::f(b1, b2, b3, b4) const
// a1->f(_1, _2, _3, _4)

template<class R, class T, class B1, class B2, class B3, class B4, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf4<R, T, B1, B2, B3, B4>, typename boost::_bi::list_av_5<A1, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4> >::type>
Bind(R (T::*f) (B1, B2, B3, B4) const, A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>(), boost::arg<3>(), boost::arg<4>());
}

//////////////////////////////////////////////////////////////////////

// Bind(&T::f, a1, a2, a3, a4, a5, a6)
// R T::f(b1, b2, b3, b4, b5)
// a1->f(a2, a3, a4, a5, a6)

template<class R, class T, class B1, class B2, class B3, class B4, class B5, class A1, class A2, class A3, class A4, class A5, class A6>
boost::_bi::bind_t<R, boost::_mfi::mf5<R, T, B1, B2, B3, B4, B5>, typename boost::_bi::list_av_6<A1, A2, A3, A4, A5, A6>::type>
Bind(R (T::*f) (B1, B2, B3, B4, B5), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  return boost::bind(f, a1, a2, a3, a4, a5, a6);
}

// Bind(&T::f, a1, a2, a3, a4, a5, a6)
// R T::f(b1, b2, b3, b4, b5) const
// a1->f(a2, a3, a4, a5, a6)

template<class R, class T, class B1, class B2, class B3, class B4, class B5, class A1, class A2, class A3, class A4, class A5, class A6>
boost::_bi::bind_t<R, boost::_mfi::cmf5<R, T, B1, B2, B3, B4, B5>, typename boost::_bi::list_av_6<A1, A2, A3, A4, A5, A6>::type>
Bind(R (T::*f) (B1, B2, B3, B4, B5) const, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  return boost::bind(f, a1, a2, a3, a4, a5, a6);
}

// Bind(&T::f, a1)
// R T::f(b1, b2, b3, b4, b5)
// a1->f(_1, _2, _3, _4, _5)

template<class R, class T, class B1, class B2, class B3, class B4, class B5, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf5<R, T, B1, B2, B3, B4, B5>, typename boost::_bi::list_av_6<A1, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4>, boost::arg<5> >::type>
Bind(R (T::*f) (B1, B2, B3, B4, B5), A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>(), boost::arg<3>(), boost::arg<4>(), boost::arg<5>());
}

// Bind(&T::f, a1)
// R T::f(b1, b2, b3, b4, b5) const
// a1->f(_1, _2, _3, _4, _5)

template<class R, class T, class B1, class B2, class B3, class B4, class B5, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf5<R, T, B1, B2, B3, B4, B5>, typename boost::_bi::list_av_6<A1, boost::arg<1>, boost::arg<2>, boost::arg<3>, boost::arg<4>, boost::arg<5> >::type>
Bind(R (T::*f) (B1, B2, B3, B4, B5) const, A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>(), boost::arg<2>(), boost::arg<3>(), boost::arg<4>(), boost::arg<5>());
}

//////////////////////////////////////////////////////////////////////

} // namespace Vaca

#endif
