// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_ANVIL_H_
#define SCORCH_ANVIL_ANVIL_H_

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "base/macros.h"
#include "scorch/anvil/anvil_interface.h"
#include "scorch/anvil/anvil_types.h"

namespace thilenius {
namespace scorch {
namespace anvil {

// The implementation for AnvilInterface
class Anvil : public AnvilInterface {
 public:
  Anvil(int* argc, char*** argv);
  ~Anvil() = default;

  // Adds a source file that cannot be modified by the user
  virtual void AddFrozenSource(const std::string& relative_path);

  // Adds a source file that can be modified by the user
  virtual void AddUserSource(const std::string& relative_path);

  // Sets the points for a specific test case
  virtual void SetPoints(const std::string& test_name, int denominator,
                         int points_possible);

  // Execute the configured Anvil stack
  virtual void Execute();

 private:
  DISALLOW_COPY_AND_ASSIGN(Anvil);

  ::anvil::AnvilRunReport GenerateAnvilRunReport();
  ::anvil::ProjectState LoadProjectState();
  ::anvil::TestRunReport RunUnitTests();
  std::vector<::crucible::FileInfo> LoadFrozenFiles();
  std::vector<::crucible::SourceFile> LoadSourceFiles();

  int* argc_;
  char*** argv_;
  std::unordered_map<std::string, std::tuple<int, int>> points_;
  std::vector<std::string> user_source_files_;
  std::vector<std::string> frozen_files_;
};  // class Anvil

}  // namespace anvil
}  // namespace scorch
}  // namespace thilenius

#endif  // SCORCH_ANVIL_ANVIL_H_
