#ifndef COMMON_ENUM_TRAITS_H_
#define COMMON_ENUM_TRAITS_H_

#include <array>
#include <cstdint>
#include <utility>

namespace common {

template <typename T,
          typename = typename std::enable_if<std::is_enum<T>::value>::type>
struct EnumTrait {
  constexpr static size_t num_values();
  constexpr static T default_value();
  constexpr static const char* to_string(T e);
};

template <typename T,
          typename = typename std::enable_if<std::is_enum<T>::value>::type>
struct EnumListTrait {
  static constexpr std::array<T, EnumTrait<T>::num_values()> values();
};

}  // namespace common

#endif  // COMMON_ENUM_TRAITS_H_
