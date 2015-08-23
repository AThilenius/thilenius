// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/file.h"
#include "base/gtest/gtest.h"
#include "base/http.h"
#include "base/string.h"

namespace thilenius {
namespace base {
namespace {

TEST(HttpTest, GetExampleComTest) {
  EXPECT_LT(0, Http::GetContent("http://example.com").GetOrDie().size());
  EXPECT_LT(0, Http::GetContent("https://example.com").GetOrDie().size());
}

TEST(HttpTest, DoesNotExistTest) {
  EXPECT_FALSE(Http::GetContent("http://thisdomainDNEj2039jf9jfojf89s.com")
                   .IsValid());
}

TEST(HttpTest, DownloadFileTest) {
  std::string html = Http::GetContent("https://example.com").GetOrDie();
  EXPECT_EQ("", Http::DownloadToFile("https://example.com", "output.html"));
  EXPECT_EQ(html, File::ReadContentsOrDie("output.html"));
}

}  // namespace
}  // namespace base
}  // namespace thilenius
