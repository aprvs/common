#ifndef COMMON_MEMORY_REF_COUNTED_H_
#define COMMON_MEMORY_REF_COUNTED_H_

#include <type_traits>

#include "common/memory/thread_unsafe_ref_control.h"
#include "common/memory/weak_ref_counted.h"

namespace common {

/// @class RefCounted
/// RefCounted is reference counting based smart pointer class. Unlike
/// std::shared_ptr the control block is a template argument and can be changed
/// in order to get different characteristics like thread safety, counter
/// overflow / underflow handling and debug diagnostics
/// @tparam T  object type to be managed
/// @tparam Counter  reference counter object, default is a simple counter
template <typename T, typename Counter = ThreadUnsafeRefControl>
class RefCounted {
 public:
  RefCounted() : RefCounted(nullptr, nullptr) {}

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted(U* raw_ptr) : RefCounted(nullptr, raw_ptr) {}

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted(RefCounted<U, Counter>& other)
      : RefCounted(other.control_ptr_, other.managed_) {
    if (this->managed_ != nullptr) {
      if (this->control_ptr_ != nullptr) {
        this->control_ptr_->IncrementUseCount();
      } else {
        this->control_ptr_ = other.control_ptr_ = new Counter(2u);
      }
    }
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted(RefCounted<U, Counter>&& other)
      : RefCounted(other.control_ptr_, other.managed_) {
    other.control_ptr_ = nullptr;
    other.managed_ = nullptr;
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted(const WeakRefCounted<U, Counter>& other)
      : RefCounted(!other.HasWeakRef() ? nullptr : other.control_ptr_,
                   !other.HasWeakRef() ? nullptr : other.managed_) {
    if (this->managed_ != nullptr && this->control_ptr_ != nullptr) {
      this->control_ptr_->IncrementUseCount();
    }
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted(WeakRefCounted<U, Counter>&& other)
      : RefCounted(!other.HasWeakRef() ? nullptr : other.control_ptr_,
                   !other.HasWeakRef() ? nullptr : other.managed_) {}

  ~RefCounted() {
    DestroyManaged();
    if (control_ptr_ != nullptr) {
      if (control_ptr_->UseCount() < 2u && control_ptr_->WeakCount() < 1u) {
        delete control_ptr_;
      } else {
        control_ptr_->DecrementUseCount();
      }
    }
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted& operator=(RefCounted<U, Counter>& other) {
    DestroyManaged();
    if (other.control_ptr_ != nullptr) {
      DestroyControl();
      this->control_ptr_ = other.control_ptr_;
      this->control_ptr_->IncrementUseCount();
    } else if (this->control_ptr_ != nullptr) {
      this->control_ptr_->SetUseCount(2u);
      other.control_ptr_ = this->control_ptr_;
    }
    this->managed_ = other.managed_;
    return *this;
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted& operator=(RefCounted<U, Counter>&& other) {
    DestroyManaged();
    if (other.control_ptr_ != nullptr) {
      DestroyControl();
      this->control_ptr_ = other.control_ptr_;
    }
    this->managed_ = other.managed_;
    other.control_ptr_ = nullptr;
    other.managed_ = nullptr;
    return *this;
  }

  T const* operator->() const { return managed_; }
  T* operator->() { return managed_; }

  const T& operator*() const { return *managed_; }
  T& operator*() { return *managed_; }

  std::size_t UseCount() const {
    return (control_ptr_ == nullptr ? (managed_ == nullptr ? 0u : 1u)
                                    : control_ptr_->UseCount());
  }

  std::size_t WeakCount() const {
    return (control_ptr_ == nullptr ? 0u : control_ptr_->WeakCount());
  }

  WeakRefCounted<T, Counter> GetWeakRef() {
    if (this->control_ptr_ == nullptr) {
      this->control_ptr_ = new Counter(1u);
    }
    this->control_ptr_->IncrementWeakCount();
    return WeakRefCounted(this->control_ptr_, this->managed_);
  }

 private:
  template <typename U, typename UCounter>
  friend class RefCounted;

  RefCounted(Counter* control_ptr, T* to_manage)
      : control_ptr_(control_ptr), managed_(to_manage) {}

  void DestroyManaged() {
    if (managed_ != nullptr &&
        (control_ptr_ == nullptr || control_ptr_->UseCount() < 2u)) {
      delete managed_;
      managed_ = nullptr;
    }
  }

  void DestroyControl() {
    if (control_ptr_ != nullptr && control_ptr_->WeakCount() < 1u &&
        control_ptr_->UseCount() < 2u) {
      delete control_ptr_;
      control_ptr_ = nullptr;
    }
  }

  Counter* control_ptr_;
  T* managed_;
};

}  // namespace common

#endif  // COMMON_MEMORY_REF_COUNTED_H_
