// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CLOUD_FIBER_FIBER_CLIENT_H_
#define CLOUD_FIBER_FIBER_CLIENT_H_

#include <memory>

#include "base/value_of.hh"
#include "cloud/fiber/Fiber.h"
#include "cloud/fiber/fiber_constants.h"
#include "cloud/fiber/fiber_mapper.h"
#include "cloud/fiber/fiber_types.h"
#include "cloud/sentinel/sentinel_client.h"
#include "cloud/utils/thrift_http_client.hh"

using ::thilenius::base::ValueOf;
using ::thilenius::cloud::fiber::FiberMapper;
using ::thilenius::cloud::sentinel::SentinelClient;
using ::thilenius::cloud::utils::ThriftHttpClient;

namespace thilenius {
namespace cloud {
namespace fiber {

class FiberClient {
 public:
  FiberClient();

  ValueOf<void> Connect(const std::string& fiber_ip, int fiber_port,
                        const std::string& fiber_route,
                        const std::string& sentinel_ip, int sentinel_port,
                        const std::string& sentinel_route);

  // Creates a new Cord using the project token or asks CIN
  ValueOf<::fiber::proto::Cord> CreateCord(const std::string& project_path,
                                           const std::string& name);

  ValueOf<::fiber::proto::Cord> GetCord(const std::string& cord_uuid);

  ValueOf<void> WriteCord(const ::fiber::proto::Cord& cord,
                          const std::vector<::fiber::proto::Grain>& grains);

  ValueOf<void> CloseCord(const ::fiber::proto::Cord& cord);

  ValueOf<std::vector<::fiber::proto::Grain>> ReadCordGTEIndex(
      const ::fiber::proto::Cord& cord, int index);

 private:
  typedef std::shared_ptr<ThriftHttpClient<::fiber::proto::FiberClient>>
      ProtoFiberClientPtr;

  FiberMapper fiber_mapper_;
  ProtoFiberClientPtr http_client_ptr_;
  SentinelClient sentinel_client_;
  bool connected_;
};

}  // namespace fiber
}  // namespace cloud
}  // namespace thilenius

#endif  // CLOUD_FIBER_FIBER_CLIENT_H_
