// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_AUTO_UPDATE_H_
#define BASE_AUTO_UPDATE_H_

#include <string>

namespace thilenius {
namespace base {

class AutoUpdate {
 public:
   static void UpdateAtCurrentExecutable(const std::string& update_json_url);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_AUTO_UPDATE_H_
