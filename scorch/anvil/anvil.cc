#include <iostream>
#include "base/gflags.h"
#include "base/string.h"

DEFINE_string(test, "Defaul value", "A test flag!");

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::cout << "Hello, world" << std::endl;
  std::cout << "Got flag: " << FLAGS_test << std::endl;
}
