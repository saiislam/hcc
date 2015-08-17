// XFAIL: Linux
// RUN: %cxxamp -Xclang -fhsa-ext %s -o %t.out && %t.out

#include <amp.h>

#include <iostream>

#define SIZE (16)

using namespace concurrency;

// test supply a class with operator() to parallel_for_each
// the class will call a separate functor with a customized ctor
class user_functor {
public:
  int (&input)[SIZE];

  user_functor(int (&t)[SIZE]) : input(t) {}

  void operator() (index<1>& idx) restrict(amp) {
    input[idx[0]] = idx[0];
  }
};

class prog {
  user_functor& kernel;

public:
  prog(user_functor& f) : kernel(f) {
  }

  void operator() (index<1>& idx) restrict(amp) {
    kernel(idx);
  }

  void run() {
    parallel_for_each(extent<1>(SIZE), *this);
  }

  // verify output
  bool test() {
    bool ret = true;
    for (int i = 0; i < SIZE; ++i) {
      if (kernel.input[i] != i) {
        ret = false;
        break;
      }
    }
    return true;
  }
};

int main() {
  bool ret = true;
 
  // prepare test data
  int input[SIZE] { 0 };

  // launch kernel
  user_functor kernel(input);
  prog p(kernel);
  p.run();

  // check result
  ret &= p.test();

  return !(ret == true);
}

