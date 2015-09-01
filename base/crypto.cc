// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/crypto.h"

#include <cassert>
#include <cctype>
#include <limits>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdexcept>
#include <string>

namespace thilenius {
namespace base {
namespace {

const char b64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char reverse_table[128] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64

};

int iteration_count_ = 1000;

}  // namespace

ValueOf<std::string> Crypto::GenerateSaltBase64() {
  unsigned char salt[256];
  if (!RAND_bytes(salt, 256)) {
    return {"", "Failed to generate random bytes with OpenSSL"};
  }
  std::string salt_str(reinterpret_cast<char*>(salt), 256);
  return Crypto::ToBase64(salt_str);
}

ValueOf<std::string> Crypto::HashPassword(const std::string& salt_base_64,
                                          const std::string& password) {
  ValueOf<std::string> salt_binary_value = Crypto::FromBase64(salt_base_64);
  if (!salt_binary_value.IsValid()) {
    return {"", std::move(salt_binary_value.GetError())};
  }
  std::string salt_binary = salt_binary_value.GetOrDie();
  unsigned char output[256];
  PKCS5_PBKDF2_HMAC_SHA1(
      password.c_str(), password.length(),
      reinterpret_cast<const unsigned char*>(salt_binary.c_str()),
      salt_binary.length(), iteration_count_, 256, output);
  SHA256_CTX context;
  unsigned char md[SHA256_DIGEST_LENGTH];
  SHA256_Init(&context);
  SHA256_Update(&context, (unsigned char*)output, 256);
  SHA256_Final(md, &context);
  return Crypto::ToBase64(std::string(reinterpret_cast<const char*>(md), 256));
}

ValueOf<std::string> Crypto::ToBase64(const std::string& binary_data) {
  using ::std::string;
  using ::std::numeric_limits;

  if (binary_data.size() >
      (numeric_limits<string::size_type>::max() / 4u) * 3u) {
    return {"", "Converting too large a string to base64."};
  }

  const ::std::size_t binlen = binary_data.size();
  // Use = signs so the end is properly padded.
  string retval((((binlen + 2) / 3) * 4), '=');
  ::std::size_t outpos = 0;
  int bits_collected = 0;
  unsigned int accumulator = 0;
  const string::const_iterator binend = binary_data.end();

  for (string::const_iterator i = binary_data.begin(); i != binend; ++i) {
    accumulator = (accumulator << 8) | (*i & 0xffu);
    bits_collected += 8;
    while (bits_collected >= 6) {
      bits_collected -= 6;
      retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
    }
  }
  if (bits_collected > 0) {  // Any trailing bits that are missing.
    assert(bits_collected < 6);
    accumulator <<= 6 - bits_collected;
    retval[outpos++] = b64_table[accumulator & 0x3fu];
  }
  assert(outpos >= (retval.size() - 2));
  assert(outpos <= retval.size());
  return {std::move(retval)};
}

ValueOf<std::string> Crypto::FromBase64(const std::string& base_64_data) {
  using ::std::string;
  string retval;
  const string::const_iterator last = base_64_data.end();
  int bits_collected = 0;
  unsigned int accumulator = 0;

  for (string::const_iterator i = base_64_data.begin(); i != last; ++i) {
    const int c = *i;
    if (::std::isspace(c) || c == '=') {
      // Skip whitespace and padding. Be liberal in what you accept.
      continue;
    }
    if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
      return {"",
              "This contains characters not legal in a base64 encoded string."};
    }
    accumulator = (accumulator << 6) | reverse_table[c];
    bits_collected += 6;
    if (bits_collected >= 8) {
      bits_collected -= 8;
      retval += (char)((accumulator >> bits_collected) & 0xffu);
    }
  }
  return {std::move(retval)};
}

}  // namespace base
}  // namespace thilenius
