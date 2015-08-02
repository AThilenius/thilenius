// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_H
#define SCORCH_ANVIL_ANVIL_H

namespace thilenius {
namespace scorch {
namespace anvil {

class AnvilRunner {
 public:
  AnvilRunner(int argc, char** argv);
  int Execute();

 private:
  int argc_;
  char** argv_;
};

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_ANVIL_H
