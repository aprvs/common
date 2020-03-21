#ifndef COMMON_ERROR_H_
#define COMMON_ERROR_H_

#include <string>

namespace common {

enum class Error {
  kNotFound,
  kUnavailable,
  kOutOfRange,
  kInvalidArgument,
  kInternal,
};

const char* ErrorString(Error error);

class ErrorWithExplanation {
 public:
  ErrorWithExplanation(Error error_code, const std::string& explanation);

  Error error_code() const { return error_code_; }

  std::string Explain() const;

 private:
  Error error_code_;
  std::string explanation_;
};

}  // namespace common

#endif  // COMMON_ERROR_H_
