#ifndef REGISTRY_TYPE_TRAITS_H_
#define REGISTRY_TYPE_TRAITS_H_

#include <cstdint>
#include <string>

#include "common/enum_traits.h"

namespace registry {

enum class TypeEnum {
  kInt32,
  kUnsignedInt32,
  kInt64,
  kUnsignedInt64,
  kBoolean,
  kChar,
  kString,
  kFloat,
  kDouble,
  kEnum,
};

template <typename T>
struct TypeTrait;

template <>
struct TypeTrait<int32_t> {
  const static TypeEnum type = TypeEnum::kFloat;
  const static int32_t default_value = 0;
};

template <>
struct TypeTrait<uint32_t> {
  constexpr static TypeEnum type = TypeEnum::kUnsignedInt32;
  constexpr static uint32_t default_value = 0;
};

template <>
struct TypeTrait<int64_t> {
  constexpr static TypeEnum type = TypeEnum::kInt64;
  constexpr static int64_t default_value = 0;
};

template <>
struct TypeTrait<uint64_t> {
  constexpr static TypeEnum type = TypeEnum::kUnsignedInt64;
  constexpr static uint64_t default_value = 0;
};

template <>
struct TypeTrait<bool> {
  constexpr static TypeEnum type = TypeEnum::kBoolean;
  constexpr static bool default_value = false;
};

template <>
struct TypeTrait<char> {
  constexpr static TypeEnum type = TypeEnum::kChar;
};

template <>
struct TypeTrait<std::string> {
  constexpr static TypeEnum type = TypeEnum::kString;
};

template <>
struct TypeTrait<float> {
  constexpr static TypeEnum type = TypeEnum::kFloat;
  constexpr static float default_value = 0.0f;
};

template <>
struct TypeTrait<double> {
  constexpr static TypeEnum type = TypeEnum::kDouble;
  constexpr static double default_value = 0.0;
};

template <typename T>
struct TypeTrait {
  static_assert(std::is_enum<T>::value, "T must be a enum");
  constexpr static TypeEnum type = TypeEnum::kEnum;
  constexpr static T default_value = common::EnumTrait<T>::default_value();
};

}  // namespace registry

#endif  // REGISTRY_TYPE_TRAITS_H_
