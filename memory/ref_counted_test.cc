#include "common/memory/ref_counted.h"

#include <cstdint>
#include <iostream>
#include <utility>

#include "gtest/gtest.h"

namespace common {

namespace {

struct Base {
  virtual ~Base() = default;
};

struct NonCopyMovable : public Base {
  NonCopyMovable(std::size_t value, std::size_t* destructor_count = nullptr)
      : value_(value), destructor_count_(destructor_count) {
    if (destructor_count_ != nullptr) {
      *destructor_count_ = 0;
    }
  }

  ~NonCopyMovable() override {
    if (destructor_count_ != nullptr) {
      ++(*destructor_count_);
    }
  }

  NonCopyMovable(const NonCopyMovable&) = delete;
  NonCopyMovable& operator=(const NonCopyMovable&) = delete;

  std::size_t value_;
  std::size_t* const destructor_count_;
};

struct Movable : public Base {
  Movable(std::size_t value, std::size_t* destructor_count = nullptr)
      : value_(value), destructor_count_(destructor_count) {
    if (destructor_count_ != nullptr) {
      *destructor_count_ = 0;
    }
  }

  ~Movable() override {
    if (destructor_count_ != nullptr) {
      ++(*destructor_count_);
    }
  }

  Movable(const Movable&) = delete;
  Movable(Movable&&) = default;

  Movable& operator=(const Movable&) = delete;
  Movable& operator=(Movable&&) = default;

  std::size_t value_;
  std::size_t* const destructor_count_;
};

struct Copyable : public Base {
  Copyable(std::size_t value, std::size_t* destructor_count = nullptr)
      : value_(value), destructor_count_(destructor_count) {
    if (destructor_count_ != nullptr) {
      *destructor_count_ = 0;
    }
  }

  ~Copyable() override {
    if (destructor_count_ != nullptr) {
      ++(*destructor_count_);
    }
  }

  Copyable(const Copyable&) = default;
  Copyable(Copyable&&) = delete;

  Copyable& operator=(const Copyable&) = default;
  Copyable& operator=(Copyable&&) = delete;

  std::size_t value_;
  std::size_t* const destructor_count_;
};

struct CopyMovable : public Base {
  CopyMovable(std::size_t value, std::size_t* destructor_count = nullptr)
      : value_(value), destructor_count_(destructor_count) {
    if (destructor_count_ != nullptr) {
      *destructor_count_ = 0;
    }
  }

  ~CopyMovable() override {
    if (destructor_count_ != nullptr) {
      ++(*destructor_count_);
    }
  }

  std::size_t value_;
  std::size_t* const destructor_count_;
};

}  // namespace

template <typename T>
struct RefCountedTest : public testing::Test {
  using ParamType = T;
};

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

}  // namespace common
