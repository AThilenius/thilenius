// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_INTERFACE_H_
#define SCORCH_ANVIL_ANVIL_INTERFACE_H_

#include <string>

namespace thilenius {
namespace scorch {
namespace anvil {

// Abstract interface for Anvil
class AnvilInterface {
 public:
  virtual ~AnvilInterface() = default;

  // Adds a source file that cannot be modified by the user
  virtual void AddFrozenSource(const std::string& relative_path) = 0;

  // Adds a source file that can be modified by the user
  virtual void AddUserSource(const std::string& relative_path) = 0;

  // Sets the points for a specific test case
  virtual void SetPoints(const std::string& test_name, int denominator,
                         int points_possible) = 0;

  // Execute the configured Anvil stack
  virtual void Execute() = 0;
};  // class AnvilInterface

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_ANVIL_INTERFACE_H_
