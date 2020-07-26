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

using TestTypes =
    testing::Types<NonCopyMovable, Movable, Copyable, CopyMovable>;

TYPED_TEST_SUITE(WeakRefCountedTest, TestTypes);

TYPED_TEST(WeakRefCountedTest, ConstructDestructTest) {
  WeakRefCounted<TypeParam> test_object;
  EXPECT_FALSE(test_object.HasWeakRef());
}

TYPED_TEST(WeakRefCountedTest, WeakRefCreationTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);
  {
    WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 1u);
    EXPECT_EQ(weak_ref.WeakCount(), 1u);
  }
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);
}

TYPED_TEST(WeakRefCountedTest, CopyConstructionTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);

  WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);
  EXPECT_EQ(weak_ref.WeakCount(), 1u);

  WeakRefCounted<TypeParam> another_weak_ref(weak_ref);
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 2u);
  EXPECT_EQ(weak_ref.WeakCount(), 2u);
  EXPECT_EQ(another_weak_ref.WeakCount(), 2u);
}

TYPED_TEST(WeakRefCountedTest, MoveConstructionTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);

  WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);
  EXPECT_EQ(weak_ref.WeakCount(), 1u);

  {
    WeakRefCounted<TypeParam> moved_ref(std::move(weak_ref));
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 1u);
    EXPECT_EQ(weak_ref.UseCount(), 0u);
    EXPECT_EQ(weak_ref.WeakCount(), 0u);
    EXPECT_EQ(moved_ref.UseCount(), 1u);
    EXPECT_EQ(moved_ref.WeakCount(), 1u);
  }
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);
  EXPECT_EQ(weak_ref.UseCount(), 0u);
  EXPECT_EQ(weak_ref.WeakCount(), 0u);
}

TYPED_TEST(WeakRefCountedTest, CopyAssignmentTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);

  WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);

  {
    WeakRefCounted<TypeParam> weak_ref_copy;
    weak_ref_copy = weak_ref;
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 2u);
    EXPECT_EQ(weak_ref.UseCount(), 1u);
    EXPECT_EQ(weak_ref.WeakCount(), 2u);
    EXPECT_EQ(weak_ref_copy.UseCount(), 1u);
    EXPECT_EQ(weak_ref_copy.WeakCount(), 2u);
  }
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);
  EXPECT_EQ(weak_ref.UseCount(), 1u);
  EXPECT_EQ(weak_ref.WeakCount(), 1u);
}

TYPED_TEST(WeakRefCountedTest, MoveAssignmentTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);

  WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);

  {
    WeakRefCounted<TypeParam> moved_weak_ref;
    moved_weak_ref = std::move(weak_ref);
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 1u);
    EXPECT_EQ(weak_ref.UseCount(), 0u);
    EXPECT_EQ(weak_ref.WeakCount(), 0u);
    EXPECT_EQ(moved_weak_ref.UseCount(), 1u);
    EXPECT_EQ(moved_weak_ref.WeakCount(), 1u);
  }
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);
  EXPECT_EQ(weak_ref.UseCount(), 0u);
  EXPECT_EQ(weak_ref.WeakCount(), 0u);
}

TYPED_TEST(WeakRefCountedTest, ExpirationTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));
  WeakRefCounted<TypeParam> weak_ref;
  {
    RefCounted<TypeParam> moved_obj(std::move(obj));
    weak_ref = moved_obj.GetWeakRef();
    EXPECT_EQ(moved_obj.UseCount(), 1u);
    EXPECT_EQ(moved_obj.WeakCount(), 1u);
    EXPECT_EQ(weak_ref.WeakCount(), 1u);
  }
  EXPECT_EQ(obj.UseCount(), 0u);
  EXPECT_EQ(weak_ref.UseCount(), 0u);
  EXPECT_EQ(weak_ref.WeakCount(), 1u);
}

TYPED_TEST(WeakRefCountedTest, InheritanceTest) {
  RefCounted<TypeParam> obj(new TypeParam(12));

  {
    // Copy construction
    WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
    WeakRefCounted<Base> base_weak_ref(weak_ref);
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 2u);
    EXPECT_EQ(weak_ref.UseCount(), 1u);
    EXPECT_EQ(weak_ref.WeakCount(), 2u);
    EXPECT_EQ(base_weak_ref.UseCount(), 1u);
    EXPECT_EQ(base_weak_ref.WeakCount(), 2u);
  }
  {
    // Move construction
    WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
    WeakRefCounted<Base> base_weak_ref(std::move(weak_ref));
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 1u);
    EXPECT_EQ(weak_ref.UseCount(), 0u);
    EXPECT_EQ(weak_ref.WeakCount(), 0u);
    EXPECT_EQ(base_weak_ref.UseCount(), 1u);
    EXPECT_EQ(base_weak_ref.WeakCount(), 1u);
  }
  {
    // Copy assignment
    WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
    WeakRefCounted<Base> base_weak_ref;
    EXPECT_EQ(base_weak_ref.UseCount(), 0u);
    EXPECT_EQ(base_weak_ref.WeakCount(), 0u);
    base_weak_ref = weak_ref;
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 2u);
    EXPECT_EQ(weak_ref.UseCount(), 1u);
    EXPECT_EQ(weak_ref.WeakCount(), 2u);
    EXPECT_EQ(base_weak_ref.UseCount(), 1u);
    EXPECT_EQ(base_weak_ref.WeakCount(), 2u);
  }
  {
    // Move assignment
    WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
    WeakRefCounted<Base> base_weak_ref;
    EXPECT_EQ(base_weak_ref.UseCount(), 0u);
    EXPECT_EQ(base_weak_ref.WeakCount(), 0u);
    base_weak_ref = std::move(weak_ref);
    EXPECT_EQ(obj.UseCount(), 1u);
    EXPECT_EQ(obj.WeakCount(), 1u);
    EXPECT_EQ(weak_ref.UseCount(), 0u);
    EXPECT_EQ(weak_ref.WeakCount(), 0u);
    EXPECT_EQ(base_weak_ref.UseCount(), 1u);
    EXPECT_EQ(base_weak_ref.WeakCount(), 1u);
  }

  EXPECT_EQ(obj.UseCount(), 1u);
}

}  // namespace common
