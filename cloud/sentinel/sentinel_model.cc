// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/sentinel/sentinel_model.h"

#include <gflags/gflags.h>

#include "base/string.h"

DEFINE_string(mongo_ip, "localhost", "The MongoDB instance ip address.");
DEFINE_string(mongo_port, "27017", "The MongoDB instance port number.");
DEFINE_string(mongo_user_table, "sentinel.users",
              "The MongoDB database and table name for user entries.");
DEFINE_string(mongo_token_table, "sentinel.tokens",
              "The MongoDB database and table name for tokens.");

using ::thilenius::base::String;

namespace thilenius {
namespace cloud {
namespace sentinel {

SentinelModel::SentinelModel() {
  try {
    connection_.connect(StrCat(FLAGS_mongo_ip, ":", FLAGS_mongo_port));
  } catch (const mongo::DBException& e) {
    LOG(FATAL) << "MongoDB Driver failed to connect to " << FLAGS_mongo_ip
               << ":" << FLAGS_mongo_port;
  }
}

ValueOf<SentinelMapper::SentinelEntry> SentinelModel::FindUser(
    const ::sentinel::proto::User& user_partial) {
  ::mongo::BSONObj query;
  if (!String::Blank(user_partial.uuid)) {
    // Search by string
    query = BSON("user.uuid" << user_partial.uuid);
  } else if (!String::Blank(user_partial.email_address)) {
    query = BSON("user.email_address" << user_partial.email_address);
  } else if (!String::Blank(user_partial.first_name) &&
             !String::Blank(user_partial.last_name)) {
    query =
        BSON("user.first_name" << user_partial.first_name << "user.last_name"
                               << user_partial.last_name);
  } else {
    return {SentinelMapper::SentinelEntry(),
            "Partial needs to have a user_uuid or email_address or first & "
            "last names"};
  }
  auto cursor = connection_.query(FLAGS_mongo_user_table, query);
  if (cursor->more()) {
    ::mongo::BSONObj bson = cursor->next();
    SentinelMapper::SentinelEntry entry;
    sentinel_mapper_.entry_mapper.from_bson(bson, entry);
    return {std::move(entry)};
  }
  return {SentinelMapper::SentinelEntry(), "Failed to find user"};
}

bool SentinelModel::FindToken(const ::sentinel::proto::Token& token) {
  ::mongo::BSONObj query =
      BSON("user_uuid" << token.user_uuid << "token_uuid" << token.token_uuid
                       << "permission_level" << token.permission_level);
  auto cursor = connection_.query(FLAGS_mongo_token_table, query);
  if (cursor->more()) {
    return true;
  }
  return false;
}

bool SentinelModel::SaveToken(const ::sentinel::proto::Token& token) {
  if (token.permission_level >= constants_.USER_THRESHOLD) {
    // Primary Token, make sure we don't have duplicates
    connection_.remove(
        FLAGS_mongo_token_table,
        BSON("user_uuid" << token.user_uuid << "permission_level"
                         << ::mongo::GTE << constants_.USER_THRESHOLD));
  }
  ::mongo::BSONObj bson;
  sentinel_mapper_.token_mapper.to_bson(token, bson);
  connection_.update(FLAGS_mongo_token_table, bson, bson, true);
  std::string last_error = connection_.getLastError();
  if (last_error != "") {
    LOG(WARNING) << "Got a MongoDB error: " << last_error;
    return false;
  }
  return true;
}

bool SentinelModel::SaveSentinelEntry(
    const SentinelMapper::SentinelEntry& sentinel_entry) {
  ::mongo::BSONObj bson;
  sentinel_mapper_.entry_mapper.to_bson(sentinel_entry, bson);
  connection_.update(FLAGS_mongo_user_table, BSON("user.uuid" << sentinel_entry.user.uuid),
                     bson, true);
  std::string last_error = connection_.getLastError();
  if (last_error != "") {
    LOG(WARNING) << "Got a MongoDB error: " << last_error;
    return false;
  }
  return true;
}

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius
