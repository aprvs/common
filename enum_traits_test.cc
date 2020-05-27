#include "common/enum_traits.h"

#include <utility>
#include "gtest/gtest.h"

namespace common {

namespace {

enum class MyEnum1 { kA, kB, kC, kD, kE };

enum MyEnum2 { kMyEnum2A, kMyEnum2B, kMyEnum2C };

class MyClass {};

}  // namespace

template <>
MyEnum1 EnumTrait<MyEnum1>::default_value() {
  return MyEnum1::kD;
}

template <>
constexpr size_t EnumTrait<MyEnum1>::num_values() {
  return 5;
}

template <>
const char* EnumTrait<MyEnum1>::to_string(MyEnum1 e) {
  switch (e) {
    case MyEnum1::kA:
      return "A";
    case MyEnum1::kB:
      return "B";
    case MyEnum1::kC:
      return "C";
    case MyEnum1::kD:
      return "D";
    case MyEnum1::kE:
      return "E";
  }
  return "Unknown";
}

template <>
std::array<MyEnum1, EnumTrait<MyEnum1>::num_values()>
EnumListTrait<MyEnum1>::values() {
  return {{MyEnum1::kA, MyEnum1::kB, MyEnum1::kC, MyEnum1::kD, MyEnum1::kE}};
}

template <>
MyEnum2 EnumTrait<MyEnum2>::default_value() {
  return kMyEnum2B;
}

TEST(EnumTraitsTest, DefaultValueTest) {
  EXPECT_EQ(EnumTrait<MyEnum1>::default_value(), MyEnum1::kD);
  EXPECT_EQ(EnumTrait<MyEnum2>::default_value(), kMyEnum2B);
}

TEST(EnumTraitsTest, ArrayTest) {
  for (MyEnum1 val : EnumListTrait<MyEnum1>::values()) {
    std::cout << EnumTrait<MyEnum1>::to_string(val) << std::endl;
  }
}

}  // namespace common
