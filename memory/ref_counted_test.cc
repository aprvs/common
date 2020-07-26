#include "common/memory/ref_counted.h"

#include <cstdint>
#include <utility>

#include "gtest/gtest.h"

#include "common/test_structures/base_types.h"

namespace common {

using test_structures::Base;
using test_structures::NonCopyMovable;
using test_structures::Movable;
using test_structures::Copyable;
using test_structures::CopyMovable;

template <typename T>
struct RefCountedTest : public testing::Test {};

using TestTypes =
    testing::Types<NonCopyMovable, Movable, Copyable, CopyMovable>;

TYPED_TEST_SUITE(RefCountedTest, TestTypes);

TYPED_TEST(RefCountedTest, ConstructDestructTest) {
  RefCounted<TypeParam> test_object;
  EXPECT_EQ(test_object.UseCount(), 0u);
}

TYPED_TEST(RefCountedTest, ConstructAssignTest) {
  std::size_t destructor_call_count = 0u;
  RefCounted<TypeParam> test_object1(new TypeParam(12, &destructor_call_count));
  EXPECT_EQ(test_object1.UseCount(), 1);
  EXPECT_EQ(test_object1->value_, 12);

  RefCounted<TypeParam> test_object2(test_object1);
  EXPECT_EQ(test_object1.UseCount(), 2);
  EXPECT_EQ(test_object2.UseCount(), 2);

  EXPECT_EQ(test_object1->value_, 12);
  EXPECT_EQ(test_object2->value_, 12);

  test_object1->value_ = 10;
  EXPECT_EQ(test_object1->value_, 10);
  EXPECT_EQ(test_object2->value_, 10);

  {
    RefCounted<TypeParam> test_object3;
    EXPECT_EQ(test_object3.UseCount(), 0);
    test_object3 = test_object2;

    EXPECT_EQ(test_object1.UseCount(), 3);
    EXPECT_EQ(test_object2.UseCount(), 3);
    EXPECT_EQ(test_object3.UseCount(), 3);

    EXPECT_EQ(test_object1->value_, 10);
    EXPECT_EQ(test_object2->value_, 10);
    EXPECT_EQ(test_object3->value_, 10);
    test_object3->value_ = 5;
  }

  EXPECT_EQ(test_object1.UseCount(), 2);
  EXPECT_EQ(test_object2.UseCount(), 2);
  EXPECT_EQ(test_object1->value_, 5);
  EXPECT_EQ(test_object2->value_, 5);

  {
    RefCounted<TypeParam> test_object4(std::move(test_object1));
    EXPECT_EQ(test_object2.UseCount(), 2);
    EXPECT_EQ(test_object4.UseCount(), 2);

    EXPECT_EQ(test_object2->value_, 5);
    EXPECT_EQ(test_object4->value_, 5);
    test_object4->value_ = 7;
  }

  EXPECT_EQ(test_object2.UseCount(), 1);
  EXPECT_EQ(test_object2->value_, 7);

  EXPECT_EQ(test_object1.UseCount(), 0);

  {
    RefCounted<TypeParam> test_object5;
    EXPECT_EQ(test_object5.UseCount(), 0);
    test_object5 = std::move(test_object2);

    EXPECT_EQ(test_object2.UseCount(), 0);
    EXPECT_EQ(test_object5.UseCount(), 1);
    EXPECT_EQ(test_object5->value_, 7);
  }

  EXPECT_EQ(test_object1.UseCount(), 0);
  EXPECT_EQ(test_object2.UseCount(), 0);
  EXPECT_EQ(destructor_call_count, 1u);
}

TYPED_TEST(RefCountedTest, InheritanceTest) {
  std::size_t destructor_count = 0;
  RefCounted<TypeParam> derived_object(new TypeParam(1, &destructor_count));
  RefCounted<Base> base_ptr(derived_object);
  EXPECT_EQ(derived_object.UseCount(), 2);
  EXPECT_EQ(base_ptr.UseCount(), 2);

  {
    RefCounted<Base> move_constructed_ptr(std::move(derived_object));
    EXPECT_EQ(derived_object.UseCount(), 0);
    EXPECT_EQ(base_ptr.UseCount(), 2);
    EXPECT_EQ(move_constructed_ptr.UseCount(), 2);
  }
  EXPECT_EQ(base_ptr.UseCount(), 1);

  RefCounted<Base> copy_assigned_ptr;
  EXPECT_EQ(copy_assigned_ptr.UseCount(), 0);
  copy_assigned_ptr = base_ptr;
  EXPECT_EQ(base_ptr.UseCount(), 2);
  EXPECT_EQ(copy_assigned_ptr.UseCount(), 2);

  {
    RefCounted<Base> move_assigned_ptr;
    EXPECT_EQ(move_assigned_ptr.UseCount(), 0);
    move_assigned_ptr = std::move(base_ptr);
    EXPECT_EQ(base_ptr.UseCount(), 0);
    EXPECT_EQ(copy_assigned_ptr.UseCount(), 2);
    EXPECT_EQ(move_assigned_ptr.UseCount(), 2);
  }
  EXPECT_EQ(copy_assigned_ptr.UseCount(), 1);
}

TYPED_TEST(RefCountedTest, EndToEndTest) {
  RefCounted<TypeParam> obj(new TypeParam(1));
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 0u);

  WeakRefCounted<TypeParam> weak_ref = obj.GetWeakRef();
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);
  {
    RefCounted<TypeParam> obj_copy(weak_ref);
    EXPECT_EQ(obj_copy.UseCount(), 2u);
    EXPECT_EQ(obj_copy.WeakCount(), 1u);
    EXPECT_EQ(obj.UseCount(), 2u);
    EXPECT_EQ(obj.WeakCount(), 1u);
    EXPECT_EQ(weak_ref.UseCount(), 2u);
    EXPECT_EQ(weak_ref.WeakCount(), 1u);
  }
  EXPECT_EQ(obj.UseCount(), 1u);
  EXPECT_EQ(obj.WeakCount(), 1u);
  EXPECT_EQ(weak_ref.UseCount(), 1u);
  EXPECT_EQ(weak_ref.WeakCount(), 1u);

  {
    RefCounted<TypeParam> moved_obj(std::move(obj));
    EXPECT_EQ(moved_obj.UseCount(), 1u);
    EXPECT_EQ(moved_obj.WeakCount(), 1u);
    EXPECT_EQ(obj.UseCount(), 0u);
    EXPECT_EQ(obj.WeakCount(), 0u);
    EXPECT_EQ(weak_ref.UseCount(), 1u);
    EXPECT_EQ(weak_ref.WeakCount(), 1u);
  }
  EXPECT_EQ(obj.UseCount(), 0u);
  EXPECT_EQ(obj.WeakCount(), 0u);
  EXPECT_EQ(weak_ref.UseCount(), 0u);
  EXPECT_EQ(weak_ref.WeakCount(), 1u);

  // Failed attempt at getting a RefCounted
  RefCounted<TypeParam> failed_obj(weak_ref);
  EXPECT_EQ(obj.UseCount(), 0u);
  EXPECT_EQ(obj.WeakCount(), 0u);
  EXPECT_EQ(failed_obj.UseCount(), 0u);
  EXPECT_EQ(failed_obj.WeakCount(), 0u);
  EXPECT_EQ(weak_ref.UseCount(), 0u);
  EXPECT_EQ(weak_ref.WeakCount(), 1u);
}

}  // namespace common
