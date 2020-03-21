#include "common/error.h"

namespace common {

const char* ErrorString(Error error) {
  switch (error) {
    case Error::kNotFound:
      return "Not found";
    case Error::kUnavailable:
      return "Unavailable";
    case Error::kOutOfRange:
      return "Out of range";
    case Error::kInvalidArgument:
      return "Invalid Argument";
    case Error::kInternal:
      return "Internal";
      // no default. Let -werror=switch catch missing enum cases
  }
  return "Unknown";
}

ErrorWithExplanation::ErrorWithExplanation(Error error_code,
                                           const std::string& explanation)
    : error_code_(error_code), explanation_(explanation) {}

std::string ErrorWithExplanation::Explain() const {
  return std::string(ErrorString(error_code_)) + ": " + explanation_;
}

}  // namespace common