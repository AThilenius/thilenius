// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "scorch/cloud/crucible/crucible_client.h"

#include <gflags/gflags.h>
#include <set>

#include "base/directory.h"
#include "base/file.h"
#include "base/log.h"
#include "base/path.h"
#include "base/string.h"
#include "base/time.h"

DEFINE_string(crucible_dir_name, ".anvil",
              "The name of the dir crucible uses to store data.");
DEFINE_string(crucible_repo_file_cache_name, "crucible_repo.json",
              "The name of the file within crucible_dir_name that crucible "
              "uses to store the serialized repo.");

using ::thilenius::base::Directory;
using ::thilenius::base::File;
using ::thilenius::base::Path;
using ::thilenius::base::String;
using ::thilenius::base::Time;

namespace thilenius {
namespace scorch {
namespace cloud {
namespace crucible {

CrucibleClient::CrucibleClient() : connected_(false) {}

ValueOf<void> CrucibleClient::Connect(const std::string& crucible_ip,
                                      int crucible_port,
                                      const std::string& crucible_route,
                                      const std::string& sentinel_ip,
                                      int sentinel_port,
                                      const std::string& sentinel_route) {
  if (connected_) {
    return {"Already connected"};
  }
  crucible_ip_ = crucible_ip;
  crucible_port_ = crucible_port;
  crucible_route_ = crucible_route;
  sentinel_ip_ = sentinel_ip;
  sentinel_port_ = sentinel_port;
  sentinel_route_ = sentinel_route;
  // Connect crucible
  LOG(INFO) << "Connection to Crucible";
  http_client_ptr_ = ProtoCrucibleClientPtr(
      new ThriftHttpClient<::crucible::proto::CrucibleClient>(
          crucible_ip, crucible_port, crucible_route));
  auto connection = http_client_ptr_->Connect();
  if (!connection.IsValid()) {
    return {connection.GetError()};
  }
  // Connect sentinel
  LOG(INFO) << "Connection to Sentinel";
  auto sentinel_connection =
      sentinel_client_.Connect(sentinel_ip_, sentinel_port_, sentinel_route_);
  if (!sentinel_connection.IsValid()) {
    return {sentinel_connection.GetError()};
  }
  connected_ = true;
  return {};
}

ValueOf<CrucibleRepo> CrucibleClient::CreateNewBaseRepoInDirectory(
    const std::string& path, const std::string& repo_name) {
  ValueOf<::sentinel::proto::Token> token_value = LoginAndAuthroSecondary(path);
  if (!token_value.IsValid()) {
    return {CrucibleRepo(), token_value.GetError()};
  }
  ::sentinel::proto::Token token = token_value.GetOrDie();
  // Set up a crucible project
  std::string crucible_dir_path = Path::Combine(path, FLAGS_crucible_dir_name);
  std::string crucible_repo_json_path =
      Path::Combine(crucible_dir_path, FLAGS_crucible_repo_file_cache_name);
  if (File::Exists(crucible_repo_json_path)) {
    return {CrucibleRepo(), StrCat("Cannot create a new repo at ", path,
                                   ", a repo already exists there")};
  }
  Directory::Create(crucible_dir_path);
  ::crucible::proto::Repo repo_proto;
  try {
    http_client_ptr_->ConnectOrDie()->CreateBaseRepo(repo_proto, token,
                                                     repo_name);
  } catch (::crucible::proto::OperationFailure op_failure) {
    return {CrucibleRepo(),
            StrCat("Crucible remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {CrucibleRepo(),
            "In CreateBaseRepo, Crucible server threw an unhandled exception."};
  }
  // Write the repo state proto to file before passing it off to CrucibleRepo
  // for management
  std::string json = crucible_mapper_.repo_mapper.to_json(repo_proto);
  File::WriteToFile(crucible_repo_json_path, json);
  CrucibleRepo crucible_repo;
  crucible_repo.Connect(crucible_ip_, crucible_port_, crucible_route_, path);
  ValueOf<void> results = crucible_repo.SyncToHead();
  if (!results.IsValid()) {
    return {CrucibleRepo(), results.GetError()};
  }
  return std::move(crucible_repo);
}

ValueOf<CrucibleRepo> CrucibleClient::CloneBaseRepoInDirectory(
    const std::string& path, const std::string& base_repo_name) {
  ValueOf<::sentinel::proto::Token> token_value = LoginAndAuthroSecondary(path);
  if (!token_value.IsValid()) {
    return {CrucibleRepo(), token_value.GetError()};
  }
  ::sentinel::proto::Token token = token_value.GetOrDie();
  // Set up a crucible project
  std::string crucible_dir_path = Path::Combine(path, FLAGS_crucible_dir_name);
  std::string crucible_repo_json_path =
      Path::Combine(crucible_dir_path, FLAGS_crucible_repo_file_cache_name);
  if (File::Exists(crucible_repo_json_path)) {
    return {CrucibleRepo(), StrCat("Cannot create a new repo at ", path,
                                   ", a repo already exists there")};
  }
  Directory::Create(crucible_dir_path);
  ::crucible::proto::Repo repo_proto;
  try {
    http_client_ptr_->ConnectOrDie()->CreateForkedRepo(repo_proto, token,
                                                       base_repo_name);
  } catch (::crucible::proto::OperationFailure op_failure) {
    return {CrucibleRepo(),
            StrCat("Crucible remote exception: ", op_failure.user_message)};
  } catch (...) {
    return {CrucibleRepo(),
            "In CloneBaseRepoInDirectory, Crucible server threw an unhandled "
            "exception."};
  }
  // Write the repo state proto to file before passing it off to CrucibleRepo
  // for management
  std::string json = crucible_mapper_.repo_mapper.to_json(repo_proto);
  File::WriteToFile(crucible_repo_json_path, json);
  CrucibleRepo crucible_repo;
  crucible_repo.Connect(crucible_ip_, crucible_port_, crucible_route_, path);
  ValueOf<void> results = crucible_repo.SyncToHead();
  if (!results.IsValid()) {
    return {CrucibleRepo(), results.GetError()};
  }
  return std::move(crucible_repo);
}

ValueOf<CrucibleRepo> CrucibleClient::LoadRepoFromDirectory(
    const std::string& path) {
  if (!connected_) {
    return {CrucibleRepo(), "Client must be connected before use"};
  }
  CrucibleRepo crucible_repo;
  crucible_repo.Connect(crucible_ip_, crucible_port_, crucible_route_, path);
  return std::move(crucible_repo);
}

ValueOf<::sentinel::proto::Token> CrucibleClient::LoginAndAuthroSecondary(
    const std::string& path) {
  if (!connected_) {
    return {::sentinel::proto::Token(), "Client must be connected before use"};
  }
  ValueOf<::sentinel::proto::Token> token_value =
      sentinel_client_.LoginUserFromCin();
  if (!token_value.IsValid()) {
    return {::sentinel::proto::Token(),
            StrCat("Sentinel Exception: ", token_value.GetError())};
  }
  ::sentinel::proto::Token token = token_value.GetOrDie();
  // Before doing anything else, author a secondary token with the primary
  ValueOf<::sentinel::proto::Token> stoken_value = sentinel_client_.AuthorToken(
      token, ::sentinel::proto::g_sentinel_constants.SECONDARY_THRESHOLD);
  if (!stoken_value.IsValid()) {
    return {::sentinel::proto::Token(),
            StrCat("Sentinel Exception: ", stoken_value.GetError())};
  }
  sentinel_client_.SaveProjectToken(stoken_value.GetOrDie(), path);
  return std::move(token);
}

}  // namespace crucible
}  // namespace cloud
}  // namespace scorch
}  // namespace thilenius
