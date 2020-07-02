#include "common/memory/weak_ref_counted.h"

#include <utility>

#include "gtest/gtest.h"

#include "common/memory/ref_counted.h"
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

// using TestTypes =
//     testing::Types<NonCopyMovable, Movable, Copyable, CopyMovable>;

using TestTypes = testing::Types<NonCopyMovable>;

TYPED_TEST_SUITE(WeakRefCountedTest, TestTypes);

TYPED_TEST(WeakRefCountedTest, ConstructDestructTest) {
  WeakRefCounted<TypeParam> test_object;
  EXPECT_FALSE(test_object.HasWeakRef());
}

TYPED_TEST(WeakRefCountedTest, WeakRefCreationTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);
  {
    WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 2u);
    EXPECT_EQ(weak_ref.WeakCount(), 2u);

    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 2u);
    EXPECT_EQ(weak_ref.WeakCount(), 2u);
  }
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);
}

TYPED_TEST(WeakRefCountedTest, CopyConstructionTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);

  const WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 2u);
  EXPECT_EQ(weak_ref.WeakCount(), 2u);

  std::cout << "Constructing" << std::endl;
  WeakRefCounted<TypeParam> another_weak_ref(weak_ref);
  std::cout << "Done" << std::endl;
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 3u);
  EXPECT_EQ(weak_ref.WeakCount(), 3u);
  EXPECT_EQ(another_weak_ref.WeakCount(), 3u);
}

TYPED_TEST(WeakRefCountedTest, ExpirationTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  {
    RefCounted<TypeParam> moved_obj(std::move(obj));
    WeakRefCounted<TypeParam> weak_ref = moved_obj.GetWeakRef();
    EXPECT_EQ(moved_obj.UseCount(), 1u);
    EXPECT_EQ(moved_obj.WeakCount(), 2u);
    EXPECT_EQ(weak_ref.WeakCount(), 2u);
  }
  EXPECT_EQ(obj.UseCount(), 0u);
}

}  // namespace common
