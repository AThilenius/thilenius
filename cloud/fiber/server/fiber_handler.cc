// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "cloud/fiber/server/fiber_handler.h"

#include <gflags/gflags.h>

#include "base/guid.h"
#include "base/string.h"
#include "base/time.h"
#include "cloud/fiber/fiber_constants.h"
#include "cloud/fiber/fiber_model.h"
#include "cloud/sentinel/sentinel_client.h"

DEFINE_string(sentinel_ip, "localhost", "The Sentinel instance ip address.");
DEFINE_int32(sentinel_port, 2100, "The Sentinel instance port number.");
DEFINE_string(sentinel_route, "/", "The Sentinel instance route.");

using ::thilenius::base::Guid;
using ::thilenius::base::String;
using ::thilenius::base::Time;
using ::thilenius::cloud::sentinel::SentinelClient;

namespace thilenius {
namespace cloud {
namespace fiber {
namespace server {
namespace {

void ThrowOpFailure(const std::string& user_message) {
  ::fiber::proto::OperationFailure op_failure;
  op_failure.user_message = user_message;
  throw op_failure;
}

void AuthenticateOrThrow(const ::sentinel::proto::Token& token) {
  SentinelClient sentinel_client;
  sentinel_client.Connect(FLAGS_sentinel_ip, FLAGS_sentinel_port,
                          FLAGS_sentinel_route);
  // Authenticate
  if (!sentinel_client.ValidateToken(token).IsValid()) {
    ThrowOpFailure("Sentinel failure, invalid token");
  }
}

}  // namespace

void FiberHandler::CreateCord(::fiber::proto::Cord& _return,
                              const ::sentinel::proto::Token& token,
                              const std::string& name) {
  AuthenticateOrThrow(token);
  ::fiber::proto::Cord cord;
  cord.uuid = Guid::NewGuid();
  cord.name = name;
  cord.user_uuid = token.user_uuid;
  cord.timestamp = std::to_string(Time::EpochMilliseconds());
  ActiveCord active_cord;
  active_cord.cord = cord;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    active_cords_.insert({cord.uuid, std::move(active_cord)});
  }
  _return = std::move(cord);
}

void FiberHandler::GetCord(::fiber::proto::Cord& _return,
                           const std::string& cord_uuid) {
  // Try finding it in cache first
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = active_cords_.find(cord_uuid);
    if (iter != active_cords_.end()) {
      _return = iter->second.cord;
      return;
    }
  }
  // Look it up in MongoDB (Scope this so we only connect mongo with needed)
  {
    FiberModel fiber_model;
    ValueOf<FiberMapper::FiberEntry> entry_value =
        fiber_model.FindCord(cord_uuid);
    if (!entry_value.IsValid()) {
      ThrowOpFailure("Failed to find Cord");
    }
    _return = std::move(entry_value.GetOrDie().cord);
    return;
  }
}

void FiberHandler::WriteCord(const ::fiber::proto::Cord& cord,
                             const std::vector<::fiber::proto::Grain>& grains) {
  // Try finding it in cache
  std::shared_ptr<BlockingStream<::fiber::proto::Grain>> grains_stream(nullptr);
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = active_cords_.find(cord.uuid);
    if (iter != active_cords_.end()) {
      grains_stream = iter->second.grains;
    }
  }
  if (grains_stream) {
      grains_stream->Write(grains);
  } else {
    // We can assume it's closed and not bother checking MongoDB
    throw ::fiber::proto::EndOfCord();
  }
}

void FiberHandler::CloseCord(const ::fiber::proto::Cord& cord) {
  // Try finding it in cache (Must be done in a single lock)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = active_cords_.find(cord.uuid);
    if (iter == active_cords_.end()) {
      // Assume it's already been closed
      throw ::fiber::proto::EndOfCord();
    } else {
      FiberMapper::FiberEntry fiber_entry;
      fiber_entry.cord = iter->second.cord;
      iter->second.grains->WriteEndOfStream();
      auto read_all_value = iter->second.grains->ReadItemsAfterIndex(0);
      if (read_all_value.IsValid()) {
        // At least one item was written to the stream
        fiber_entry.grains = std::move(read_all_value.GetOrDie());
      }
      active_cords_.erase(iter);
      // Sadly, we need to keep the lock till the write to mongo finishes (see
      // ReadCordGTEIndex for why)
      FiberModel fiber_model;
      if (!fiber_model.SaveEntry(fiber_entry)) {
        ThrowOpFailure("Failed to archive Cord");
      }
    }
  }
}

void FiberHandler::ReadCordGTEIndex(std::vector<::fiber::proto::Grain>& _return,
                                    const ::fiber::proto::Cord& cord,
                                    const int32_t index) {
  // Try finding the stream in cache
  std::shared_ptr<BlockingStream<::fiber::proto::Grain>> grains_stream(nullptr);
  {
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = active_cords_.find(cord.uuid);
    if (iter != active_cords_.end()) {
      grains_stream = iter->second.grains;
    }
  }
  // Need to release the lock before waiting the stream
  if (grains_stream != nullptr) {
    auto read_value = grains_stream->ReadItemsAfterIndex(index);
    if (!read_value.IsValid()) {
      throw ::fiber::proto::EndOfCord();
    }
    _return = std::move(read_value.GetOrDie());
  } else {
    // It's not in cache, look it up in MongoDB
    FiberModel fiber_model;
    ValueOf<FiberMapper::FiberEntry> entry_value =
        fiber_model.FindCord(cord.uuid);
    if (!entry_value.IsValid()) {
      ThrowOpFailure("Read: Failed to find Cord");
    }
    FiberMapper::FiberEntry entry = entry_value.GetOrDie();
    if (index >= entry.grains.size()) {
      throw ::fiber::proto::EndOfCord();
    }
    std::vector<::fiber::proto::Grain> filtered_grains;
    for (int i = index; i < entry.grains.size(); i++) {
      filtered_grains.emplace_back(std::move(entry.grains[i]));
    }
    _return = std::move(filtered_grains);
  }
}

}  // namespace server
}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius
