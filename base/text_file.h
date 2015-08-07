// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_TEST_FILE_H_
#define BASE_TEST_FILE_H_

#include <string>

namespace thilenius {
namespace base {

class TextFile {
 public:
  static TextFile LoadFromFileOrFail(const std::string& path);

 private:
  std::string file_contents_;
};  // class AnvilSnapshot

}  // namespace base
}  // namespace thilenius

#endif  // BASE_TEST_FILE_H_
