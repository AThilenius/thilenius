// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_FIBER_FIBER_HANDLER_H_
#define CLOUD_FIBER_FIBER_HANDLER_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "base/blocking_stream.hh"
#include "base/mutex.h"
#include "cloud/fiber/Fiber.h"
#include "cloud/fiber/fiber_types.h"

using ::thilenius::base::BlockingStream;

namespace thilenius {
namespace cloud {
namespace fiber {
namespace server {

class FiberHandler : virtual public ::fiber::proto::FiberIf {
 public:
  void CreateCord(::fiber::proto::Cord& _return,
                  const ::sentinel::proto::Token& token,
                  const std::string& name);

  void GetCord(::fiber::proto::Cord& _return, const std::string& cord_uuid);

  void WriteCord(const ::fiber::proto::Cord& cord,
                 const std::vector<::fiber::proto::Grain>& grains);

  void CloseCord(const ::fiber::proto::Cord& cord);

  void ReadCordGTEIndex(std::vector<::fiber::proto::Grain>& _return,
                        const ::fiber::proto::Cord& cord, const int32_t index);

 private:
  // Active cords are kept in-memory, keyed by UUID. Closed cords are written to
  // MongoDB
  struct ActiveCord {
    ActiveCord() : grains(new BlockingStream<::fiber::proto::Grain>()) {}

    ::fiber::proto::Cord cord;
    // Needs to be a shared ptr so it don't be destoryed when the stream is
    // closed. Everything else can be flushed.
    std::shared_ptr<BlockingStream<::fiber::proto::Grain>> grains;
  };
  std::mutex mutex_;
  std::unordered_map<std::string, ActiveCord> active_cords_ GUARDED_BY(mutex_);
};

}  // namespace server
}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_FIBER_FIBER_HANDLER_H_
