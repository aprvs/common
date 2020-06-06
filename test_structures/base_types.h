#ifndef COMMON_TEST_STRUCTURES_BASE_TYPES_H_
#define COMMON_TEST_STRUCTURES_BASE_TYPES_H_

#include <cstdint>

namespace common {
namespace test_structures {

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

}  // namespace test_structures
}  // namespace common

#endif  // COMMON_TEST_STRUCTURES_BASE_TYPES_H_
