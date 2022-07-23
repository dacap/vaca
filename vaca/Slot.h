// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SLOT_H
#define VACA_SLOT_H

#include "vaca/base.h"

#include <cassert>
#include <functional>
#include <type_traits>

namespace vaca {

/**
   @defgroup slot_group Slot Classes
   @{
 */

template<typename T>
struct is_callable_without_args : std::is_convertible<T, std::function<void()>> { };

class SlotBase {
public:
  SlotBase() { }
  virtual ~SlotBase() { }

  // Disable copy
  SlotBase(const SlotBase&) = delete;
  SlotBase& operator=(const SlotBase&) = delete;
};

// Generic slot
template<typename Callable>
class Slot { };

template<typename R, typename...Args>
class Slot<R(Args...)> : public SlotBase {
public:
  template<typename F,
           typename = typename std::enable_if<(sizeof...(Args) == 0 ||
                                               !is_callable_without_args<F>::value)>::type>
  Slot(F&& f) : f(std::forward<F>(f)) { }

  template<typename G,
           typename = typename std::enable_if<(sizeof...(Args) != 0 &&
                                               is_callable_without_args<G>::value)>::type>
  Slot(G g) : f([g](Args...) -> R { return g(); }) { }

  Slot(const Slot& s) { (void)s; }

  template<typename...Args2>
  R operator()(Args2&&...args) {
    assert(f);
    return f(std::forward<Args2>(args)...);
  }

private:
  std::function<R(Args...)> f;
};

template<typename...Args>
class Slot<void(Args...)> : public SlotBase {
public:
  template<typename F,
           typename = typename std::enable_if<(sizeof...(Args) == 0 ||
                                               !is_callable_without_args<F>::value)>::type>
  Slot(F&& f) : f(std::forward<F>(f)) { }

  template<typename G,
           typename = typename std::enable_if<(sizeof...(Args) != 0 &&
                                               is_callable_without_args<G>::value)>::type>
  Slot(G g) : f([g](Args...){ g(); }) { }

  Slot(const Slot& s) { (void)s; }

  template<typename...Args2>
  void operator()(Args2&&...args) {
    assert(f);
    f(std::forward<Args2>(args)...);
  }

private:
  std::function<void(Args...)> f;
};

/** @} */

} // namespace vaca

#endif // VACA_SLOT_H
