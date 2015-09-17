// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_SENTINEL_SENTINEL_MODEL_H_
#define CLOUD_SENTINEL_SENTINEL_MODEL_H_

#include "base/value_of.hh"
#include "cloud/sentinel/sentinel_mapper.h"
#include "third_party/mongo/client/dbclient.h"

using ::thilenius::base::ValueOf;

namespace thilenius {
namespace cloud {
namespace sentinel {

class SentinelModel {
 public:
  SentinelModel(::mongo::DBClientConnection& connection,
                const std::string user_table, const std::string& token_table)
      : connection_(connection),
        user_table_(user_table),
        token_table_(token_table) {}

  ValueOf<SentinelMapper::SentinelEntry> FindUser(
      const ::sentinel::proto::User& user_partial);

  bool FindToken(const ::sentinel::proto::Token& token);

  bool SaveToken(const ::sentinel::proto::Token& token);

  bool SaveSentinelEntry(const SentinelMapper::SentinelEntry& sentinel_entry);

 private:
  SentinelMapper sentinel_mapper_;
  ::mongo::DBClientConnection& connection_;
  ::sentinel::proto::sentinelConstants constants_;
  const std::string user_table_;
  const std::string token_table_;
};

}  // namespace sentinel
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_SENTINEL_SENTINEL_MODEL_H_
