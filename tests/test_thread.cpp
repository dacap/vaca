#include <gtest/gtest.h>
#include <vector>

#include "vaca/Thread.h"
#include "vaca/Mutex.h"
#include "vaca/ScopedLock.h"

using namespace std;
using namespace vaca;

namespace test1 {

  Mutex mutex;
  int counter = 0;
  int times = 0;

  void do_it()
  {
    {
      ScopedLock hold(mutex);
      ++counter;
    }
    ::Sleep(100);
  }

  TEST(Thread, Highload)
  {
    const int n = 100;
    vector<Thread*> threads;
    for (int c=0; c<n; c++) threads.push_back(new Thread(&do_it));
    for (int c=0; c<n; c++) threads[c]->join();
    for (int c=0; c<n; c++) delete threads[c];
    assert(counter == n);
  }

}
