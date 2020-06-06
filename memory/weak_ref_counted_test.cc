#include "common/memory/weak_ref_counted.h"

#include "gtest/gtest.h"

#include "common/test_structures/base_types.h"

namespace common {

using test_structures::Base;
using test_structures::NonCopyMovable;
using test_structures::Movable;
using test_structures::Copyable;
using test_structures::CopyMovable;

template <typename T>
struct WeakRefCountedTest : public testing::Test {
  using ParamType = T;
};

using TestTypes =
    testing::Types<NonCopyMovable, Movable, Copyable, CopyMovable>;

TYPED_TEST_SUITE(WeakRefCountedTest, TestTypes);

TYPED_TEST(WeakRefCountedTest, ConstructDestructTest) {
  WeakRefCounted<TypeParam> test_object;
  EXPECT_TRUE(test_object.HasWeakRef());
}

}  // namespace common
