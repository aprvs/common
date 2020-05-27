#include "common/enum_traits.h"

#include <utility>
#include "gtest/gtest.h"

namespace test_namespace {

namespace {

enum class MyEnum1 { kA, kB, kC, kD, kE };

enum MyEnum2 { kMyEnum2A, kMyEnum2B, kMyEnum2C };

}  // namespace
}  // test_namespace

namespace common {

template <>
struct EnumTrait<test_namespace::MyEnum1> {
  constexpr static test_namespace::MyEnum1 default_value() {
    return test_namespace::MyEnum1::kD;
  }
  constexpr static size_t num_values() { return 5; }
  constexpr static const char* to_string(test_namespace::MyEnum1 e) {
    switch (e) {
      case test_namespace::MyEnum1::kA:
        return "A";
      case test_namespace::MyEnum1::kB:
        return "B";
      case test_namespace::MyEnum1::kC:
        return "C";
      case test_namespace::MyEnum1::kD:
        return "D";
      case test_namespace::MyEnum1::kE:
        return "E";
    }
    return "Unknown";
  }
};

template <>
std::array<test_namespace::MyEnum1,
           EnumTrait<test_namespace::MyEnum1>::num_values()>
EnumListTrait<test_namespace::MyEnum1>::values() {
  return {{test_namespace::MyEnum1::kA, test_namespace::MyEnum1::kB,
           test_namespace::MyEnum1::kC, test_namespace::MyEnum1::kD,
           test_namespace::MyEnum1::kE}};
}

template <>
test_namespace::MyEnum2 EnumTrait<test_namespace::MyEnum2>::default_value() {
  return test_namespace::kMyEnum2B;
}

}  // namespace  common
namespace test_namespace {

TEST(EnumTraitsTest, DefaultValueTest) {
  EXPECT_EQ(common::EnumTrait<MyEnum1>::default_value(), MyEnum1::kD);
  EXPECT_EQ(common::EnumTrait<MyEnum2>::default_value(), kMyEnum2B);
}

TEST(EnumTraitsTest, ArrayTest) {
  const auto& values = common::EnumListTrait<MyEnum1>::values();
  EXPECT_EQ(values[0], MyEnum1::kA);
  EXPECT_EQ(common::EnumTrait<MyEnum1>::to_string(values[0]), "A");
  EXPECT_EQ(values[1], MyEnum1::kB);
  EXPECT_EQ(common::EnumTrait<MyEnum1>::to_string(values[1]), "B");
  EXPECT_EQ(values[2], MyEnum1::kC);
  EXPECT_EQ(common::EnumTrait<MyEnum1>::to_string(values[2]), "C");
  EXPECT_EQ(values[3], MyEnum1::kD);
  EXPECT_EQ(common::EnumTrait<MyEnum1>::to_string(values[3]), "D");
  EXPECT_EQ(values[4], MyEnum1::kE);
  EXPECT_EQ(common::EnumTrait<MyEnum1>::to_string(values[4]), "E");
}

}  // namespace test_namespace
