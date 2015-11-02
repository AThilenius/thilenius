// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/http.h"

#include <curl/curl.h>
#include <string>

#include "base/log.h"
#include "base/string.h"
#include "base/types.h"

namespace thilenius {
namespace base {
namespace {

size_t WriteString(void* ptr, size_t size, size_t count, void* stream) {
  ((std::string*)stream)->append((char*)ptr, 0, size * count);
  return size * count;
}

size_t WriteFile(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

class ScopedCurlEasy {
 public:
  ScopedCurlEasy() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
      curl_global_cleanup();
    }
  }

  ~ScopedCurlEasy() {
    if (curl) {
      curl_easy_cleanup(curl);
      curl_global_cleanup();
    }
  }

  CURL* curl;
};

struct WriteThis {
  const char* readptr;
  int sizeleft;
};

size_t read_callback(void* ptr, size_t size, size_t nmemb, void* userp) {
  struct WriteThis* pooh = (struct WriteThis*)userp;
  if (size * nmemb) {
    *(char*)ptr = pooh->readptr[0];
    pooh->readptr++;
    pooh->sizeleft--;
    return 1;
  }
  return 0;
}

}  // namespace

ValueOf<std::string> Http::GetContent(const std::string& url) {
  ScopedCurlEasy scoped_curl_easy;
  CURL* curl = scoped_curl_easy.curl;
  if (!curl) {
    return {"", "Failed to initialize Lib Curl."};
  }
  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteString);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    return {"", StrCat("Failed to get content from ", url, ". Error: ",
                       curl_easy_strerror(res))};
  }
  return {std::move(response)};
}

ValueOf<std::string> Http::PostContent(const std::string& url,
                                       const std::string& json) {
  ScopedCurlEasy scoped_curl_easy;
  CURL* curl = scoped_curl_easy.curl;
  if (!curl) {
    return {"", "Failed to initialize Lib Curl."};
  }
  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteString);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    return {"", StrCat("Failed to get content from ", url, ". Error: ",
                       curl_easy_strerror(res))};
  }
  return {std::move(response)};
}

std::string Http::DownloadToFile(const std::string& url,
                                 const std::string& file_path) {
  ScopedCurlEasy scoped_curl_easy;
  CURL* curl = scoped_curl_easy.curl;
  if (!curl) {
    return "Failed to initialize Lib Curl.";
  }
  FILE* file = std::fopen(file_path.c_str(), "wb");
  if (!file) {
    return StrCat("Failed to open file at path: ", file_path);
  }
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFile);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    return StrCat("Failed to fetch file from URL: ", url);
  }
  std::fclose(file);
  return "";
}

}  // namespace base
}  // namespace thilenius
