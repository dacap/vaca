#include <gtest/gtest.h>
#include <vector>

#include "Vaca/Thread.h"
#include "Vaca/Mutex.h"
#include "Vaca/ScopedLock.h"

using namespace std;
using namespace Vaca;

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
