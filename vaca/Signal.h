// Vaca - Visual Application Components Abstraction
// Copyright (c) 2005-2022 David Capello
//
// This file is distributed under the terms of the MIT license,
// please read LICENSE.txt for more information.

#ifndef VACA_SIGNAL_H
#define VACA_SIGNAL_H

#include "vaca/base.h"
#include "vaca/Slot.h"

#include <vector>

namespace vaca {

/**
   @defgroup signal_group Signal Classes
   @{
 */

// Signal for any kind of functions
template<typename Callable>
class Signal { };

/**
   Base class for signals which call functions with return values.
*/
template<typename R, typename...Args>
class Signal<R(Args...)> {
public:
  using SlotType = Slot<R(Args...)>;
  using SlotList = std::vector<SlotType*>;

  Signal() { }
  Signal(const Signal& s) {
    copy(s);
  }

  ~Signal() {
    disconnectAll();
  }

  SlotType* addSlot(SlotType* slot) {
    m_slots.push_back(slot);
    return slot;
  }

  template<typename Function>
  SlotType* connect(Function&& f) {
    return addSlot(new SlotType(std::forward<Function>(f)));
  }

  const SlotList& getSlots() const {
    return m_slots;
  }

  void disconnect(SlotType* slot) {
    remove_from_container(m_slots, slot);
  }

  void disconnectAll() {
    auto end = m_slots.end();
    for (auto it = m_slots.begin(); it != end; ++it)
      delete *it;
    m_slots.clear();
  }

  bool empty() const {
    return m_slots.empty();
  }

  Signal& operator=(const Signal& s) {
    copy(s);
    return *this;
  }

  template<typename...Args2>
  typename std::enable_if<!std::is_void<R>::value, R>::type
  operator()(Args2&&...args) {
    R result = R();
    for (auto slot : m_slots)
      result = (*slot)(std::forward<Args2>(args)...);
    return result;
  }

  template<typename...Args2>
  typename std::enable_if<!std::is_void<R>::value, R>::type
  callWithDefaultReturn(R result, Args2&&...args) {
    for (auto slot : m_slots)
      result = (*slot)(std::forward<Args2>(args)...);
    return result;
  }

private:

  void copy(const Signal& s) {
    auto end = s.m_slots.end();
    for (auto it = s.m_slots.begin(); it != end; ++it) {
      m_slots.push_back((*it)->clone());
    }
  }

  SlotList m_slots;
};

template<typename...Args>
class Signal<void(Args...)> {
public:
  using SlotType = Slot<void(Args...)>;
  using SlotList = std::vector<SlotType*>;

  Signal() { }
  Signal(const Signal& s) {
    copy(s);
  }

  ~Signal() {
    disconnectAll();
  }

  SlotType* addSlot(SlotType* slot) {
    m_slots.push_back(slot);
    return slot;
  }

  template<typename Function>
  SlotType* connect(Function&& f) {
    return addSlot(new SlotType(std::forward<Function>(f)));
  }

  template<class Class>
  SlotType* connect(void (Class::*m)(Args...args), Class* t) {
    return addSlot(new SlotType(
                     [=](Args...args) {
                       (t->*m)(std::forward<Args>(args)...);
                     }));
  }

  const SlotList& getSlots() const {
    return m_slots;
  }

  void disconnect(SlotType* slot) {
    remove_from_container(m_slots, slot);
  }

  void disconnectAll() {
    auto end = m_slots.end();
    for (auto it = m_slots.begin(); it != end; ++it)
      delete *it;
    m_slots.clear();
  }

  bool empty() const {
    return m_slots.empty();
  }

  Signal& operator=(const Signal& s) {
    copy(s);
    return *this;
  }

  template<typename...Args2>
  void operator()(Args2&&...args) {
    for (auto slot : m_slots)
      (*slot)(std::forward<Args2>(args)...);
  }

private:

  void copy(const Signal& s) {
    auto end = s.m_slots.end();
    for (auto it = s.m_slots.begin(); it != end; ++it) {
      m_slots.push_back((*it)->clone());
    }
  }

  SlotList m_slots;
};

/** @} */

} // namespace vaca

#endif // VACA_SIGNAL_H
