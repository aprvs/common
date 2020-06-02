#ifndef COMMON_ERROR_OR_H_
#define COMMON_ERROR_OR_H_

#include <cstdlib>
#include <utility>

#include "common/error.h"
#include "common/memory/placement_new.h"

namespace common {

template <typename T, typename E>
class ErrorOrTemplate {
 public:
  ErrorOrTemplate(const T& success) : case_(kSuccess), success_(success) {}
  ErrorOrTemplate(T&& success)
      : case_(kSuccess), success_(std::move(success)) {}

  ErrorOrTemplate(const E& failure) : case_(kFailure), failure_(failure) {}
  ErrorOrTemplate(E&& failure)
      : case_(kFailure), failure_(std::move(failure)) {}

  ErrorOrTemplate(const ErrorOrTemplate& other) : case_(other.case_) {
    switch (case_) {
      case kFailure:
        Construct<E>(&failure_, std::forward<const E&>(other.ErrorOrDie()));
        return;
      case kSuccess:
        Construct<T>(&success_, std::forward<const T&>(other.ValueOrDie()));
        return;
        // no default. Let -werror=switch catch missing enum cases
    }
  }

  ErrorOrTemplate(ErrorOrTemplate&& other) : case_(other.case_) {
    switch (case_) {
      case kFailure:
        Construct<E>(&failure_, std::forward<E&&>(other.MoveErrorOrDie()));
        return;
      case kSuccess:
        Construct<T>(&success_, std::forward<T&&>(other.MoveValueOrDie()));
        return;
        // no default. Let -werror=switch catch missing enum cases
    }
  }

  ~ErrorOrTemplate() { Destroy(); }

  ErrorOrTemplate& operator=(const ErrorOrTemplate& other) {
    bool needs_construction = case_ != other.case_;
    if (needs_construction) {
      Destroy();
    }
    case_ = other.case_;
    switch (case_) {
      case kFailure:
        if (needs_construction) {
          Construct<E>(&failure_, std::forward<const E&>(other.ErrorOrDie()));
        } else {
          failure_ = other.failure_;
        }
        break;
      case kSuccess:
        if (needs_construction) {
          Construct<T>(&success_, std::forward<const T&>(other.ValueOrDie()));
        } else {
          success_ = other.success_;
        }
        break;
        // no default. Let -werror=switch catch missing enum cases
    }
    return *this;
  }

  ErrorOrTemplate& operator=(ErrorOrTemplate&& other) {
    bool needs_construction = case_ != other.case_;
    if (needs_construction) {
      Destroy();
    }
    case_ = other.case_;
    switch (case_) {
      case kFailure:
        if (needs_construction) {
          Construct<E>(&failure_, std::forward<E&&>(other.MoveErrorOrDie()));
        } else {
          failure_ = other.MoveErrorOrDie();
        }
        break;
      case kSuccess:
        if (needs_construction) {
          Construct<T>(&success_, std::forward<T&&>(other.MoveValueOrDie()));
        } else {
          success_ = other.MoveValueOrDie();
        }
        break;
        // no default. Let -werror=switch catch missing enum cases
    }
    return *this;
  }

  bool HasError() const { return case_ == kFailure; }

  const E& ErrorOrDie() const {
    if (!HasError()) {
      std::exit(EXIT_FAILURE);
    }
    return failure_;
  }

  E&& MoveErrorOrDie() {
    if (!HasError()) {
      std::exit(EXIT_FAILURE);
    }
    return std::move(failure_);
  }

  bool HasValue() const { return case_ == kSuccess; }

  const T& ValueOrDie() const {
    if (!HasValue()) {
      std::exit(EXIT_FAILURE);
    }
    return success_;
  }

  T&& MoveValueOrDie() {
    if (!HasValue()) {
      std::exit(EXIT_FAILURE);
    }
    return std::move(success_);
  }

 private:
  void Destroy() {
    switch (case_) {
      case kFailure:
        failure_.~E();
        return;
      case kSuccess:
        success_.~T();
        return;
        // no default. Let -werror=switch catch missing enum cases
    }
  }

  enum { kFailure, kSuccess } case_;
  union {
    E failure_;
    T success_;
  };
};

template <typename T>
using ErrorOr = ErrorOrTemplate<T, Error>;

template <typename T>
using ExplainedErrorOr = ErrorOrTemplate<T, ErrorWithExplanation>;

}  // namespace common

#endif  // COMMON_ERROR_OR_H_
