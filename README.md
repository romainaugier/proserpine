# proserpine

Single-header, dependencies-free, C++20, Vulkan companion for a nice and smooth experience without writing thousands of lines of code.

The library is still early, in progress, and will have a lot more to offer. If you feel like there is something missing, don't hesitate to open an issue or submit a pull request with your feature!

## Usage
```cpp
#define PROSERPINE_IMPLEMENTATION
#include "proserpine.hpp"
```

## Design Philosophy

proserpine aims to be as easy to use as possible, while providing enough control so that you can tune Vulkan as you want, and still get maximum performance.
We also want to write code that is as concise, easy-to-read and short as possible. Thus, if you think that there is bloat somewhere, that could be deleted, without changing the library behavior, feel free to remove it and submit a pull request! As they say, the less code there is, the less code there is to debug.

## Configuration

proserpine provides macros that you can define to tune the library behavior.

### Includes

```cpp
// Only if you want to include vulkan and don't have it included before
#define PROSERPINE_INCLUDE_VULKAN
``` 

### Logging

proserpine does a lot of logging and you can configure it easily.

It can be configured to use either fmt style arguments, or printf style arguments
```cpp
#define PROSERPINE_LOG_FORMAT_FMT // "{}"
// or
#define PROSERPINE_LOG_FORMAT_PRINTF // "%s, %f, %d ..."
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
