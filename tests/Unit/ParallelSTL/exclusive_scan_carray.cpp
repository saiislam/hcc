// XFAIL: Linux
// RUN: %cxxamp -Xclang -fhsa-ext %s -o %t.out && %t.out

// Parallel STL headers
#include <coordinate>
#include <experimental/algorithm>
#include <experimental/numeric>
#include <experimental/execution_policy>

#define _DEBUG (0)
#include "test_base.h"


template<typename T, size_t SIZE>
bool test(void) {

  auto binary_op = std::plus<T>();
  auto init = T{};

  using namespace std::experimental::parallel;

  bool ret = true;
  ret &= run<T, SIZE>([init, binary_op]
                      (T (&input1)[SIZE], T (&output1)[SIZE],
                       T (&input2)[SIZE], T (&output2)[SIZE]) {
    std::partial_sum(std::begin(input1), std::end(input1), std::begin(output1), binary_op);
    for (int i = SIZE-2; i >= 0; i--)
      output1[i+1] = binary_op(init, output1[i]);
    output1[0] = init;

    exclusive_scan(par, std::begin(input2), std::end(input2), std::begin(output2), init, binary_op);
  });

  return ret;
}

int main() {
  bool ret = true;

  ret &= test<int, TEST_SIZE>();
  ret &= test<unsigned, TEST_SIZE>();
  ret &= test<float, TEST_SIZE>();
  ret &= test<double, TEST_SIZE>();

  return !(ret == true);
}

