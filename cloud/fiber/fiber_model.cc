// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/fiber/fiber_model.h"

#include <gflags/gflags.h>

#include "base/log.h"
#include "base/string.h"

DEFINE_string(mongo_ip, "localhost", "The MongoDB instance ip address.");
DEFINE_string(mongo_port, "27017", "The MongoDB instance port number.");
DEFINE_string(mongo_fiber_table, "fiber.cords",
              "The MongoDB database and table name for fiber entries.");

using ::thilenius::base::String;

namespace thilenius {
namespace cloud {
namespace fiber {

FiberModel::FiberModel() {
  try {
    ::mongo::client::initialize();
    connection_.connect(StrCat(FLAGS_mongo_ip, ":", FLAGS_mongo_port));
  } catch (const mongo::DBException& e) {
    LOG(FATAL) << "MongoDB Driver failed to connect to " << FLAGS_mongo_ip
               << ":" << FLAGS_mongo_port;
  }
}

ValueOf<FiberMapper::FiberEntry> FiberModel::FindCord(
    const std::string& cord_uuid) {
  ::mongo::BSONObj query = BSON("cord.uuid" << cord_uuid);
  auto cursor = connection_.query(FLAGS_mongo_fiber_table, query);
  if (cursor->more()) {
    ::mongo::BSONObj bson = cursor->next();
    FiberMapper::FiberEntry fiber_entry;
    fiber_mapper_.fiber_entry_mapper.from_bson(bson, fiber_entry);
    return {std::move(fiber_entry)};
  }
  return {FiberMapper::FiberEntry(), "Failed to find Cord"};
}

bool FiberModel::SaveEntry(const FiberMapper::FiberEntry& entry) {
  ::mongo::BSONObj bson;
  fiber_mapper_.fiber_entry_mapper.to_bson(entry, bson);
  connection_.remove(FLAGS_mongo_fiber_table,
                     BSON("cord.uuid" << entry.cord.uuid));
  connection_.insert(FLAGS_mongo_fiber_table, bson);
  std::string last_error = connection_.getLastError();
  if (last_error != "") {
    LOG(WARNING) << "Got a MongoDB error: " << last_error;
    return false;
  }
  return true;
}

}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius
