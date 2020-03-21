#ifndef COMMON_PLACEMENT_NEW_H_
#define COMMON_PLACEMENT_NEW_H_

#include <utility>

namespace common {

/// Wrapper around placement new
template <typename T, typename... Args>
T* Construct(void* memory_location, Args... args) {
  return new (memory_location) T(std::forward<Args>(args)...);
}

}  // namespace common

#endif  // COMMON_PLACEMENT_NEW_H_
