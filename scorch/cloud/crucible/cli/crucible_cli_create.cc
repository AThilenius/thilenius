// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/gflags/gflags.h"
#include "base/log.h"
#include "base/string.h"
#include "cloud/utils/thrift_http_client.hh"
#include "scorch/cloud/crucible/cli/Crucible.h"
#include "third_party/thrift/protocol/TJSONProtocol.h"
#include "third_party/thrift/TSerializer.h"
#include "third_party/thrift/transport/TSocket.h"
#include "third_party/thrift/transport/TTransportUtils.h"

DEFINE_string(endpoint, "localhost", "The crucible remote endpoint");

using ::thilenius::cloud::utils::ThriftHttpClient;

int main(int argc, char **argv) {
  ::gflags::SetUsageMessage("crucible create [flags] <username> <repo_name>");
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (argc < 3) {
    LOG(FATAL) << "Expected 2 arguments, got " << (argc - 1);
  }
  std::string user_name = argv[argc - 2];
  std::string repo_name = argv[argc - 1];
  if (Blank(user_name) || Blank(repo_name)) {
    LOG(FATAL) << "You must specify both a username and repo name";
  }

  LOG(INFO) << "Connecting to crucible at: " << FLAGS_endpoint;
  ThriftHttpClient<::crucible::CrucibleClient> http_client(FLAGS_endpoint, 80,
                                                           "/");
  auto client = http_client.ConnectOrDie();

  LOG(INFO) << "Creating crucible repo " << user_name << "/" << repo_name;
  ::crucible::Repo repo;
  client->CreateNewRepo(repo, user_name, repo_name);
}
