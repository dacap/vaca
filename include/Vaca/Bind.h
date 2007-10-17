// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005, 2006, David A. Capello
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

#ifndef VACA_BIND_H
#define VACA_BIND_H

#include <boost/bind.hpp>

namespace Vaca {

/**
 * @fn Bind 
 * @brief It's like boost::bind, but without the boost::arg<1>, boost::arg<2>, etc.
 *
 * You can use directly boost::bind in connect method for signals.
 */

// to use "Bind(&ClassName::methodName, thisInstance)"
// when methodName signature is "void ClassName::methodName()"

template<class R, class T, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf0<R, T>, typename boost::_bi::list_av_1<A1>::type>
Bind(R (T::*f) (), A1 a1)
{
  return boost::bind(f, a1);
}

template<class R, class T, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf0<R, T>, typename boost::_bi::list_av_1<A1>::type>
Bind(R (T::*f) () const, A1 a1)
{
  return boost::bind(f, a1);
}

// to use "Bind(&ClassName::methodName, thisInstance)"
// when methodName signature is "void ClassName::methodName(boost::arg<1>())"

template<class R, class T, class B1, class A1>
boost::_bi::bind_t<R, boost::_mfi::mf1<R, T, B1>, typename boost::_bi::list_av_2<A1, boost::arg<1> >::type>
Bind(R (T::*f) (B1), A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>());
}

template<class R, class T, class B1, class A1>
boost::_bi::bind_t<R, boost::_mfi::cmf1<R, T, B1>, typename boost::_bi::list_av_2<A1, boost::arg<1> >::type>
Bind(R (T::*f) (B1) const, A1 a1)
{
  return boost::bind(f, a1, boost::arg<1>());
}

// to use "Bind(&ClassName::methodName, thisInstance, firstArgument)"
// when methodName signature is "void ClassName::methodName(firstArgument)"

template<class R, class T, class B1, class A1, class A2>
boost::_bi::bind_t<R, boost::_mfi::mf1<R, T, B1>, typename boost::_bi::list_av_2<A1, A2>::type>
Bind(R (T::*f) (B1), A1 a1, A2 a2)
{
  return boost::bind(f, a1, a2);
}

template<class R, class T, class B1, class A1, class A2>
boost::_bi::bind_t<R, boost::_mfi::cmf1<R, T, B1>, typename boost::_bi::list_av_2<A1, A2>::type>
Bind(R (T::*f) (B1) const, A1 a1, A2 a2)
{
  return boost::bind(f, a1, a2);
}

// to use "Bind(&ClassName::methodName, thisInstance, firstArgument, secondArgument)"
// when methodName signature is "void ClassName::methodName(firstArgument, secondArgument)"

template<class R, class T, class B1, class B2, class A1, class A2, class A3>
boost::_bi::bind_t<R, boost::_mfi::mf2<R, T, B1, B2>, typename boost::_bi::list_av_3<A1, A2, A3>::type>
Bind(R (T::*f) (B1, B2), A1 a1, A2 a2, A3 a3)
{
  return boost::bind(f, a1, a2, a3);
}

template<class R, class T, class B1, class B2, class A1, class A2, class A3>
boost::_bi::bind_t<R, boost::_mfi::cmf2<R, T, B1, B2>, typename boost::_bi::list_av_3<A1, A2, A3>::type>
Bind(R (T::*f) (B1, B2) const, A1 a1, A2 a2, A3 a3)
{
  return boost::bind(f, a1, a2, a3);
}

// to use "Bind(&ClassName::methodName, thisInstance, firstArg, secondArg, thirdArg)"
// when methodName signature is "void ClassName::methodName(firstArg, secondArg, thirdArg)"

template<class R, class T, class B1, class B2, class B3, class A1, class A2, class A3, class A4>
boost::_bi::bind_t<R, boost::_mfi::mf3<R, T, B1, B2, B3>, typename boost::_bi::list_av_4<A1, A2, A3, A4>::type>
Bind(R (T::*f) (B1, B2, B3), A1 a1, A2 a2, A3 a3, A4 a4)
{
  return boost::bind(f, a1, a2, a3, a4);
}

template<class R, class T, class B1, class B2, class B3, class A1, class A2, class A3, class A4>
boost::_bi::bind_t<R, boost::_mfi::cmf3<R, T, B1, B2, B3>, typename boost::_bi::list_av_4<A1, A2, A3, A4>::type>
Bind(R (T::*f) (B1, B2, B3) const, A1 a1, A2 a2, A3 a3, A4 a4)
{
  return boost::bind(f, a1, a2, a3, a4);
}

} // namespace Vaca

#endif
