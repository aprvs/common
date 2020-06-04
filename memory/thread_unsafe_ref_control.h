#ifndef COMMON_MEMORY_REF_COUNT_CONTROL_H_
#define COMMON_MEMORY_REF_COUNT_CONTROL_H_

#include <cstdint>
#include <limits>

namespace common {

/// @class ThreadUnsafeRefControl
/// A thread unsafe control block for reference counting
class ThreadUnsafeRefControl {
 public:
  ThreadUnsafeRefControl(std::size_t use_count)
      : use_count_(use_count), weak_count_(use_count_ != 0) {}

  // ThreadUnsafeRefControl is not copy / move constructible / assignable
  ThreadUnsafeRefControl(const ThreadUnsafeRefControl&) = delete;
  ThreadUnsafeRefControl& operator=(const ThreadUnsafeRefControl) = delete;

  /// @return  current reference / use count
  std::size_t UseCount() const { return use_count_; }

  /// @return  current count of weak references
  std::size_t WeakCount() const { return weak_count_; }

  /// Increases the reference / use count
  /// @return  the updated reference / use count
  // Note: in case of an overfow the reference count will not be updated
  std::size_t IncrementUseCount() {
    return (use_count_ < kCountMax ? ++use_count_ : use_count_);
  }

  /// Decreases the reference / use count
  /// @return  the updated reference / use count
  // Note: in case of an underflow the reference count will not be updated
  std::size_t DecrementUseCount() {
    return (use_count_ > 0 ? --use_count_ : use_count_);
  }

  /// Increases the weak reference count
  /// @return  updated value fo the weak reference count
  std::size_t IncrementWeakCount() {
    return (weak_count_ < kCountMax ? ++weak_count_ : weak_count_);
  }

  /// Decreases the weak reference count
  /// @return  updated value fo the weak reference count
  std::size_t DecrementWeakCount() {
    return (weak_count_ > 0 ? --weak_count_ : weak_count_);
  }

  /// Sets the value of the reference / use count
  void SetUseCount(std::size_t use_count) { use_count_ = use_count; }

  /// Sets the value fo the weak reference count
  void SetWeakCount(std::size_t weak_count) { weak_count_ = weak_count; }

 private:
  constexpr static std::size_t kCountMax =
      std::numeric_limits<std::size_t>::max();
  std::size_t use_count_;
  std::size_t weak_count_;
};

}  // namespace common

#endif  // COMMON_MEMORY_REF_COUNT_CONTROL_H_
