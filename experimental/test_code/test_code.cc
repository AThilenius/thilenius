// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>

#include "base/http.h"
#include "base/json.h"
#include "base/log.h"
#include "base/string.h"

using ::thilenius::base::Http;
using ::thilenius::base::String;

int main(int argc, char** argv) {
  std::string json = "{\"user_uuid\":\"doesntmatter\",\"minecraft_username\":\"alth3531\",\"dimension\":0,\"target\": \"move\",\"json\":{\"direction\":\"Forward\"}}";
  LOG(INFO) << Http::PostContent("192.168.59.3:2500", json).GetOrDie();
  //LOG(INFO) << Http::PostContent("http://posttestserver.com/post.php",
                                 //"{ \"Hello\":\"World\" }").GetOrDie();
  return 0;
}
