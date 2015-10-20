// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/blaze/blaze_model.h"

#include <gflags/gflags.h>

#include "base/json.h"
#include "base/log.h"
#include "base/string.h"
#include "base/types.h"

DEFINE_string(mongo_ip, "localhost", "The MongoDB instance ip address.");
DEFINE_string(mongo_port, "27017", "The MongoDB instance port number.");
DEFINE_string(mongo_minecraft_accounts_table, "blaze.minecraft_accounts",
              "The MongoDB database and table name to use.");

namespace thilenius {
namespace scorch {
namespace cloud {
namespace blaze {

BlazeModel::BlazeModel() {
  try {
    connection_.connect(StrCat(FLAGS_mongo_ip, ":", FLAGS_mongo_port));
  } catch (const mongo::DBException& e) {
    LOG(FATAL) << "MongoDB Driver failed to connect to " << FLAGS_mongo_ip
               << ":" << FLAGS_mongo_port;
  }
}

ValueOf<::blaze::proto::MinecraftAccount>
BlazeModel::FindMinecraftAccountByUserUuid(const std::string& user_uuid) {
  ::mongo::BSONObj query = BSON("user_uuid" << user_uuid);
  auto cursor = connection_.query(FLAGS_mongo_minecraft_accounts_table, query);
  if (cursor->more()) {
    // Got a account back
    ::mongo::BSONObj bson = cursor->next();
    ::blaze::proto::MinecraftAccount minecraft_account;
    blaze_mapper_.minecraft_account_mapper.from_bson(bson, minecraft_account);
    return std::move(minecraft_account);
  } else {
    return {::blaze::proto::MinecraftAccount(), "Failed to find account"};
  }
}

ValueOf<::blaze::proto::MinecraftAccount>
BlazeModel::FindMinecraftAccountByAccountUsername(
    const std::string& minecraft_username) {
  ::mongo::BSONObj query = BSON("minecraft_username" << minecraft_username);
  auto cursor = connection_.query(FLAGS_mongo_minecraft_accounts_table, query);
  if (cursor->more()) {
    // Got a account back
    ::mongo::BSONObj bson = cursor->next();
    ::blaze::proto::MinecraftAccount minecraft_account;
    blaze_mapper_.minecraft_account_mapper.from_bson(bson, minecraft_account);
    return std::move(minecraft_account);
  } else {
    return {::blaze::proto::MinecraftAccount(), "Failed to find account"};
  }
}

bool BlazeModel::SaveMinecraftAccount(
    const ::blaze::proto::MinecraftAccount& account) {
  connection_.update(FLAGS_mongo_minecraft_accounts_table,
                     BSON("user_uuid" << account.user_uuid),
                     blaze_mapper_.minecraft_account_mapper.to_bson(account),
                     true);
  std::string last_error = connection_.getLastError();
  if (last_error != "") {
    LOG(WARNING) << "Got a MongoDB error: " << last_error;
    return false;
  }
  return true;
}

}  // namespace blaze
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
