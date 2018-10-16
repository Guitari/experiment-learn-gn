#include "hello_lib.h"

// Shamelessly taken from https://github.com/google/googletest/blob/master/googletest/samples/sample1.cc
int Factorial(int n) {
  int result = 1;
  for (int i = 1; i <= n; i++) {
    result *= i;
  }

  return result;
}