// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_STRING_H_
#define BASE_STRING_H_

#include <cstring>
#include <string>

// No namespace

namespace str_cat_detail {

template <typename>
struct string_size_impl;

template <size_t N>
struct string_size_impl<const char[N]> {
  static constexpr size_t size(const char(&)[N]) { return N - 1; }
};

template <size_t N>
struct string_size_impl<char[N]> {
  static size_t size(char(&s)[N]) { return N ? std::strlen(s) : 0; }
};

template <>
struct string_size_impl<const char*> {
  static size_t size(const char* s) { return s ? std::strlen(s) : 0; }
};

template <>
struct string_size_impl<char*> {
  static size_t size(char* s) { return s ? std::strlen(s) : 0; }
};

template <>
struct string_size_impl<std::string> {
  static size_t size(const std::string& s) { return s.size(); }
};

template <typename String>
size_t string_size(String&& s) {
  using noref_t = typename std::remove_reference<String>::type;
  using string_t =
      typename std::conditional<std::is_array<noref_t>::value, noref_t,
                                typename std::remove_cv<noref_t>::type>::type;
  return string_size_impl<string_t>::size(s);
}

template <typename...>
struct concatenate_impl;

template <typename String>
struct concatenate_impl<String> {
  static size_t size(String&& s) { return string_size(s); }
  static void concatenate(std::string& result, String&& s) { result += s; }
};

template <typename String, typename... Rest>
struct concatenate_impl<String, Rest...> {
  static size_t size(String&& s, Rest&&... rest) {
    return string_size(s) +
           concatenate_impl<Rest...>::size(std::forward<Rest>(rest)...);
  }
  static void concatenate(std::string& result, String&& s, Rest&&... rest) {
    result += s;
    concatenate_impl<Rest...>::concatenate(result, std::forward<Rest>(rest)...);
  }
};

}  // namespace str_cat_detail

template <typename... Strings>
std::string StrCat(Strings&&... strings) {
  std::string result;
  result.reserve(str_cat_detail::concatenate_impl<Strings...>::size(
      std::forward<Strings>(strings)...));
  str_cat_detail::concatenate_impl<Strings...>::concatenate(
      result, std::forward<Strings>(strings)...);
  return result;
}

bool Empty(const std::string& str);
bool Blank(const std::string& str);
bool BeginsWith(const std::string& str, const std::string& other_str);
bool EndsWith(const std::string& str, const std::string& other_str);

namespace thilenius {
namespace base {

class String {
 public:
  static bool BeginsWith(const std::string& str, const std::string& other_str);
  static bool Blank(const std::string& str);
  static bool Empty(const std::string& str);
  static bool EndsWith(const std::string& str, const std::string& other_str);
  static std::string RemoveFromBeginning(const std::string& from_string,
                                         const std::string& value);
  static std::string RemoveFromEnd(const std::string& from_string,
                                   const std::string& value);
};

} // namespace base
} // namespace thilenius

#endif  // BASE_STRING_H_
