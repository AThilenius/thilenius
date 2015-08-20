// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "base/guid.h"

namespace thilenius {
namespace base {

std::string Guid::NewGuid() {
  return boost::lexical_cast<std::string>(::boost::uuids::random_generator()());
}

}  // namespace base
}  // namespace thilenius
