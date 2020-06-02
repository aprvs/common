#include "common/memory/placement_new.h"

#include "gtest/gtest.h"

namespace common {
namespace {

class TestClass {
 public:
  explicit TestClass(int val, bool* destructor)
      : val_(val), destructor_(destructor) {
    *destructor_ = false;
  }

  ~TestClass() { *destructor_ = true; }

  TestClass(const TestClass&) = delete;
  TestClass& operator=(const TestClass&) = delete;

  int val() const { return val_; }

 private:
  int val_;
  bool* destructor_;
};

}  // namespace

TEST(PlacementNewTest, ConstructionTest) {
  bool destructor_called = false;
  char allocated_memory[128];
  {
    TestClass* test1 = Construct<TestClass>(
        static_cast<void*>(allocated_memory), 10, &destructor_called);
    EXPECT_EQ(test1->val(), 10);
    test1->~TestClass();
  }
  EXPECT_TRUE(destructor_called);
}

}  // namespace common
