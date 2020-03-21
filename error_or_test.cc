#include "common/error_or.h"

#include <memory>

#include "gtest/gtest.h"

namespace common {

namespace {

struct TestStruct {
  int a;
};

struct NonCopyableStruct {
  NonCopyableStruct() = default;
  NonCopyableStruct(int value) : a(value) {}
  NonCopyableStruct(const NonCopyableStruct&) = delete;
  NonCopyableStruct& operator=(const NonCopyableStruct&) = delete;
  int a = 2;
};

}  // namespace

TEST(ErrorOrTest, ConstructDestructTest) {
  ErrorOr<TestStruct> has_success(TestStruct{.a = 145});
  EXPECT_FALSE(has_success.HasError());
  EXPECT_EQ(has_success.ValueOrDie().a, 145);
  EXPECT_EXIT(has_success.ErrorOrDie(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "");

  ErrorOr<TestStruct> has_failure(Error::kUnavailable);
  EXPECT_TRUE(has_failure.HasError());
  EXPECT_EQ(has_failure.ErrorOrDie(), Error::kUnavailable);
  EXPECT_EXIT(has_failure.ValueOrDie(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "");
}

TEST(ErrorOrTest, MoveOnlySuccessTest) {
  std::unique_ptr<NonCopyableStruct> moved_data =
      std::make_unique<NonCopyableStruct>();
  moved_data->a = 120;
  NonCopyableStruct* value = moved_data.get();
  ErrorOr<std::unique_ptr<NonCopyableStruct>> has_success(
      std::move(moved_data));
  EXPECT_FALSE(has_success.HasError());
  std::unique_ptr<NonCopyableStruct> got_value = has_success.MoveValueOrDie();
  EXPECT_EQ(got_value->a, 120);
  EXPECT_EXIT(has_success.MoveErrorOrDie(),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
  EXPECT_EQ(got_value.get(), value);
}

TEST(ErrorOrTest, CopyConstructionTest) {
  ErrorOr<TestStruct> has_success(TestStruct{.a = 145});
  ErrorOr<TestStruct> has_success_copy(has_success);
  EXPECT_FALSE(has_success_copy.HasError());
  EXPECT_EQ(has_success_copy.ValueOrDie().a, 145);
  EXPECT_EXIT(has_success_copy.ErrorOrDie(),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(ErrorOrTest, CopyAssignmentTest) {
  ErrorOr<TestStruct> has_success(TestStruct{.a = 145});
  ErrorOr<TestStruct> has_failure(Error::kNotFound);
  has_failure = has_success;
  EXPECT_TRUE(has_failure.HasValue());
  EXPECT_EQ(has_success.ValueOrDie().a, 145);
  EXPECT_EXIT(has_success.ErrorOrDie(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "");
}

TEST(ErrorOrTest, MoveConstructionTest) {
  ErrorOr<std::unique_ptr<NonCopyableStruct>> has_success(
      std::make_unique<NonCopyableStruct>(120));
  EXPECT_FALSE(has_success.HasError());
  EXPECT_TRUE(has_success.HasValue());
  ErrorOr<std::unique_ptr<NonCopyableStruct>> has_success_copy(
      std::move(has_success));
  EXPECT_FALSE(has_success_copy.HasError());
  EXPECT_TRUE(has_success_copy.HasValue());
  std::unique_ptr<NonCopyableStruct> success =
      has_success_copy.MoveValueOrDie();
  EXPECT_EQ(success->a, 120);
}

TEST(ErrorOrTest, MoveAssignmentTest) {
  ErrorOr<std::unique_ptr<NonCopyableStruct>> has_failure(Error::kNotFound);
  ErrorOr<std::unique_ptr<NonCopyableStruct>> has_success(
      std::make_unique<NonCopyableStruct>(111));
  has_failure = std::move(has_success);
  EXPECT_TRUE(has_failure.HasValue());
  std::unique_ptr<NonCopyableStruct> value = has_failure.MoveValueOrDie();
  EXPECT_EQ(value->a, 111);
}

}  // namespace common
