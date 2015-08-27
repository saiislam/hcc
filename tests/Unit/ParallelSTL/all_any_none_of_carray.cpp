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
  auto pred1 = [](const T& v) { return static_cast<int>(v) % 3 == 0; };
  auto pred2 = [](const T& v) { return static_cast<int>(v) == SIZE + 1; };
  auto pred3 = [](const T& v) { return v >= 0; };

  using namespace std::experimental::parallel;

  bool ret = true;
  bool eq = true;
  // any_of
  ret &= run<T, SIZE>([pred1, &eq](T (&input1)[SIZE],
                                   T (&input2)[SIZE]) {
    auto expected = std::any_of(std::begin(input1), std::end(input1), pred1);
    auto result = std::experimental::parallel::any_of(std::begin(input2), std::end(input2), pred1);
    eq = EQ(expected, result);
  }, false);
  ret &= eq;


  // none_of
  ret &= run<T, SIZE>([pred2, &eq](T (&input1)[SIZE],
                                   T (&input2)[SIZE]) {
    auto expected = std::none_of(std::begin(input1), std::end(input1), pred2);
    auto result = std::experimental::parallel::none_of(std::begin(input2), std::end(input2), pred2);
    eq = EQ(expected, result);
  }, false);
  ret &= eq;


  // all_of
  ret &= run<T, SIZE>([pred3, &eq](T (&input1)[SIZE],
                                   T (&input2)[SIZE]) {
    auto expected = std::all_of(std::begin(input1), std::end(input1), pred3);
    auto result = std::experimental::parallel::all_of(std::begin(input2), std::end(input2), pred3);
    eq = EQ(expected, result);
  }, false);
  ret &= eq;

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

