// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_CRYPTO_H_
#define BASE_CRYPTO_H_

#include <string>

#include "base/types.h"
#include "base/value_of.hh"

namespace thilenius {
namespace base {

class Crypto {
 public:
  static ValueOf<std::string> GenerateSaltBase64();
  static ValueOf<std::string> HashPassword(const std::string& salt_base_64,
                                           const std::string& password);
  static ValueOf<std::string> ToBase64(const std::string& binary_data);
  static ValueOf<std::string> FromBase64(const std::string& base_64_data);
};

}  // namespace base
}  // namespace thilenius

#endif  // BASE_CRYPTO_H_
