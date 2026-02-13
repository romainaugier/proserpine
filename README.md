# proserpine

Single-header, dependencies-free, C++20, Vulkan companion for a nice and smooth experience without writing thousands of lines of code.

## Usage
```cpp
// Only if you want to include vulkan and don't have it include before
#define PROSERPINE_INCLUDE_VULKAN
#define PROSERPINE_IMPLEMENTATION
#include "proserpine.hpp"
```

## Logging

proserpine does a lot of logging and you can configure it easily.

It can be configured to use either fmt style arguments, or printf style arguments
```cpp
#define PROSERPINE_LOG_FORMAT_FMT
// or
#define PROSERPINE_LOG_FORMAT_PRINTF
```

If no functions are defined, no logging will be done. You are responsible for setting them (writing yet another logging system has no meaning here).
```cpp
// 5 logging functions are usable:
#define PROSERPINE_LOG_ERROR(fmt, ...) my_error_log(fmt, ##__VA_ARGS__)
#define PROSERPINE_LOG_WARN(fmt, ...) my_warn_log(fmt, ##__VA_ARGS__)
#define PROSERPINE_LOG_INFO(fmt, ...) my_info_log(fmt, ##__VA_ARGS__)
#define PROSERPINE_LOG_DEBUG(fmt, ...) my_debug_log(fmt, ##__VA_ARGS__)
#define PROSERPINE_LOG_TRACE(fmt, ...) my_trace_log(fmt, ##__VA_ARGS__)

// TRACE logging is very verbose, you can disable it using
#define PROSERPINE_LOG_NO_TRACE
```
