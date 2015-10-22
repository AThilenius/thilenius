// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_FIBER_FIBER_MODEL_H_
#define CLOUD_FIBER_FIBER_MODEL_H_

#include "base/value_of.hh"
#include "cloud/fiber/fiber_mapper.h"
#include "third_party/mongo/client/dbclient.h"

using ::thilenius::base::ValueOf;

namespace thilenius {
namespace cloud {
namespace fiber {

class FiberModel {
 public:
  FiberModel();

  ValueOf<FiberMapper::FiberEntry> FindCord(const std::string& cord_uuid);

  bool SaveEntry(const FiberMapper::FiberEntry& entry);

 private:
  FiberMapper fiber_mapper_;
  ::mongo::DBClientConnection connection_;
};

}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_FIBER_FIBER_MODEL_H_
