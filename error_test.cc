#include "common/error.h"

#include "gtest/gtest.h"

namespace common {

TEST(ErrorWithExplanationTest, ConstructDestructTest) {
  ErrorWithExplanation error{.error_code = Error::kUnavailable,
                             .explanation = "Something was not available"};
  EXPECT_EQ(error.Explain(), "Unavailable: Something was not available");
}

}  // namespace common
