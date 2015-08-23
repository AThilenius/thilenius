// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/http.h"

#include "base/curl/curl.h"
#include "base/log.h"
#include "base/string.h"
#include "base/types.h"

namespace thilenius {
namespace base {
namespace {

// Mostly bull shit needed to make shitty C-code in libcurl work
std::string read_buffer_;

size_t WriteString(void* contents, size_t size, size_t nmemb, void* userp) {
  size_t realsize = size * nmemb;
  read_buffer_ = std::move(std::string(static_cast<char*>(contents), realsize));
  return realsize;
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

}  // namespace

ValueOf<std::string> Http::GetContent(const std::string& url) {
  ScopedCurlEasy scoped_curl_easy;
  CURL* curl = scoped_curl_easy.curl;
  if (!curl) {
    return {"", "Failed to initialize Lib Curl."};
  }
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteString);
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    return {"", StrCat("Failed to get content from ", url, ". Error: ",
                       curl_easy_strerror(res))};
  }
  return {std::move(read_buffer_)};
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
