#ifndef COMMON_MEMORY_REF_COUNTED_H_
#define COMMON_MEMORY_REF_COUNTED_H_

#include <cstdint>
#include <limits>
#include <type_traits>

namespace common {

namespace internal {

class Counter {
 public:
  Counter(std::size_t count) : count_(count) {}

  // Counter is not copy / move constructible / assignable
  Counter(const Counter&) = delete;
  Counter& operator=(const Counter) & = delete;

  std::size_t Value() const { return count_; }

  std::size_t Increment() { return (count_ < kCountMax ? ++count_ : count_); }

  std::size_t Decrement() { return (count_ > 0 ? --count_ : count_); }

  void Set(std::size_t count) { count_ = count; }

 private:
  constexpr static std::size_t kCountMax =
      std::numeric_limits<std::size_t>::max();
  std::size_t count_;
};

}  // namespace internal

/// @class RefCounted
/// RefCounted is reference counting based smart pointer class. The default
/// Counter is thread unsafe and uses a simple unsigned integer object as the
/// reference counter. Whereever thread safety is required, this can be changed
/// to an atomic integer object.
/// @tparam T  object type to be managed
/// @tparam Counter  reference counter object, default is a simple counter
// TODO(apoorv) handle counter overflow / underflow
template <typename T, typename Counter = internal::Counter>
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
    if (this->managed_ == nullptr) {
      if (this->control_ptr_ != nullptr) {
        this->control_ptr_->Set(0u);
      }
      return;
    }
    if (this->control_ptr_ == nullptr) {
      other.control_ptr_ = this->control_ptr_ = new Counter(2u);
    } else {
      this->control_ptr_->Increment();
    }
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted(RefCounted<U, Counter>&& other)
      : RefCounted(other.control_ptr_, other.managed_) {
    other.managed_ = nullptr;
    other.control_ptr_ = nullptr;
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted& operator=(RefCounted<U, Counter>& other) {
    if (control_ptr_ == nullptr || control_ptr_->Value() == 1u) {
      DestroyManaged();
    }
    this->managed_ = other.managed_;
    if (other.control_ptr_ == nullptr) {
      this->control_ptr_->Set(2u);
      other.control_ptr_ = this->control_ptr_;
      return *this;
    }
    DestroyControl();
    this->control_ptr_ = other.control_ptr_;
    this->control_ptr_->Increment();
    return *this;
  }

  template <typename U, typename = typename std::enable_if<
                            std::is_convertible<U*, T*>::value>::type>
  RefCounted& operator=(RefCounted<U, Counter>&& other) {
    if (this->control_ptr_ == nullptr || control_ptr_->Value() == 1u) {
      DestroyManaged();
    }
    this->managed_ = other.managed_;
    if (other.control_ptr_ != nullptr) {
      DestroyControl();
      this->control_ptr_ = other.control_ptr_;
    } else if (this->control_ptr_ != nullptr) {
      this->control_ptr_->Set(1u);
    }
    other.managed_ = nullptr;
    other.control_ptr_ = nullptr;
    return *this;
  }

  ~RefCounted() {
    if (control_ptr_ == nullptr || control_ptr_->Value() == 1u) {
      DestroyManaged();
      DestroyControl();
      return;
    }
    control_ptr_->Decrement();
  }

  T const* operator->() const { return managed_; }
  T* operator->() { return managed_; }

  const T& operator*() const { return *managed_; }
  T& operator*() { return *managed_; }

  std::size_t UseCount() const {
    return (control_ptr_ == nullptr ? (managed_ == nullptr ? 0u : 1u)
                                    : control_ptr_->Value());
  }

 private:
  template <typename U, typename UCounter>
  friend class RefCounted;

  RefCounted(Counter* control_ptr, T* to_manage)
      : control_ptr_(control_ptr), managed_(to_manage) {}

  void DestroyManaged() { delete managed_; }

  void DestroyControl() { delete control_ptr_; }

  Counter* control_ptr_;
  T* managed_;
};

}  // namespace common

#endif  // COMMON_MEMORY_REF_COUNTED_H_
