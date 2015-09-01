// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/crypto.h"
#include "base/gtest/gtest.h"
#include "base/string.h"
#include "base/value_of.hh"

namespace thilenius {
namespace base {
namespace {

TEST(CryptoTest, Base64Test) {
  std::string original = "A long string that should be base 64ed";
  EXPECT_TRUE(Crypto::ToBase64(original).IsValid());
  std::string base_64 = Crypto::ToBase64(original).GetOrDie();
  EXPECT_GT(base_64.size(), 0);
  EXPECT_EQ(Crypto::FromBase64(base_64).GetOrDie(), original);
  char invalid_base_64[4];
  invalid_base_64[0] = static_cast<char>(188);
  invalid_base_64[1] = static_cast<char>(200);
  invalid_base_64[2] = static_cast<char>(255);
  invalid_base_64[3] = static_cast<char>(0);
  EXPECT_FALSE(Crypto::FromBase64(invalid_base_64).IsValid());
}

TEST(CryptoTest, SaltTest) {
  EXPECT_TRUE(Crypto::GenerateSaltBase64().IsValid());
  EXPECT_GT(Crypto::GenerateSaltBase64().GetOrDie().size(), 0);
}

TEST(CryptoTest, HashPasswordTestEmpty) {
  std::string password = "";
  std::string salt = Crypto::GenerateSaltBase64().GetOrDie();
  ValueOf<std::string> hash_value = Crypto::HashPassword(salt, password);
  EXPECT_TRUE(hash_value.IsValid());
  std::string hash = hash_value.GetOrDie();
  EXPECT_EQ(hash, Crypto::HashPassword(salt, password).GetOrDie());
  std::string salt_copy = salt;
  EXPECT_EQ(hash, Crypto::HashPassword(salt_copy, password).GetOrDie());
}

TEST(CryptoTest, HashPasswordTestShort) {
  std::string password = "d";
  std::string salt = Crypto::GenerateSaltBase64().GetOrDie();
  ValueOf<std::string> hash_value = Crypto::HashPassword(salt, password);
  EXPECT_TRUE(hash_value.IsValid());
  std::string hash = hash_value.GetOrDie();
  EXPECT_EQ(hash, Crypto::HashPassword(salt, password).GetOrDie());
  std::string salt_copy = salt;
  EXPECT_EQ(hash, Crypto::HashPassword(salt_copy, password).GetOrDie());
}

TEST(CryptoTest, HashPasswordTestLong) {
  std::string password = "some_long and Odd*Passwo!@#d ";
  std::string salt = Crypto::GenerateSaltBase64().GetOrDie();
  ValueOf<std::string> hash_value = Crypto::HashPassword(salt, password);
  EXPECT_TRUE(hash_value.IsValid());
  std::string hash = hash_value.GetOrDie();
  EXPECT_EQ(hash, Crypto::HashPassword(salt, password).GetOrDie());
  std::string salt_copy = salt;
  EXPECT_EQ(hash, Crypto::HashPassword(salt_copy, password).GetOrDie());
}

}  // namespace
}  // namespace base
}  // namespace thilenius
