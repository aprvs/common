#ifndef COMMON_MEMORY_WEAK_REF_COUNTED_H_
#define COMMON_MEMORY_WEAK_REF_COUNTED_H_

#include <type_traits>

#include "common/memory/thread_unsafe_ref_control.h"

namespace common {

template <typename T, typename Control = ThreadUnsafeRefControl>
class WeakRefCounted {
 public:
  WeakRefCounted() : WeakRefCounted(nullptr, nullptr) {}

  template <typename U,
            typename = std::enable_if<std::is_convertible<U*, T*>::value>>
  WeakRefCounted(const WeakRefCounted<U, Control>& other)
      : WeakRefCounted(other.control_ptr_, other.managed_) {
    if (this->control_ptr_ != nullptr) {
      this->control_ptr_->IncrementWeakCount();
    }
  }

  template <typename U,
            typename = std::enable_if<std::is_convertible<U*, T*>::value>>
  WeakRefCounted(WeakRefCounted<U, Control>&& other)
      : WeakRefCounted(other.control_ptr_, other.managed_) {
    other.control_ptr_ = nullptr;
    other.managed_ = nullptr;
  }

  template <typename U,
            typename = std::enable_if<std::is_convertible<U*, T*>::value>>
  WeakRefCounted& operator=(const WeakRefCounted<U, Control>& other) {
    if (this->control_ptr_ != nullptr) {
      if (this->control_ptr_->WeakCount() == 1) {
        DestroyControl();
      } else {
        this->control_ptr_->DecrementWeakCount();
      }
    }
    this->control_ptr_ = other->control_ptr_;
    this->managed_ = other->managed_;
    if (this->control_ptr_ != nullptr) {
      this->control_ptr_->IncrementWeakCount();
    }
  }

  template <typename U,
            typename = std::enable_if<std::is_convertible<U*, T*>::value>>
  WeakRefCounted& operator=(WeakRefCounted<U, Control>&& other) {
    this->control_ptr_ = other->control_ptr_;
    this->managed_ = other->managed_;
    other.control_ptr_ = nullptr;
    other.managed_ = nullptr;
  }

  bool HasWeakRef() const {
    return (control_ptr_ == nullptr) || (control_ptr_->UseCount() == 0);
  }

 private:
  template <typename U, typename UCounter>
  friend class WeakRefCounted;

  template <typename U, typename UCounter>
  friend class RefCounted;

  WeakRefCounted(Control* control_ptr, T* to_manage)
      : control_ptr_(control_ptr), managed_(to_manage) {}

  void DestroyControl() { delete control_ptr_; }

  Control* control_ptr_;
  T* managed_;
};

}  // namespace common

#endif  // COMMON_MEMORY_WEAK_REF_COUNTED_H_
