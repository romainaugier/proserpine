// =============================================================================
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026 - Present Romain Augier MIT License
// All rights reserved
//
// proserpine.hpp - Header-only Vulkan utility library
//
// Usage (in only one translation unit):
//     /* only if you want to include vulkan and don't have it include before */
//     #define PROSERPINE_INCLUDE_VULKAN
//     #define PROSERPINE_IMPLEMENTATION
//     #include "proserpine.hpp"
//
// =============================================================================

#pragma once

#if !defined(__PROSERPINE)
#define __PROSERPINE

// =============================================================================
// Logging
// =============================================================================

// It can be configured to use either fmt style arguments, or printf style arguments
// #define PROSERPINE_LOG_FORMAT_FMT
// #define PROSERPINE_LOG_FORMAT_PRINTF
//
// 5 logging functions are usable:
//     PROSERPINE_LOG_ERROR(fmt, ...)
//     PROSERPINE_LOG_WARN(fmt, ...)
//     PROSERPINE_LOG_INFO(fmt, ...)
//     PROSERPINE_LOG_DEBUG(fmt, ...)
//     PROSERPINE_LOG_TRACE(fmt, ...)
// If not defined, no logging will be done. You are responsible for setting them
// (writing yet another logging system has no meaning here)
// TRC logging is very verbose, you can disable it using
// #define PROSERPINE_LOG_NO_TRACE

#if defined(PROSERPINE_LOG_FORMAT_FMT)
#define __FMT_STR "{}"
#define __FMT_WSTR "{}"
#define __FMT_U32 "{}"
#define __FMT_I32 "{}"
#define __FMT_U64 "{}"
#define __FMT_U64H "{:016x}"
#define __FMT_I64 "{}"
#define __FMT_BOOL "{}"
#define __FMT_FLT32 "{}"
#define __FMT_FLT64 "{}"
#elif defined(PROSERPINE_LOG_FORMAT_PRINTF)
#define __FMT_STR "%s"
#define __FMT_WSTR "%ls"
#define __FMT_U32 "%d"
#define __FMT_I32 "%i"
#define __FMT_U64 "%zu"
#define __FMT_U64H "0x%016zx"
#define __FMT_I64 "%zi"
#define __FMT_BOOL "%d"
#define __FMT_FLT32 "%f"
#define __FMT_FLT64 "%f"
#else
#define __FMT_STR ""
#define __FMT_WSTR ""
#define __FMT_U32 ""
#define __FMT_I32 ""
#define __FMT_U64 ""
#define __FMT_U64H ""
#define __FMT_I64 ""
#define __FMT_BOOL ""
#define __FMT_FLT32 ""
#define __FMT_FLT64 ""
#endif // defined(PROSERPINE_LOG_FORMAT_FMT)

#if !defined(PROSERPINE_LOG_ERROR)
#define PROSERPINE_LOG_ERROR(fmt, ...)
#endif // !defined(PROSERPINE_LOG_ERROR)

#if !defined(PROSERPINE_LOG_WARN)
#define PROSERPINE_LOG_WARN(fmt, ...)
#endif // !defined(PROSERPINE_LOG_WARN)

#if !defined(PROSERPINE_LOG_INFO)
#define PROSERPINE_LOG_INFO(fmt, ...)
#endif // !defined(PROSERPINE_LOG_INFO)

#if !defined(PROSERPINE_LOG_DEBUG)
#define PROSERPINE_LOG_DEBUG(fmt, ...)
#endif // !defined(PROSERPINE_LOG_DEBUG)

#if !defined(PROSERPINE_LOG_TRACE)
#define PROSERPINE_LOG_TRACE(fmt, ...)
#endif // !defined(PROSERPINE_LOG_TRACE)

#if defined(PROSERPINE_LOG_NO_TRACE)
#define PROSERPINE_LOG_TRACE(fmt, ...)
#endif // defined(PROSERPINE_LOG_NO_TRACE)

#define __LOG_ERROR(fmt, ...) PROSERPINE_LOG_ERROR(fmt, ##__VA_ARGS__)
#define __LOG_WARN(fmt, ...) PROSERPINE_LOG_WARN(fmt, ##__VA_ARGS__)
#define __LOG_INFO(fmt, ...) PROSERPINE_LOG_INFO(fmt, ##__VA_ARGS__)
#define __LOG_DEBUG(fmt, ...) PROSERPINE_LOG_DEBUG(fmt, ##__VA_ARGS__)
#define __LOG_TRACE(fmt, ...) PROSERPINE_LOG_TRACE(fmt, ##__VA_ARGS__)

// =============================================================================
//  Utilities
// =============================================================================

// In debug mode, assertions will abort, and in Release only return an error value
// depending on the calling context. It makes error checking easier
#if defined(NDEBUG)
#define PROSERPINE_ASSERT(expr, msg, retvalue)                  \
    if(!(expr)) {                                               \
        std::fprintf(stderr,                                    \
                    "Check failed in file %s at line %d: %s\n", \
                    __FILE__,                                   \
                    __LINE__,                                   \
                    msg);                                       \
        return retvalue; }
#else
#define PROSERPINE_ASSERT(expr, msg, retvalue)                      \
    if(!(expr)) {                                                   \
        std::fprintf(stderr,                                        \
                    "Assertion failed in file %s at line %d: %s\n", \
                    __FILE__,                                       \
                    __LINE__,                                       \
                    msg);                                           \
        std::abort(); }
#endif // defined(NDEBUG)

#define PROSERPINE_VK_CHECK(expr, msg)     \
    do {                                   \
        VkResult __vk_res = (expr);        \
        if(__vk_res != VK_SUCCESS)         \
            return Error(__vk_res, (msg)); \
    } while (0)

#define PROSERPINE_VK_CHECK_VOID(expr, msg)                          \
    do {                                                             \
        VkResult __vk_res = (expr);                                  \
        if(__vk_res != VK_SUCCESS)                                   \
            __LOG_ERROR(__FMT_I32 ": " __FMT_STR "", __vk_res, msg); \
    } while (0)

#define PROSERPINE_WAIT_INFINITE std::numeric_limits<std::uint64_t>::max()
#define PROSERPINE_INVALID_QUEUE_INDEX std::numeric_limits<std::uint32_t>::max()

#define PROSERPINE_COPYABLE(__class__) __class__(const __class__& other); __class__& operator=(const __class__& other)
#define PROSERPINE_NON_COPYABLE(__class__) __class__(const __class__&) = delete; __class__& operator=(const __class__&) = delete
#define PROSERPINE_MOVABLE(__class__) __class__(__class__&& other) noexcept; __class__& operator=(__class__&& other) noexcept
#define PROSERPINE_NON_MOVABLE(__class__) __class__(__class__&&) = delete; __class__& operator=(__class__&&) = delete
#define PROSERPINE_COPYABLE_MOVABLE(__class__) PROSERPINE_COPYABLE(__class__); PROSERPINE_MOVABLE(__class__)
#define PROSERPINE_COPYABLE_NON_MOVABLE(__class__) PROSERPINE_COPYABLE(__class__); PROSERPINE_NON_MOVABLE(__class__)
#define PROSERPINE_NON_COPYABLE_MOVABLE(__class__) PROSERPINE_NON_COPYABLE(__class__); PROSERPINE_MOVABLE(__class__)
#define PROSERPINE_NON_COPYABLE_NON_MOVABLE(__class__) PROSERPINE_NON_COPYABLE(__class__); PROSERPINE_NON_MOVABLE(__class__)

// Just null
#define __DETAIL_NULL(m) other.m = VK_NULL_HANDLE;
// Shallow copy + VK_NULL_HANDLE
#define __DETAIL_MOVE_AND_NULL(m) this->m = other.m; other.m = VK_NULL_HANDLE;
// Shallow copy + 0
#define __DETAIL_MOVE_AND_ZERO(m) this->m = other.m; other.m = 0;
// std::move for non-trivial types
#define __DETAIL_MOVE(m) this->m = std::move(other.m);
// Plain copy
#define __DETAIL_COPY(m) this->m = other.m;
// memcpy array + zero source
#define __DETAIL_MEMCPY_AND_ZERO(m) std::memcpy(this->m, other.m, sizeof(m)); \
                                    std::memset(other.m, 0, sizeof(other.m));
// memcpy array (no zeroing)
#define __DETAIL_MEMCPY(m) std::memcpy(this->m, other.m, sizeof(m));

// FOR_EACH preprocessor engine (supports up to 32 args)
// Helping in move ctor/assignment when having a lot of variables

#define __PP_EXPAND(x) x

#define __PP_CAT(a, b) __PP_CAT_IMPL(a, b)
#define __PP_CAT_IMPL(a, b) a##b

#define ___PP_ARG_32(_,                                                          \
    _32,_31,_30,_29,_28,_27,_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14, \
    _13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,X_,...) X_

#define __PP_HAS_COMMA(...) __PP_EXPAND(___PP_ARG_32(__VA_ARGS__, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0))

#define ___PP_TRIGGER_PARENTHESIS_(...) ,
#define ___PP_PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define ___PP_IS_EMPTY_CASE_0001 ,
#define ___PP_IS_EMPTY(_0, _1, _2, _3) __PP_HAS_COMMA(___PP_PASTE5(___PP_IS_EMPTY_CASE_, _0, _1, _2, _3))

#define __PP_IS_EMPTY(...)                                           \
___PP_IS_EMPTY(                                                      \
__PP_HAS_COMMA(__VA_ARGS__),                                         \
__PP_HAS_COMMA(___PP_TRIGGER_PARENTHESIS_ __VA_ARGS__),              \
__PP_HAS_COMMA(__VA_ARGS__ (/* empty */)),                           \
__PP_HAS_COMMA(___PP_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/* empty */)) \
)

#define ___PP_VAR_COUNT_EMPTY_1(...) 0
#define ___PP_VAR_COUNT_EMPTY_0(...) __PP_EXPAND(___PP_ARG_32(__VA_ARGS__,      \
    32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7, \
    6,5,4,3,2,1))

#define __PP_NARG(...) __PP_CAT(___PP_VAR_COUNT_EMPTY_, __PP_IS_EMPTY(__VA_ARGS__))(__VA_ARGS__)

#define __PP_FOR_EACH(action, ...) \
    __PP_FOR_EACH_( __PP_NARG(__VA_ARGS__), action, __VA_ARGS__)

#define __PP_FOR_EACH_(N, action, ...) \
    __PP_FOR_EACH_IMPL(N, action, __VA_ARGS__)

#define __PP_FOR_EACH_IMPL(N, action, ...) \
    __PP_CAT(__PP_FOR_EACH_, N)(action, __VA_ARGS__)

#define __PP_FOR_EACH_1(action, a) action(a)
#define __PP_FOR_EACH_2(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_1(action, __VA_ARGS__))
#define __PP_FOR_EACH_3(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_2(action, __VA_ARGS__))
#define __PP_FOR_EACH_4(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_3(action, __VA_ARGS__))
#define __PP_FOR_EACH_5(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_4(action, __VA_ARGS__))
#define __PP_FOR_EACH_6(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_5(action, __VA_ARGS__))
#define __PP_FOR_EACH_7(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_6(action, __VA_ARGS__))
#define __PP_FOR_EACH_8(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_7(action, __VA_ARGS__))
#define __PP_FOR_EACH_9(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_8(action, __VA_ARGS__))
#define __PP_FOR_EACH_10(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_9(action, __VA_ARGS__))
#define __PP_FOR_EACH_11(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_10(action, __VA_ARGS__))
#define __PP_FOR_EACH_12(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_11(action, __VA_ARGS__))
#define __PP_FOR_EACH_13(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_12(action, __VA_ARGS__))
#define __PP_FOR_EACH_14(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_13(action, __VA_ARGS__))
#define __PP_FOR_EACH_15(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_14(action, __VA_ARGS__))
#define __PP_FOR_EACH_16(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_15(action, __VA_ARGS__))
#define __PP_FOR_EACH_17(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_16(action, __VA_ARGS__))
#define __PP_FOR_EACH_18(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_17(action, __VA_ARGS__))
#define __PP_FOR_EACH_19(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_18(action, __VA_ARGS__))
#define __PP_FOR_EACH_20(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_19(action, __VA_ARGS__))
#define __PP_FOR_EACH_21(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_20(action, __VA_ARGS__))
#define __PP_FOR_EACH_22(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_21(action, __VA_ARGS__))
#define __PP_FOR_EACH_23(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_22(action, __VA_ARGS__))
#define __PP_FOR_EACH_24(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_23(action, __VA_ARGS__))
#define __PP_FOR_EACH_25(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_24(action, __VA_ARGS__))
#define __PP_FOR_EACH_26(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_25(action, __VA_ARGS__))
#define __PP_FOR_EACH_27(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_26(action, __VA_ARGS__))
#define __PP_FOR_EACH_28(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_27(action, __VA_ARGS__))
#define __PP_FOR_EACH_29(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_28(action, __VA_ARGS__))
#define __PP_FOR_EACH_30(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_29(action, __VA_ARGS__))
#define __PP_FOR_EACH_31(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_30(action, __VA_ARGS__))
#define __PP_FOR_EACH_32(action, a, ...) action(a) __PP_EXPAND(__PP_FOR_EACH_31(action, __VA_ARGS__))

#define __NULL(...) __PP_FOR_EACH(__DETAIL_NULL, __VA_ARGS__)
#define __MOVE_AND_NULL(...) __PP_FOR_EACH(__DETAIL_MOVE_AND_NULL, __VA_ARGS__)
#define __MOVE_AND_ZERO(...) __PP_FOR_EACH(__DETAIL_MOVE_AND_ZERO, __VA_ARGS__)
#define __MOVE(...) __PP_FOR_EACH(__DETAIL_MOVE, __VA_ARGS__)
#define __COPY(...) __PP_FOR_EACH(__DETAIL_COPY, __VA_ARGS__)
#define __MEMCPY_AND_ZERO(...) __PP_FOR_EACH(__DETAIL_MEMCPY_AND_ZERO, __VA_ARGS__)
#define __MEMCPY(...) __PP_FOR_EACH(__DETAIL_MEMCPY, __VA_ARGS__)

// =============================================================================
//  Required headers
// =============================================================================

#if defined(PROSERPINE_INCLUDE_VULKAN)
#include <vulkan/vulkan.h>
#endif // defined(PROSERPINE_INCLUDE_VULKAN)

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <condition_variable>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <ranges>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

// =============================================================================
//  Platform detection
// =============================================================================

#if defined(_WIN32)
#define PROSERPINE_PLATFORM_WINDOWS
#elif defined(__linux__)
#define PROSERPINE_PLATFORM_LINUX
#elif defined(__APPLE__)
#define PROSERPINE_PLATFORM_APPLE
#else
#error "[proserpine] : Unsupported platform"
#endif // defined(_WIN32)

namespace proserpine {

// ============================================================================
//  Forward declarations
// ============================================================================

class VulkanContext;
class Buffer;
class Image;
class Fence;
class SwapChain;
class Semaphore;
class TimelineSemaphore;
class TimelineCallbackSystem;
class RenderDocIntegration;
class StagingBufferManager;
class DescriptorPool;
class DescriptorSet;
class CommandPool;
class CommandBuffer;
class CommandProfiler;
class ShaderModule;
class PipelineLayout;
class GraphicsPipeline;
class ComputePipeline;

// ============================================================================
//  Error handling: Expected<T>
// ============================================================================

struct Error
{
    VkResult result = VK_SUCCESS;
    std::string message;

    Error() = default;
    Error(VkResult r, std::string msg) : result(r), message(std::move(msg)) {}
    explicit Error(std::string msg) : result(VK_ERROR_UNKNOWN), message(std::move(msg)) {}
};

// Rust's Result-like struct to handle error easily
template <typename T>
class Expected
{
public:
    Expected(T value) : _data(std::move(value)) {}
    Expected(Error err) : _data(std::move(err)) {}

    explicit operator bool() const { return std::holds_alternative<T>(this->_data); }
    bool has_value() const { return std::holds_alternative<T>(this->_data); }

    bool has_error() const { return std::holds_alternative<Error>(this->_data); }

    inline T value() { return std::move(std::get<T>(this->_data)); }

    inline const T& value() const { return std::get<T>(this->_data); }

    inline T value_or(std::function<void(const Error&)> lbd)
    {
        if(this->has_error())
            lbd(this->error());

        return std::move(std::get<T>(this->_data));
    }

    const Error& error() const { return std::get<Error>(this->_data); }

private:
    std::variant<T, Error> _data;
};

// =============================================================================
//  Enums
// =============================================================================

enum class QueueType : std::uint8_t {
    Graphics,
    Compute,
    Transfer,
    SparseBinding,
    Present,
    Count,
};

enum class Feature : std::uint8_t {
    TimelineSemaphore,
    BufferDeviceAddress,
    DescriptorIndexing,
    Synchronization2,
    DynamicRendering,
    RayQuery,
    RayTracingPipeline,
    MeshShader,
    ShaderInt64,
    ShaderFloat64,
    Count,
};

// =============================================================================
//  Device properties
// =============================================================================

struct DeviceProperties
{
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceVulkan11Features features_11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
    VkPhysicalDeviceVulkan12Features features_12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    VkPhysicalDeviceVulkan13Features features_13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan14Features features_14{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };
    VkPhysicalDeviceMemoryProperties memory_properties{};
    std::vector<VkQueueFamilyProperties> queue_families;
    std::vector<VkExtensionProperties> available_extensions;
};

struct SelectedDevice
{
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    DeviceProperties props;
    std::uint32_t queue_family_indices[static_cast<std::size_t>(QueueType::Count)] = {};
    bool queue_family_valid[static_cast<std::size_t>(QueueType::Count)]   = {};
};

// ============================================================================
//  Device filtering
// ============================================================================

struct DeviceFilter
{
    std::uint32_t min_api_version = VK_API_VERSION_1_4;

    bool require_separate_compute_queue  = false;
    bool require_separate_transfer_queue = false;

    std::vector<const char*> required_extensions;

    // User-supplied scoring callback. Return < 0 to reject.
    std::function<std::int32_t(VkPhysicalDevice, const DeviceProperties&)> scorer;
};

// ============================================================================
//  Features requested
// ============================================================================

struct FeaturesRequested
{
    // Overriden if VK_KHR_SURFACE_EXTENSION_NAME in extra_instance_extensions
    // in VulkanContext::CreateInfo
    bool surface = false;

    bool timeline_semaphore = true;
    bool buffer_device_address  = true;
    bool descriptor_indexing = true;
    bool synchronization2 = true;
    bool dynamic_rendering = true;

    // Opt-in advanced features
    bool ray_query = false;
    bool ray_tracing_pipeline = false;
    bool mesh_shader = false;
    bool shader_int64 = false;
    bool shader_float64 = false;

    std::vector<const char*> extra_device_extensions;
};

// =============================================================================
//  SwapChain
// =============================================================================

class SwapChain
{
    friend class VulkanContext;

public:
    struct CreateInfo {
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        VkColorSpaceKHR colorspace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        std::int32_t present_mode = VK_PRESENT_MODE_FIFO_KHR;

        std::uint32_t extent_width = std::numeric_limits<std::int32_t>::max();
        std::uint32_t extent_height = std::numeric_limits<std::int32_t>::max();
    };

public:
    ~SwapChain();

    PROSERPINE_NON_COPYABLE_MOVABLE(SwapChain);

    VkResult acquire_next_image(VkSemaphore signal_semaphore,
                                VkFence signal_fence,
                                std::uint32_t* image_index)
    {
        VkAcquireNextImageInfoKHR info{VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR};
        info.pNext = nullptr;
        info.semaphore = signal_semaphore;
        info.fence = signal_fence;
        info.swapchain = this->_swapchain;
        info.timeout = PROSERPINE_WAIT_INFINITE;

        // Modify if multi-gpu usage, for now we just reference the first attached GPU
        info.deviceMask = 1;

        return vkAcquireNextImage2KHR(this->_device, &info, image_index);
    }

    inline VkImageView image_view(std::uint32_t index) { return this->_views[index]; }
    inline VkImage image(std::uint32_t index) { return this->_images[index]; }
    inline std::size_t image_count() const { return this->_images.size(); }

    inline bool image_first_use(std::uint32_t index) const { return this->_first_use[index]; }
    inline void image_first_use_set(std::uint32_t index) { this->_first_use[index] = 0; }

    inline VkSemaphore image_rendered_semaphore(std::uint32_t index) const { return this->_image_rendered_semaphores[index]; }

    const VkExtent2D& extent() const { return this->_extent; }

    VkFormat format() const { return this->_format; }

    inline const VkSwapchainKHR& handle() const { return this->_swapchain; }

private:
    SwapChain(VkDevice device, VkSurfaceKHR surface) : _device(device),
                                                       _surface(surface)
    {
        this->_first_use.set();
    }

    VkDevice _device = VK_NULL_HANDLE;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;

    std::vector<VkImage> _images;
    std::vector<VkImageView> _views;
    std::bitset<32> _first_use;
    std::vector<VkSemaphore> _image_rendered_semaphores;

    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkExtent2D _extent;
};

// =============================================================================
//  Fence
// =============================================================================

class Fence
{
    friend class VulkanContext;

public:
    Fence() = default;
    ~Fence();

    PROSERPINE_NON_COPYABLE_MOVABLE(Fence);

    inline VkFence handle() const { return this->_fence; }

    inline VkResult wait(std::uint64_t timeout_ns = PROSERPINE_WAIT_INFINITE)
    {
        return vkWaitForFences(this->_device, 1, &this->_fence, VK_TRUE, timeout_ns);
    }

    inline void reset()
    {
        vkResetFences(this->_device, 1, &this->_fence);
    }

private:
    Fence(VkDevice device, bool signaled = false);

    VkDevice _device = VK_NULL_HANDLE;
    VkFence _fence = VK_NULL_HANDLE;
};

// =============================================================================
//  TimelineSemaphore
// =============================================================================

class TimelineSemaphore
{
public:
    TimelineSemaphore() = default;
    TimelineSemaphore(VkDevice device, std::uint64_t initial_value = 0);
    ~TimelineSemaphore();

    PROSERPINE_NON_COPYABLE_MOVABLE(TimelineSemaphore);

    inline VkSemaphore handle() const { return this->_sema; }
    inline operator VkSemaphore() const { return this->_sema; }

    void signal(std::uint64_t value);
    VkResult wait(std::uint64_t value,
                  std::uint64_t timeout_ns = PROSERPINE_WAIT_INFINITE);
    std::uint64_t counter() const;

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkSemaphore _sema = VK_NULL_HANDLE;
};

// =============================================================================
//  Semaphore
// =============================================================================

class Semaphore
{
    friend class VulkanContext;
public:
    Semaphore() = default;
    ~Semaphore();

    PROSERPINE_NON_COPYABLE_MOVABLE(Semaphore);

    inline VkSemaphore handle() const { return this->_sema; }

private:
    Semaphore(VkDevice device);

    VkDevice _device = VK_NULL_HANDLE;
    VkSemaphore _sema = VK_NULL_HANDLE;
};

// =============================================================================
//  TimelineCallbackSystem
// =============================================================================

class TimelineCallbackSystem
{
public:
    explicit TimelineCallbackSystem(VkDevice device);
    ~TimelineCallbackSystem();

    PROSERPINE_NON_COPYABLE(TimelineCallbackSystem);

    void enqueue(VkSemaphore semaphore,
                 std::uint64_t wait_value,
                 std::function<void()> callback);
    void cancel(VkSemaphore semaphore);
    void shutdown();

private:
    struct PendingCallback
    {
        VkSemaphore semaphore;
        std::uint64_t wait_value;
        std::function<void()> callback;
    };

    void worker_loop();

    VkDevice _device = VK_NULL_HANDLE;
    std::vector<PendingCallback> _pending;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::thread _worker;
    std::atomic<bool> _running{false};
};

// =============================================================================
//  RenderDoc integration
// =============================================================================

class RenderDocIntegration
{
public:
    RenderDocIntegration();

    inline bool is_available() const noexcept { return this->_api != nullptr; }

    void start_capture();
    void end_capture();

private:
    void* _api  = nullptr; // RENDERDOC_API_1_6_0*
    void* _module = nullptr;
};

// =============================================================================
//  Buffer
// =============================================================================

class Buffer
{
public:
    struct CreateInfo
    {
        VkDeviceSize size = 0;
        VkBufferUsageFlags usage = 0;
        VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        const void* initial_data = nullptr; // if non-null, staged upload
    };

public:
    Buffer() = default;
    Buffer(VkDevice device,
           VkPhysicalDevice physical_device,
           const Buffer::CreateInfo& info);
    ~Buffer();

    PROSERPINE_NON_COPYABLE_MOVABLE(Buffer);

    inline VkBuffer handle() const { return this->_buffer; }
    inline VkDeviceMemory memory() const { return this->_memory; }
    inline VkDeviceSize size() const { return this->_size; }
    inline VkDeviceAddress device_address() const { return this->_device_address; }

    inline void* mapped_ptr() const { return this->_mapped; }

    inline operator VkBuffer() const { return this->_buffer; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkDeviceSize _size = 0;
    VkDeviceAddress _device_address = 0;
    void* _mapped = nullptr;

    friend class VulkanContext;
    friend class StagingBufferManager;
};

// =============================================================================
//  Image
// =============================================================================

class Image
{
public:
    struct CreateInfo
    {
        VkExtent3D extent = {1, 1, 1};
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        VkImageType type = VK_IMAGE_TYPE_2D;
        std::uint32_t mip_levels = 1;
        std::uint32_t array_layers = 1;
        VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
        VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    };

public:
    Image() = default;
    Image(VkDevice device,
          VkPhysicalDevice physical_device,
          const Image::CreateInfo& info);
    ~Image();

    PROSERPINE_NON_COPYABLE_MOVABLE(Image);

    inline VkImage handle() const { return this->_image; }
    inline VkImageView view() const { return this->_view; }
    inline VkDeviceMemory memory() const { return this->_memory; }
    inline VkFormat format() const { return this->_format; }
    inline VkExtent3D extent() const { return this->_extent; }

    inline operator VkImage() const { return this->_image; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkImage _image = VK_NULL_HANDLE;
    VkImageView _view = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkExtent3D _extent = {0, 0, 0};

    friend class VulkanContext;
    friend class StagingBufferManager;
};

// =============================================================================
//  StagingBufferManager
// =============================================================================

class StagingBufferManager
{
public:
    StagingBufferManager() = default;
    StagingBufferManager(VkDevice device,
                         VkPhysicalDevice physical_device,
                         VkQueue transfer_queue,
                         std::uint32_t transfer_family,
                         VkDeviceSize capacity = 64 * 1024 * 1024);
    ~StagingBufferManager();

    PROSERPINE_NON_COPYABLE_MOVABLE(StagingBufferManager);

    void upload_to_buffer(const void* data,
                          VkDeviceSize size,
                          VkBuffer dst,
                          VkDeviceSize dst_offset = 0);
    void upload_to_image(const void* data,
                         const VkExtent3D& extent,
                         VkFormat format,
                         VkImage dst,
                         VkImageSubresourceLayers layers = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1});
    void flush();

private:
    struct PendingCopy
    {
        VkDeviceSize offset;
        VkDeviceSize size;
        std::function<void(VkCommandBuffer)> record;
    };

    VkDevice _device = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkQueue _transfer_queue = VK_NULL_HANDLE;
    std::uint32_t _transfer_family = 0;
    VkBuffer _staging_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _staging_memory = VK_NULL_HANDLE;
    void* _staging_mapped = nullptr;
    VkDeviceSize _capacity = 0;
    VkDeviceSize _used = 0;
    VkCommandPool _command_pool = VK_NULL_HANDLE;
    VkFence _fence = VK_NULL_HANDLE;

    std::vector<PendingCopy> _pending;
};

// =============================================================================
//  Descriptor Pool
// =============================================================================

class DescriptorPool
{
public:
    static constexpr std::uint32_t DEFAULT_SETS_SIZE = 1024;

    struct CreateInfo
    {
        std::uint32_t max_sets = DEFAULT_SETS_SIZE;
        std::uint32_t pool_size_count = 1;

        // If sizes if left nullptr, it will automatically initialize to
        // { { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024 } }
        VkDescriptorPoolSize* sizes = nullptr;
    };

public:
    DescriptorPool();
    DescriptorPool(VkDevice device,
                   DescriptorPool::CreateInfo& create_info);

    ~DescriptorPool();

    PROSERPINE_NON_COPYABLE_MOVABLE(DescriptorPool);

    inline VkDescriptorPool pool() const { return this->_pool; }

    Expected<DescriptorSet> allocate_descriptor_set(VkDescriptorSetLayout layout);

private:
    VkDescriptorPool _pool = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> _sets;
};

// =============================================================================
//  Descriptor Set
// =============================================================================

class DescriptorSet
{
public:
    ~DescriptorSet();

    PROSERPINE_NON_COPYABLE_MOVABLE(DescriptorSet);

    inline const VkDescriptorSet& handle() const { return this->_set; }

    // TODO: make function per descriptor type instead of asking the user to
    // choose the type each time, cleaner
    DescriptorSet& write(std::uint32_t binding,
                         const Buffer& buffer,
                         VkDescriptorType type,
                         VkDeviceSize offset = 0,
                         VkDeviceSize range = VK_WHOLE_SIZE);

    DescriptorSet& write_storage_buffer(std::uint32_t binding,
                                        const Buffer& buffer,
                                        VkDeviceSize offset = 0,
                                        VkDeviceSize range = VK_WHOLE_SIZE)
    {
        return this->write(binding, buffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, offset, range);
    }

    DescriptorSet& update();

private:
    DescriptorSet(VkDevice device, VkDescriptorSet set) : _device(device),
                                                          _set(set) {}

    struct PendingWrite
    {
        std::uint32_t binding;
        VkDescriptorType type;
        std::size_t buffer_index;
    };

    VkDevice _device = VK_NULL_HANDLE;
    VkDescriptorSet _set = VK_NULL_HANDLE;

    std::vector<PendingWrite> _writes;
    std::vector<VkDescriptorBufferInfo> _buffer_infos;

    friend class VulkanContext;
    friend class DescriptorPool;
};

// =============================================================================
//  Command Buffer
// =============================================================================

class CommandBuffer
{
public:
    enum class Level
    {
        Primary,
        Secondary,
    };

    struct RenderingAttachment
    {
        VkImageView view;
        VkImageLayout layout;
        VkAttachmentLoadOp load_op;
        VkAttachmentStoreOp store_op;
        VkClearValue clear;
    };

    struct RenderingInfo
    {
        VkRect2D render_area;
        std::uint32_t layer_count = 1;

        std::vector<RenderingAttachment> color_attachments;
        std::optional<RenderingAttachment> depth_attachment;
    };

    CommandBuffer() = default;
    ~CommandBuffer();

    PROSERPINE_NON_COPYABLE_MOVABLE(CommandBuffer);

    void reset();

    void begin(VkCommandBufferUsageFlags flags = 0);
    void end();

    void transition_image(VkImage image,
                          VkImageLayout old_layout,
                          VkImageLayout new_layout,
                          VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

    void begin_render(const RenderingInfo& info);
    void end_render();

    void bind_graphics_pipeline(VkPipeline pipeline);
    void bind_compute_pipeline(VkPipeline pipeline);

    void set_viewport(float width,
                      float height,
                      float x = 0.0f,
                      float y = 0.0f,
                      float min_depth = 0.0f,
                      float max_depth = 1.0f);

    void set_scissor(std::uint32_t width,
                     std::uint32_t height,
                     std::int32_t offset_x = 0,
                     std::int32_t offset_y = 0);

    void draw(std::uint32_t vertex_count,
              std::uint32_t instance_count,
              std::uint32_t first_vertex,
              std::uint32_t first_instance);

    inline VkCommandBuffer handle() const { return this->_buffer; }

private:
    enum class State
    {
        Initial,
        Recording,
        Executable,
        Pending,
    };

    friend class CommandPool;
    friend class VulkanContext;

    CommandBuffer(VkDevice device, VkCommandPool pool, CommandBuffer::Level level);

    VkDevice _device = VK_NULL_HANDLE;
    VkCommandPool _pool = VK_NULL_HANDLE;
    VkCommandBuffer _buffer = VK_NULL_HANDLE;

    State _state;
};

// =============================================================================
//  Command Pool
// =============================================================================

class CommandPool
{
    friend class VulkanContext;

public:
    CommandPool() = default;
    ~CommandPool();

    PROSERPINE_NON_COPYABLE_MOVABLE(CommandPool);

    CommandBuffer allocate(CommandBuffer::Level level = CommandBuffer::Level::Primary);

    inline void reset()
    {
        vkResetCommandPool(this->_device, this->_pool, 0);
    }

private:
    CommandPool(VkDevice device, std::uint32_t queue_family_index);

    bool ready() const { return this->_pool != VK_NULL_HANDLE; }

    VkDevice _device = VK_NULL_HANDLE;
    VkCommandPool _pool = VK_NULL_HANDLE;
};

// =============================================================================
//  Command Profiler
// =============================================================================

class CommandProfiler
{
public:
    enum ElapsedUnit
    {
        Seconds,
        Milliseconds,
        Microseconds,
        Nanoseconds,
    };

    static constexpr double INVALID_MEASURE = std::numeric_limits<double>::max();
public:
    CommandProfiler(VkDevice device, VkPhysicalDevice physical_device);

    ~CommandProfiler();

    PROSERPINE_NON_COPYABLE_MOVABLE(CommandProfiler);

    void start(VkCommandBuffer cmd) noexcept;

    void end(VkCommandBuffer cmd) noexcept;

    double elapsed(ElapsedUnit unit = ElapsedUnit::Milliseconds) noexcept;

private:
    VkQueryPool _pool = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

    double _timestamp_period = 0.0;

    friend class VulkanContext;
};

// =============================================================================
//  Shader introspection types
// =============================================================================

struct DescriptorBindingInfo
{
    std::uint32_t binding = 0;
    VkDescriptorType descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    std::uint32_t descriptor_count = 1;
    VkShaderStageFlags stage_flags = 0;
    bool variable_count = false;
};

struct DescriptorSetLayoutInfo
{
    std::uint32_t set = 0;
    std::vector<DescriptorBindingInfo> bindings;
};

struct PushConstantRange
{
    VkShaderStageFlags stage_flags = 0;
    std::uint32_t offset = 0;
    std::uint32_t size = 0;
};

struct VertexInputAttribute
{
    std::uint32_t location = 0;
    std::uint32_t binding = 0;
    VkFormat format = VK_FORMAT_UNDEFINED;
    std::uint32_t offset = 0;
};

// =============================================================================
//  ShaderModule
// =============================================================================

using SPIRVByteCode = std::vector<std::uint32_t>;

Expected<SPIRVByteCode> load_spirv_file(const char* file_path) noexcept;

class ShaderModule
{
public:
    ShaderModule();
    ~ShaderModule();

    PROSERPINE_NON_COPYABLE_MOVABLE(ShaderModule);

    static Expected<ShaderModule> create_compute_shader(VkDevice device,
                                                        const char* path);

    static Expected<ShaderModule> create_vertex_shader(VkDevice device,
                                                       const char* path);

    static Expected<ShaderModule> create_fragment_shader(VkDevice device,
                                                         const char* path);

    inline VkShaderModule handle() const { return this->_module; }
    inline VkShaderStageFlagBits stage() const { return this->_stage; }
    inline operator VkShaderModule() const { return this->_module; }

    inline const std::vector<DescriptorSetLayoutInfo>& descriptor_set_layouts() const { return this->_set_layouts; }
    inline const std::vector<PushConstantRange>& push_constant_ranges() const { return this->_push_ranges; }
    inline const std::vector<VertexInputAttribute>& vertex_inputs() const { return this->_vertex_inputs; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkShaderModule _module = VK_NULL_HANDLE;
    VkShaderStageFlagBits _stage = VK_SHADER_STAGE_VERTEX_BIT;

    std::vector<DescriptorSetLayoutInfo> _set_layouts;
    std::vector<PushConstantRange> _push_ranges;
    std::vector<VertexInputAttribute> _vertex_inputs;

    void reflect_spirv(const std::vector<std::uint32_t>& spirv);

    static Expected<ShaderModule> create(VkDevice device,
                                         const std::vector<std::uint32_t>& spirv,
                                         VkShaderStageFlagBits stage);
};

// =============================================================================
//  PipelineLayout
// =============================================================================

class PipelineLayout
{
public:
    PipelineLayout();
    ~PipelineLayout();

    PROSERPINE_NON_COPYABLE_MOVABLE(PipelineLayout);

    inline VkPipelineLayout handle() const { return this->_layout; }
    inline operator VkPipelineLayout() const { return this->_layout; }

    inline const std::vector<VkDescriptorSetLayout>& set_layouts() const { return this->_set_layouts; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayout> _set_layouts;

    friend class PipelineLayoutBuilder;
};

// =============================================================================
//  PipelineLayoutBuilder
// =============================================================================

class PipelineLayoutBuilder
{
public:
    explicit PipelineLayoutBuilder(VkDevice device) : _device(device)
    {
        __LOG_TRACE("PipelineLayoutBuilder : Initializing PipelineLayoutBuilder");
    }

    PipelineLayoutBuilder& add_set(std::uint32_t set,
                                   const DescriptorSetLayoutInfo& info);
    PipelineLayoutBuilder& add_push_constant_range(VkShaderStageFlags stages,
                                                   std::uint32_t offset,
                                                   std::uint32_t size);
    Expected<PipelineLayout> build();

private:
    VkDevice _device;
    std::vector<std::pair<std::uint32_t, DescriptorSetLayoutInfo>> _sets;
    std::vector<VkPushConstantRange> _push_ranges;
};

// =============================================================================
//  Pipeline wrappers
// =============================================================================

struct ShaderStages
{
    std::vector<VkPipelineShaderStageCreateInfo> stages;

    ShaderStages& add(const ShaderModule& mod, const char* entry = "main");
};

struct VertexInputState
{
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
};

class GraphicsPipeline
{
public:
    // Info used to build the GraphicsPipeline.
    // "Reasonable" default values so you don't have to initialize everything
    struct CreateInfo
    {
        // Color format should match the one of the swapchain
        VkFormat color_format = VK_FORMAT_R8G8B8A8_SRGB;
        VkFormat depth_format = VK_FORMAT_D32_SFLOAT;

        // Input Assembly
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkBool32 primitive_restart_enable = VK_FALSE;

        // Viewport / scissor
        std::uint32_t viewport_count = 1;
        std::uint32_t scissor_count = 1;

        // Depth/stencil
        VkBool32 depth_test_enable = VK_TRUE;
        VkBool32 depth_write_enable = VK_TRUE;
        VkCompareOp depth_compare_op = VK_COMPARE_OP_LESS;

        // Color Blend
        VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT |
                                                 VK_COLOR_COMPONENT_G_BIT |
                                                 VK_COLOR_COMPONENT_B_BIT |
                                                 VK_COLOR_COMPONENT_A_BIT;

        // Rasterization
        VkPipelineRasterizationStateCreateInfo rasterization = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .lineWidth = 1.0f,
        };

        // Multisample
        VkPipelineMultisampleStateCreateInfo multisample = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        };
    };

public:
    GraphicsPipeline();
    ~GraphicsPipeline();

    PROSERPINE_NON_COPYABLE_MOVABLE(GraphicsPipeline);

    inline VkPipeline handle() const { return this->_pipeline; }
    inline operator VkPipeline() const { return this->_pipeline; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;

    friend Expected<GraphicsPipeline> create_graphics_pipeline(VkDevice,
                                                               const GraphicsPipeline::CreateInfo&,
                                                               const ShaderStages&,
                                                               const VertexInputState&,
                                                               VkPipelineLayout);
};

class ComputePipeline
{
public:
    ComputePipeline();
    ~ComputePipeline();

    PROSERPINE_NON_COPYABLE_MOVABLE(ComputePipeline);

    inline VkPipeline handle() const { return this->_pipeline; }
    inline operator VkPipeline() const { return this->_pipeline; }

private:
    VkDevice _device   = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;

    friend Expected<ComputePipeline> create_compute_pipeline(VkDevice device,
                                                             const ShaderModule& shader,
                                                             VkPipelineLayout layout,
                                                             const char* entry);
};

Expected<GraphicsPipeline> create_graphics_pipeline(VkDevice device,
                                                    const GraphicsPipeline::CreateInfo& info,
                                                    const ShaderStages& stages,
                                                    const VertexInputState& vertex_input,
                                                    VkPipelineLayout layout);

Expected<ComputePipeline> create_compute_pipeline(VkDevice device,
                                                  const ShaderModule& shader,
                                                  VkPipelineLayout layout,
                                                  const char* entry = "main");

// =============================================================================
//  Fence / Semaphore Pool
// =============================================================================

class FencePool
{
public:
    explicit FencePool(VkDevice device);
    ~FencePool();

    PROSERPINE_NON_COPYABLE_MOVABLE(FencePool);

    VkFence acquire(bool signaled = false);
    void release(VkFence fence);

private:
    VkDevice _device;
    std::vector<VkFence> _free;
    std::vector<VkFence> _all;
};

// =============================================================================
//  Context
// =============================================================================

class VulkanContext
{
public:
    static constexpr std::size_t NUM_QUEUES = static_cast<std::size_t>(QueueType::Count);

    struct CreateInfo {
        const char* application_name = "Vulkan App";
        const char* engine_name = "Proserpine";
        std::uint32_t application_version = VK_MAKE_VERSION(1, 0, 0);
        std::vector<const char*> extra_instance_extensions;
        std::vector<const char*> extra_instance_layers;
        bool enable_validation = false;
        DeviceFilter device_filter;
        FeaturesRequested features;
        DescriptorPool::CreateInfo descriptor_pool_info;
    };

    static Expected<VulkanContext> create(VulkanContext::CreateInfo& info,
                                          std::function<bool(VkInstance, VkSurfaceKHR*)> create_surface_callback = [](VkInstance, VkSurfaceKHR*) { return true; });

    VulkanContext() = default;
    ~VulkanContext();

    PROSERPINE_NON_COPYABLE_MOVABLE(VulkanContext);

    inline VkInstance instance() const { return this->_instance; }
    inline VkPhysicalDevice physical_device() const { return this->_physical_device; }
    inline VkDevice device() const { return this->_device; }
    inline VkSurfaceKHR surface() const { return this->_surface; }

    VkQueue queue(QueueType type, std::uint32_t index = 0) const;
    std::uint32_t queue_family(QueueType type) const;
    bool has_queue(QueueType type) const;

    bool is_extension_enabled(const char* name) const;
    bool is_feature_supported(Feature feature) const;
    inline const DeviceProperties& device_properties() const { return this->_device_properties; }

    StagingBufferManager& staging();
    TimelineCallbackSystem& timeline_callbacks();
    RenderDocIntegration& renderdoc();
    FencePool& fence_pool();
    DescriptorPool& descriptor_pool();
    // Returns a CommandPool reference owned by the context.
    CommandPool& command_pool(QueueType queue_type);
    CommandProfiler& command_profiler();

    Expected<Buffer> create_buffer(const Buffer::CreateInfo& info);
    Expected<Image> create_image(const Image::CreateInfo& info);
    Expected<Image> create_depth_image(VkExtent2D extent,
                                       VkFormat format = VK_FORMAT_D32_SFLOAT);
    Expected<TimelineSemaphore> create_timeline_semaphore(std::uint64_t initial_value = 0);
    Expected<Semaphore> create_semaphore();
    Expected<Fence> create_fence(bool signaled = false);

    // Returns a new CommandPool
    Expected<SwapChain> create_swapchain(SwapChain::CreateInfo& create_info);

    // Returns a new CommandPool
    Expected<CommandPool> create_command_pool(QueueType queue_type);

    void wait_idle();

    void immediate_submit(QueueType queue_type,
                          const std::function<void(VkCommandBuffer)>& record);

    struct SubmitInfo
    {
        std::vector<CommandBuffer*> command_buffers;
        std::vector<VkSemaphore> wait_semaphores;
        std::vector<VkSemaphore> finished_semaphores;

        VkPipelineStageFlags wait_stages;

        VkFence fence = VK_NULL_HANDLE;
    };

    void submit(QueueType queue_type, const SubmitInfo& info);

    struct PresentInfo
    {
        VkSwapchainKHR swapchain;

        std::vector<VkSemaphore> wait_semaphores;

        std::uint32_t image_index;
    };

    VkResult present(const PresentInfo& info);

    Expected<DescriptorSet> allocate_descriptor_set(VkDescriptorSetLayout layout);

private:
    VkInstance _instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    DeviceProperties _device_properties;
    SelectedDevice _selected_device;
    DescriptorPool::CreateInfo _descriptor_pool_create_info;

    struct QueueInfo
    {
        VkQueue queue = VK_NULL_HANDLE;
        std::uint32_t family = 0;
        bool valid = false;
    };

    QueueInfo _queues[static_cast<std::size_t>(QueueType::Count)]{};

    // Lazy-initialized command pools
    std::array<CommandPool, NUM_QUEUES> _command_pools;

    struct ImmediateContext
    {
        VkCommandPool pool = VK_NULL_HANDLE;
        VkCommandBuffer cmd = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;
    };

    ImmediateContext _immediate_contexts[static_cast<std::size_t>(QueueType::Count)]{};

    std::vector<std::string> _enabled_device_extensions;

    bool _feature_flags[static_cast<std::size_t>(Feature::Count)]{};

    // lazy-initialized
    std::unique_ptr<StagingBufferManager> _staging;
    std::unique_ptr<TimelineCallbackSystem> _timeline_callbacks;
    std::unique_ptr<RenderDocIntegration> _renderdoc;
    std::unique_ptr<FencePool> _fence_pool;
    std::unique_ptr<DescriptorPool> _descriptor_pool;
    std::unique_ptr<CommandProfiler> _command_profiler;

    bool _validation_enabled = false;

    static VkResult create_instance(const CreateInfo& info,
                                    VkInstance& out_instance,
                                    VkDebugUtilsMessengerEXT& out_messenger);

    Expected<SelectedDevice> select_device(const DeviceFilter& filter);

    VkResult create_logical_device(const SelectedDevice& selected,
                                   const FeaturesRequested& features);

    void destroy();

    static std::uint32_t find_memory_type(VkPhysicalDevice pd,
                                     std::uint32_t type_filter,
                                     VkMemoryPropertyFlags props);
};

// ============================================================================
//  Utility functions
// ============================================================================

inline std::uint32_t find_memory_type(VkPhysicalDevice physical_device,
                                      std::uint32_t type_filter,
                                      VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

    for(std::uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i)
    {
        if((type_filter & (1u << i)) &&
           (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    return std::numeric_limits<std::uint32_t>::max();
}

} // namespace proserpine

// =============================================================================
//  IMPLEMENTATION
// =============================================================================

#if defined(PROSERPINE_IMPLEMENTATION)

#if defined(PROSERPINE_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#elif defined(PROSERPINE_PLATFORM_LINUX)
#include <dlfcn.h>
#endif

namespace proserpine {

// =============================================================================
//  Debug callback for validation layers
// =============================================================================

static VKAPI_ATTR VkBool32 VKAPI_CALL proserpine_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                                VkDebugUtilsMessageTypeFlagsEXT type,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* data,
                                                                void* /* user_data */)
{
    switch(severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            __LOG_TRACE("[debug callback] : " __FMT_STR, data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            __LOG_INFO("[debug callback] : " __FMT_STR, data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            __LOG_WARN("[debug callback] : " __FMT_STR, data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            __LOG_ERROR("[debug callback] : " __FMT_STR, data->pMessage);
            break;
        default:
            __LOG_ERROR("[debug callback] : " __FMT_STR, data->pMessage);
            break;
    }

    return VK_FALSE;
}

// =============================================================================
//  Extension/Layer checking
// =============================================================================

static bool has_extension(const std::vector<VkExtensionProperties>& exts,
                          const char* name)
{
    return std::any_of(exts.begin(), exts.end(), [name](const VkExtensionProperties& e) {
        return std::strcmp(e.extensionName, name) == 0;
    });
}

static bool has_layer(const std::vector<VkLayerProperties>& layers,
                      const char* name)
{
    return std::any_of(layers.begin(), layers.end(), [name](const VkLayerProperties& l) {
        return std::strcmp(l.layerName, name) == 0;
    });
}

// =============================================================================
//  VulkanContext::create_instance
// =============================================================================

inline VkResult VulkanContext::create_instance(const CreateInfo& info,
                                               VkInstance& out_instance,
                                               VkDebugUtilsMessengerEXT& out_messenger)
{
    __LOG_TRACE("Creating a new VulkanContext instance");

    VkApplicationInfo app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.pApplicationName = info.application_name;
    app_info.applicationVersion = info.application_version;
    app_info.pEngineName = info.engine_name;
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = info.device_filter.min_api_version;

    std::vector<const char*> extensions(info.extra_instance_extensions);

    std::uint32_t ext_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);
    std::vector<VkExtensionProperties> available(ext_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, available.data());

    if(info.enable_validation && has_extension(available, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    if(has_extension(available, "VK_KHR_get_surface_capabilities2"))
        extensions.push_back("VK_KHR_get_surface_capabilities2");

    std::vector<const char*> layers(info.extra_instance_layers);

    std::uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    if(info.enable_validation && has_layer(available_layers, "VK_LAYER_KHRONOS_validation"))
        layers.push_back("VK_LAYER_KHRONOS_validation");

    VkInstanceCreateInfo create_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
    create_info.ppEnabledLayerNames = layers.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};

    if(info.enable_validation)
    {
        __LOG_TRACE("Enabling validation in new VulkanContext instance");

        debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        debug_create_info.pfnUserCallback = proserpine_debug_callback;
        create_info.pNext = &debug_create_info;
    }

    VkResult result = vkCreateInstance(&create_info, nullptr, &out_instance);

    if(result != VK_SUCCESS)
    {
        __LOG_ERROR("Error when creating a new VulkanContext instance (" __FMT_I32 ")",
                           static_cast<std::int32_t>(result));
        return result;
    }

    if(info.enable_validation)
    {
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(out_instance, "vkCreateDebugUtilsMessengerEXT"));

        if(func)
            func(out_instance, &debug_create_info, nullptr, &out_messenger);
    }

    __LOG_TRACE("Created a new VulkanContext instance");

    return VK_SUCCESS;
}

// =============================================================================
//  VulkanContext::select_device
// =============================================================================

inline Expected<SelectedDevice> VulkanContext::select_device(const DeviceFilter& filter)
{
    __LOG_TRACE("Selecting device");

    std::uint32_t count = 0;
    vkEnumeratePhysicalDevices(this->_instance, &count, nullptr);

    if(count == 0)
        return Error("No Vulkan physical devices found");

    __LOG_TRACE(__FMT_U32 " device(s) available", count);

    std::vector<VkPhysicalDevice> physical_devices(count);
    vkEnumeratePhysicalDevices(this->_instance, &count, physical_devices.data());

    struct Candidate
    {
        VkPhysicalDevice physical_device;
        DeviceProperties properties;
        std::int32_t score;
    };

    std::vector<Candidate> candidates;

    for(auto physical_device : physical_devices)
    {
        __LOG_TRACE("Evaluating physical device " __FMT_U64H "",
                    reinterpret_cast<std::uint64_t>(physical_device));

        DeviceProperties device_properties;

        vkGetPhysicalDeviceProperties(physical_device, &device_properties.properties);

        // Version check
        if(device_properties.properties.apiVersion < filter.min_api_version)
        {
            __LOG_TRACE("Device does not support minimum API version");
            continue;
        }

        // Features chain
        VkPhysicalDeviceFeatures2 features2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        device_properties.features_13.pNext = nullptr;
        device_properties.features_12.pNext = &device_properties.features_13;
        device_properties.features_11.pNext = &device_properties.features_12;
        features2.pNext = &device_properties.features_11;
        vkGetPhysicalDeviceFeatures2(physical_device, &features2);
        device_properties.features = features2.features;

        // Memory
        vkGetPhysicalDeviceMemoryProperties(physical_device, &device_properties.memory_properties);

        // Queue families
        std::uint32_t qf_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &qf_count, nullptr);
        device_properties.queue_families.resize(qf_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &qf_count, device_properties.queue_families.data());

        // Extensions
        std::uint32_t ext_count = 0;
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, nullptr);
        device_properties.available_extensions.resize(ext_count);
        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, device_properties.available_extensions.data());

        // Check required extensions
        bool has_all_exts = true;

        for(auto ext : filter.required_extensions)
        {
            if(!has_extension(device_properties.available_extensions, ext))
            {
                has_all_exts = false;
                break;
            }
        }

        if(!has_all_exts)
        {
            __LOG_TRACE("Device does not support all required extensions");
            continue;
        }

        // Check queue family requirements
        bool has_graphics = false;
        bool has_present = false;

        for(std::uint32_t i = 0; i < qf_count; ++i)
        {
            if(device_properties.queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                // Timestamp (for CommandProfiler)
                if(device_properties.queue_families[i].timestampValidBits == 0)
                {
                    __LOG_TRACE("Graphics queue family does not support timestamp queries");
                    continue;
                }

                has_graphics = true;

                __LOG_TRACE("Device has a valid graphics queue family");
            }

            if(this->_surface != VK_NULL_HANDLE)
            {
                VkBool32 present_support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(physical_device,
                                                     i,
                                                     this->_surface,
                                                     &present_support);

                if(!has_present && present_support)
                    has_present = true;
            }
        }

        if(!has_graphics)
        {
            __LOG_TRACE("Device has no graphics queue family");
            continue;
        }

        if(this->_surface != VK_NULL_HANDLE && !has_present)
        {
            __LOG_TRACE("Device has no present queue family");
            continue;
        }

        // Timestamp (for CommandProfiler)
        if(device_properties.properties.limits.timestampPeriod == 0)
        {
            __LOG_TRACE("Device does not support timestamp queries");
            continue;
        }

        // Default scoring
        std::int32_t score = 0;

        switch(device_properties.properties.deviceType)
        {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: score = 1000; break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: score = 500;  break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: score = 250;  break;
            default: score = 100;  break;
        }

        // User scoring override
        if(filter.scorer)
        {
            std::int32_t user_score = filter.scorer(physical_device, device_properties);

            if(user_score < 0)
            {
                __LOG_TRACE("User-scoring rejected device");
                continue;
            }

            score = user_score;
        }

        __LOG_TRACE("Found a candidate physical device " __FMT_U64H "",
                    reinterpret_cast<std::uint64_t>(physical_device));

        candidates.push_back({physical_device, std::move(device_properties), score});
    }

    if(candidates.empty())
        return Error("No suitable Vulkan device found matching the filter criteria");

    std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) { return a.score > b.score; });

    auto& best = candidates[0];
    SelectedDevice selected;
    selected.physical_device = best.physical_device;
    selected.props = std::move(best.properties);

    auto& qfams = selected.props.queue_families;
    std::uint32_t qf_count = static_cast<std::uint32_t>(qfams.size());

    for(std::uint32_t i = 0; i < qf_count; ++i)
    {
        // Graphics Queue Family
        if(qfams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
           !selected.queue_family_valid[static_cast<std::size_t>(QueueType::Graphics)])
        {
            selected.queue_family_indices[static_cast<std::size_t>(QueueType::Graphics)] = i;
            selected.queue_family_valid[static_cast<std::size_t>(QueueType::Graphics)] = true;

            __LOG_TRACE("Found Graphics Queue");
        }

        // Compute Queue Family
        if((qfams[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
           i != selected.queue_family_indices[static_cast<std::size_t>(QueueType::Graphics)])
        {
            selected.queue_family_indices[static_cast<std::size_t>(QueueType::Compute)] = i;
            selected.queue_family_valid[static_cast<std::size_t>(QueueType::Compute)] = true;

            __LOG_TRACE("Found Compute Queue");
        }

        // Present Queue Family
        if(this->_surface != VK_NULL_HANDLE)
        {
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(selected.physical_device,
                                                 i,
                                                 this->_surface,
                                                 &present_support);

            if(present_support &&
               !selected.queue_family_valid[static_cast<std::size_t>(QueueType::Present)])
            {
                selected.queue_family_indices[static_cast<std::size_t>(QueueType::Present)] = i;
                selected.queue_family_valid[static_cast<std::size_t>(QueueType::Present)] = true;

                __LOG_TRACE("Found Present Queue");
            }
        }

        // Transfer queue (we prefer a dedicated one)
        if((qfams[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
           !(qfams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
           !(qfams[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
        {
            selected.queue_family_indices[static_cast<std::size_t>(QueueType::Transfer)] = i;
            selected.queue_family_valid[static_cast<std::size_t>(QueueType::Transfer)] = true;
        }
    }

    if(!selected.queue_family_valid[static_cast<std::size_t>(QueueType::Compute)] &&
       !filter.require_separate_compute_queue)
    {
        // Fall back to graphics family which also supports compute
        for(std::uint32_t i = 0; i < qf_count; ++i)
        {
            if(qfams[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                selected.queue_family_indices[static_cast<std::size_t>(QueueType::Compute)] = i;
                selected.queue_family_valid[static_cast<std::size_t>(QueueType::Compute)]   = true;
                break;
            }
        }
    }

    if(!selected.queue_family_valid[static_cast<std::size_t>(QueueType::Transfer)] &&
       !filter.require_separate_transfer_queue)
    {
        // Fall back to any family with transfer
        for(std::uint32_t i = 0; i < qf_count; ++i)
        {
            if(qfams[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                selected.queue_family_indices[static_cast<std::size_t>(QueueType::Transfer)] = i;
                selected.queue_family_valid[static_cast<std::size_t>(QueueType::Transfer)]   = true;
                break;
            }
        }
    }

    return selected;
}

// =============================================================================
//  VulkanContext::create_logical_device
// =============================================================================

inline VkResult VulkanContext::create_logical_device(const SelectedDevice& selected,
                                                     const FeaturesRequested& features)
{
    __LOG_TRACE("Creating a new logical device");

    std::vector<std::uint32_t> unique_families;

    for(std::size_t i = 0; i < static_cast<std::size_t>(QueueType::Count); ++i)
    {
        if(selected.queue_family_valid[i])
        {
            std::uint32_t f = selected.queue_family_indices[i];

            if(std::find(unique_families.begin(), unique_families.end(), f) == unique_families.end())
                unique_families.push_back(f);
        }
    }

    float priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    for(auto family : unique_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        queue_create_info.queueFamilyIndex = family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &priority;
        queue_create_infos.push_back(queue_create_info);
    }

    std::vector<const char*> device_extensions;

    auto maybe_add_ext = [&](const char* ext) -> bool {
        if(has_extension(selected.props.available_extensions, ext))
        {
            device_extensions.push_back(ext);
            return true;
        }

        return false;
    };

    if(features.surface)
    {
        if(maybe_add_ext(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
            __LOG_TRACE("Detected " __FMT_STR " extension, added " __FMT_STR " extension",
                        VK_KHR_SURFACE_EXTENSION_NAME,
                        VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    if(selected.props.properties.apiVersion < VK_API_VERSION_1_3)
    {
        maybe_add_ext("VK_KHR_synchronization2");
        maybe_add_ext("VK_KHR_timeline_semaphore");
        maybe_add_ext("VK_KHR_buffer_device_address");
        maybe_add_ext("VK_EXT_descriptor_indexing");
        maybe_add_ext("VK_KHR_create_renderpass2");
        maybe_add_ext("VK_KHR_dynamic_rendering");
    }

    for(auto ext : features.extra_device_extensions)
        maybe_add_ext(ext);

    this->_enabled_device_extensions.clear();

    for(auto ext : device_extensions)
        this->_enabled_device_extensions.emplace_back(ext);

    VkPhysicalDeviceFeatures2 features2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};

    VkPhysicalDeviceVulkan11Features f11{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
    VkPhysicalDeviceVulkan12Features f12{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    VkPhysicalDeviceVulkan13Features f13{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};

    f13.pNext = nullptr;
    f12.pNext = &f13;
    f11.pNext = &f12;
    features2.pNext = &f11;

    if(features.timeline_semaphore)
    {
        f12.timelineSemaphore = VK_TRUE;
        this->_feature_flags[static_cast<std::size_t>(Feature::TimelineSemaphore)] = true;
    }

    if(features.buffer_device_address)
    {
        f12.bufferDeviceAddress = VK_TRUE;
        this->_feature_flags[static_cast<std::size_t>(Feature::BufferDeviceAddress)] = true;
    }

    if(features.descriptor_indexing)
    {
        f12.descriptorIndexing = VK_TRUE;
        f12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        f12.descriptorBindingVariableDescriptorCount = VK_TRUE;
        f12.runtimeDescriptorArray = VK_TRUE;
        f12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
        f12.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
        f12.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
        f12.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
        f12.descriptorBindingPartiallyBound = VK_TRUE;
        this->_feature_flags[static_cast<std::size_t>(Feature::DescriptorIndexing)] = true;
    }

    if(features.synchronization2)
    {
        f13.synchronization2  = VK_TRUE;
        this->_feature_flags[static_cast<std::size_t>(Feature::Synchronization2)] = true;
    }

    if(features.dynamic_rendering)
    {
        f13.dynamicRendering   = VK_TRUE;
        this->_feature_flags[static_cast<std::size_t>(Feature::DynamicRendering)] = true;
    }

    if(features.shader_int64)
    {
        features2.features.shaderInt64   = VK_TRUE;
        this->_feature_flags[static_cast<std::size_t>(Feature::ShaderInt64)] = true;
    }

    if(features.shader_float64)
    {
        features2.features.shaderFloat64 = VK_TRUE;
        this->_feature_flags[static_cast<std::size_t>(Feature::ShaderFloat64)] = true;
    }

    VkDeviceCreateInfo device_create_info{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.pNext = &features2;
    device_create_info.queueCreateInfoCount = static_cast<std::uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.enabledExtensionCount = static_cast<std::uint32_t>(device_extensions.size());
    device_create_info.ppEnabledExtensionNames = device_extensions.data();

    VkResult result = vkCreateDevice(selected.physical_device, &device_create_info, nullptr, &_device);

    if(result != VK_SUCCESS)
    {
        __LOG_ERROR("Error during logical device creation");
        return result;
    }

    // Create Queues
    __LOG_TRACE("Creating logical device queues");

    for(std::uint32_t i = 0; i < static_cast<std::uint32_t>(QueueType::Count); ++i)
    {
        if(selected.queue_family_valid[i])
        {
            __LOG_TRACE("Getting Device Queue: " __FMT_U32 "", i);
            this->_queues[i].family = selected.queue_family_indices[i];
            this->_queues[i].valid  = true;
            vkGetDeviceQueue(this->_device, this->_queues[i].family, 0, &this->_queues[i].queue);
        }
        else
        {
            __LOG_TRACE("Cannot get Device Queue: " __FMT_U32 "", i);
        }
    }

    for(std::size_t i = 0; i < static_cast<std::size_t>(QueueType::Count); ++i)
    {
        if(this->_queues[i].valid)
        {
            // TODO: error check
            VkCommandPoolCreateInfo pool_create_info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
            pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            pool_create_info.queueFamilyIndex = this->_queues[i].family;

            vkCreateCommandPool(this->_device, &pool_create_info, nullptr, &this->_immediate_contexts[i].pool);

            VkCommandBufferAllocateInfo alloc_create_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
            alloc_create_info.commandPool = this->_immediate_contexts[i].pool;
            alloc_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            alloc_create_info.commandBufferCount = 1;

            vkAllocateCommandBuffers(this->_device, &alloc_create_info, &this->_immediate_contexts[i].cmd);

            VkFenceCreateInfo fence_create_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
            fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            vkCreateFence(this->_device, &fence_create_info, nullptr, &this->_immediate_contexts[i].fence);
        }
    }

    __LOG_TRACE("New logical device created successfully");

    return VK_SUCCESS;
}

// ============================================================================
//  VulkanContext::create
// ============================================================================

inline Expected<VulkanContext> VulkanContext::create(VulkanContext::CreateInfo& info,
                                                     std::function<bool(VkInstance, VkSurfaceKHR*)> create_surface_callback)
{
    __LOG_TRACE("Creating a new VulkanContext");

    VulkanContext ctx;
    ctx._validation_enabled = info.enable_validation;

    VkResult result = create_instance(info, ctx._instance, ctx._debug_messenger);

    if(result != VK_SUCCESS)
    {
        __LOG_ERROR("Failed to create a new VulkanContext");
        return Error(result, "Failed to create VulkanContext instance");
    }

    if(!create_surface_callback(ctx._instance, &ctx._surface))
    {
        __LOG_ERROR("Failed to create a surface");
        return Error(result, "Failed to create surface");
    }

    __LOG_TRACE("Created surface successfully");

    auto selected = ctx.select_device(info.device_filter);

    if(!selected)
        return selected.error();

    ctx._selected_device = selected.value();
    ctx._physical_device = ctx._selected_device.physical_device;
    ctx._device_properties = ctx._selected_device.props;

    if(std::any_of(info.extra_instance_extensions.begin(),
                   info.extra_instance_extensions.end(), [](const char* ext_name) {
                       return std::strcmp(ext_name, VK_KHR_SURFACE_EXTENSION_NAME) == 0;
                   }))
    {
        __LOG_TRACE("Enabling VK_KHR_Surface extension");
        info.features.surface = true;
    }

    result = ctx.create_logical_device(ctx._selected_device, info.features);

    if(result != VK_SUCCESS)
        return Error(result, "Failed to create logical device");

    ctx._descriptor_pool_create_info = info.descriptor_pool_info;

    __LOG_TRACE("Created a new VulkanContext successfully");

    return ctx;
}

// ============================================================================
//  VulkanContext dtor/move
// ============================================================================

inline void VulkanContext::destroy()
{
    if(this->_instance != VK_NULL_HANDLE)
    {
        __LOG_TRACE("VulkanContext: Destroying");

        if(this->_device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(this->_device);

            this->_staging.reset();
            this->_timeline_callbacks.reset();
            this->_fence_pool.reset();
            this->_descriptor_pool.reset();
            this->_command_profiler.reset();

            for(auto& context : this->_immediate_contexts)
            {
                if(context.cmd != VK_NULL_HANDLE)
                {
                    vkFreeCommandBuffers(this->_device, context.pool, 1, &context.cmd);
                    context.cmd = VK_NULL_HANDLE;
                }

                if(context.pool != VK_NULL_HANDLE)
                {
                    vkDestroyCommandPool(this->_device, context.pool, nullptr);
                    context.pool = VK_NULL_HANDLE;
                }

                if(context.fence != VK_NULL_HANDLE)
                {
                    vkDestroyFence(this->_device, context.fence, nullptr);
                    context.fence = VK_NULL_HANDLE;
                }
            }

            vkDestroyDevice(this->_device, nullptr);
            this->_device = VK_NULL_HANDLE;
        }

        if(this->_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(this->_instance, this->_surface, nullptr);
            this->_surface = VK_NULL_HANDLE;
        }

        if(this->_debug_messenger != VK_NULL_HANDLE && this->_instance != VK_NULL_HANDLE)
        {
            auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->_instance, "vkDestroyDebugUtilsMessengerEXT"));

            if(func)
                func(this->_instance, this->_debug_messenger, nullptr);

            this->_debug_messenger = VK_NULL_HANDLE;
        }

        vkDestroyInstance(this->_instance, nullptr);
        this->_instance = VK_NULL_HANDLE;
    }
}

inline VulkanContext::~VulkanContext() { this->destroy(); }

inline VulkanContext::VulkanContext(VulkanContext&& other) noexcept
{
    __MOVE_AND_NULL(_instance, _debug_messenger, _physical_device, _device, _surface);
    __MOVE(_device_properties, _selected_device, _enabled_device_extensions, _staging, _timeline_callbacks, _renderdoc, _fence_pool, _descriptor_pool, _command_profiler, _validation_enabled);
    __MEMCPY_AND_ZERO(_queues, _immediate_contexts, _feature_flags);
}

inline VulkanContext& VulkanContext::operator=(VulkanContext&& other) noexcept
{
    if(this != &other)
    {
        this->destroy();

        __MOVE_AND_NULL(_instance, _debug_messenger, _physical_device, _device, _surface);
        __MOVE(_device_properties, _selected_device, _enabled_device_extensions, _staging, _timeline_callbacks, _renderdoc, _fence_pool, _descriptor_pool, _command_profiler, _validation_enabled);
        __MEMCPY_AND_ZERO(_queues, _immediate_contexts, _feature_flags);
    }

    return *this;
}

// =============================================================================
//  VulkanContext accessors/getters
// =============================================================================

inline VkQueue VulkanContext::queue(QueueType type,
                                    std::uint32_t /* index */) const
{
    std::size_t i = static_cast<std::size_t>(type);

    PROSERPINE_ASSERT(this->_queues[i].valid, "Queue is invalid", VK_NULL_HANDLE);

    return this->_queues[i].queue;
}

inline std::uint32_t VulkanContext::queue_family(QueueType type) const
{
    std::size_t i = static_cast<std::size_t>(type);
    PROSERPINE_ASSERT(this->_queues[i].valid, "Queue is invalid", PROSERPINE_INVALID_QUEUE_INDEX);
    return this->_queues[i].family;
}

inline bool VulkanContext::has_queue(QueueType type) const
{
    return this->_queues[static_cast<std::size_t>(type)].valid;
}

inline bool VulkanContext::is_extension_enabled(const char* name) const
{
    return std::any_of(this->_enabled_device_extensions.begin(),
                       this->_enabled_device_extensions.end(),
                       [name](const std::string& s) { return s == name; });
}

inline bool VulkanContext::is_feature_supported(Feature feature) const
{
    return this->_feature_flags[static_cast<std::size_t>(feature)];
}

inline StagingBufferManager& VulkanContext::staging()
{
    if(!this->_staging)
    {
        QueueType qt = this->has_queue(QueueType::Transfer) ? QueueType::Transfer : QueueType::Graphics;

        this->_staging = std::make_unique<StagingBufferManager>(this->_device,
                                                                this->_physical_device,
                                                                this->queue(qt),
                                                                this->queue_family(qt));
    }

    return *this->_staging;
}

inline TimelineCallbackSystem& VulkanContext::timeline_callbacks()
{
    if(!this->_timeline_callbacks)
        this->_timeline_callbacks = std::make_unique<TimelineCallbackSystem>(this->_device);

    return *this->_timeline_callbacks;
}

inline RenderDocIntegration& VulkanContext::renderdoc()
{
    if(!this->_renderdoc)
        this->_renderdoc = std::make_unique<RenderDocIntegration>();

    return *this->_renderdoc;
}

inline FencePool& VulkanContext::fence_pool()
{
    if(!this->_fence_pool)
        this->_fence_pool = std::make_unique<FencePool>(this->_device);

    return *this->_fence_pool;
}

inline DescriptorPool& VulkanContext::descriptor_pool()
{
    if(!this->_descriptor_pool)
        this->_descriptor_pool = std::make_unique<DescriptorPool>(this->_device,
                                                                  this->_descriptor_pool_create_info);

    return *this->_descriptor_pool;
}

inline CommandPool& VulkanContext::command_pool(QueueType queue_type)
{
    const std::size_t index = static_cast<std::size_t>(queue_type);

    if(!this->_command_pools.at(index).ready())
        this->_command_pools[index] = std::move(CommandPool(this->_device, this->queue_family(queue_type)));

    return this->_command_pools[index];
}

inline CommandProfiler& VulkanContext::command_profiler()
{
    if(!this->_command_profiler)
        this->_command_profiler = std::make_unique<CommandProfiler>(this->_device,
                                                                    this->_physical_device);

    return *this->_command_profiler;
}

// =============================================================================
//  VulkanContext object creation helpers
// =============================================================================

inline std::uint32_t VulkanContext::find_memory_type(VkPhysicalDevice physical_device,
                                                     std::uint32_t type_filter,
                                                     VkMemoryPropertyFlags props)
{
    return proserpine::find_memory_type(physical_device, type_filter, props);
}

inline Expected<Buffer> VulkanContext::create_buffer(const Buffer::CreateInfo& info)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));
    PROSERPINE_ASSERT(this->_physical_device != VK_NULL_HANDLE, "Physical device is NULL", Error("Invalid Physical Device"));

    return Buffer(this->_device, this->_physical_device, info);
}

inline Expected<Image> VulkanContext::create_image(const Image::CreateInfo& info)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));
    PROSERPINE_ASSERT(this->_physical_device != VK_NULL_HANDLE, "Physical device is NULL", Error("Invalid Physical Device"));

    return Image(this->_device, this->_physical_device, info);
}

inline Expected<Image> VulkanContext::create_depth_image(VkExtent2D extent,
                                                         VkFormat format)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));
    PROSERPINE_ASSERT(this->_physical_device != VK_NULL_HANDLE, "Physical device is NULL", Error("Invalid Physical Device"));

    Image::CreateInfo info{};
    info.extent = { extent.width, extent.height, 1};
    info.format = format;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    info.aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;

    return this->create_image(info);
}

inline Expected<TimelineSemaphore> VulkanContext::create_timeline_semaphore(std::uint64_t initial_value)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));

    return TimelineSemaphore(this->_device, initial_value);
}

inline Expected<Semaphore> VulkanContext::create_semaphore()
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));

    return Semaphore(this->_device);
}

inline Expected<Fence> VulkanContext::create_fence(bool signaled)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));

    return Fence(this->_device, signaled);
}

inline Expected<SwapChain> VulkanContext::create_swapchain(SwapChain::CreateInfo& create_info)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));
    PROSERPINE_ASSERT(this->_surface != VK_NULL_HANDLE, "Surface is NULL", Error("Invalid Surface"));

    VkPhysicalDeviceSurfaceInfo2KHR surface_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR};
    surface_info.pNext = nullptr;
    surface_info.surface = this->_surface;

    VkSurfaceCapabilities2KHR capabilities{VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR};
    capabilities.pNext = nullptr;

    vkGetPhysicalDeviceSurfaceCapabilities2KHR(this->_physical_device,
                                               &surface_info,
                                               &capabilities);

    SwapChain sc = SwapChain(this->_device, this->_surface);

    VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    std::uint32_t image_count = 0;

    // Present format

    std::uint32_t formats_count;

    vkGetPhysicalDeviceSurfaceFormats2KHR(this->_physical_device,
                                          &surface_info,
                                          &formats_count,
                                          nullptr);

    if(formats_count == 0)
        return Error("SwapChain: Could not find any available format");

    std::vector<VkSurfaceFormat2KHR> formats(static_cast<std::size_t>(formats_count),
                                                {VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR});

    vkGetPhysicalDeviceSurfaceFormats2KHR(this->_physical_device,
                                          &surface_info,
                                          &formats_count,
                                          formats.data());


    for(const auto& format : formats)
    {
        if(format.surfaceFormat.format == create_info.format &&
           format.surfaceFormat.colorSpace == create_info.colorspace)
        {
            sc._format = format.surfaceFormat.format;
            color_space = format.surfaceFormat.colorSpace;
            break;
        }
    }

    if(sc._format == VK_FORMAT_UNDEFINED)
    {
        sc._format = formats[0].surfaceFormat.format;

        __LOG_WARN("Requested VkFormat " __FMT_I32 " is not available, defaulting to " __FMT_I32,
                   static_cast<std::int32_t>(create_info.format),
                   static_cast<std::int32_t>(sc._format));
    }

    __LOG_TRACE("SwapChain: Format:" __FMT_I32 ", ColorSpace: " __FMT_I32,
               static_cast<std::int32_t>(sc._format),
               static_cast<std::int32_t>(color_space));

    // Present Mode

    std::vector<VkPresentModeKHR> present_modes;
    std::uint32_t present_modes_count;

    vkGetPhysicalDeviceSurfacePresentModesKHR(this->_physical_device,
                                              this->_surface,
                                              &present_modes_count,
                                              nullptr);

    if(present_modes_count == 0)
        return Error("SwapChain: Could not find any available present mode");

    present_modes.resize(static_cast<std::size_t>(present_modes_count));
    vkGetPhysicalDeviceSurfacePresentModesKHR(this->_physical_device,
                                              this->_surface,
                                              &present_modes_count,
                                              present_modes.data());

    bool found_present_mode = false;

    for(const auto& mode : present_modes)
    {
        if(present_mode == create_info.present_mode)
        {
            present_mode = mode;
            found_present_mode = true;
        }
    }

    if(!found_present_mode)
    {
        present_mode = VK_PRESENT_MODE_FIFO_KHR;

        __LOG_WARN("Requested present mode " __FMT_I32 " is not available, defaulting to " __FMT_I32,
                   static_cast<std::int32_t>(create_info.present_mode),
                   static_cast<std::int32_t>(present_mode));
    }

    __LOG_TRACE("SwapChain: Present mode: " __FMT_I32,
                static_cast<std::int32_t>(present_mode));

    // Extent

    if(capabilities.surfaceCapabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
    {
        sc._extent = capabilities.surfaceCapabilities.currentExtent;
    }
    else
    {
        sc._extent.width = std::clamp(create_info.extent_width,
                                      capabilities.surfaceCapabilities.minImageExtent.width,
                                      capabilities.surfaceCapabilities.maxImageExtent.width);
        sc._extent.height = std::clamp(create_info.extent_height,
                                       capabilities.surfaceCapabilities.minImageExtent.height,
                                       capabilities.surfaceCapabilities.maxImageExtent.height);
    }

    __LOG_TRACE("SwapChain: Extent: " __FMT_U32 "x" __FMT_U32,
                sc._extent.width,
                sc._extent.height);

    // Image count

    image_count = capabilities.surfaceCapabilities.minImageCount + 1;

    if(capabilities.surfaceCapabilities.maxImageCount != 0 &&
       image_count > capabilities.surfaceCapabilities.maxImageCount)
        image_count = capabilities.surfaceCapabilities.maxImageCount;

    __LOG_TRACE("SwapChain: Image Count: " __FMT_U32, image_count);

    // SwapChain creation

    VkSwapchainCreateInfoKHR sc_create_info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    sc_create_info.surface = this->_surface;
    sc_create_info.imageFormat = sc._format;
    sc_create_info.imageColorSpace = color_space;
    sc_create_info.imageExtent = sc._extent;
    sc_create_info.imageArrayLayers = 1;
    sc_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    sc_create_info.presentMode = present_mode;
    sc_create_info.clipped = VK_TRUE;
    sc_create_info.minImageCount = image_count;
    sc_create_info.preTransform = capabilities.surfaceCapabilities.currentTransform;
    sc_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    sc_create_info.oldSwapchain = VK_NULL_HANDLE;

    std::uint32_t queue_family_indices[2] = {
        this->_queues[static_cast<std::size_t>(QueueType::Graphics)].family,
        this->_queues[static_cast<std::size_t>(QueueType::Present)].family
    };

    if(queue_family_indices[0] != queue_family_indices[1])
    {

        sc_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        sc_create_info.queueFamilyIndexCount = 2;
        sc_create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        sc_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        sc_create_info.queueFamilyIndexCount = 0;
        sc_create_info.pQueueFamilyIndices = nullptr;
    }

    PROSERPINE_VK_CHECK_VOID(vkCreateSwapchainKHR(this->_device, &sc_create_info, nullptr, &sc._swapchain),
                             "Failed to create SwapChain");

    // Get swapchain images

    vkGetSwapchainImagesKHR(this->_device, sc._swapchain, &image_count, nullptr);
    sc._images.resize(static_cast<std::size_t>(image_count));
    vkGetSwapchainImagesKHR(this->_device, sc._swapchain, &image_count, sc._images.data());

    // Create swapchain image views and render semaphores

    sc._views.resize(static_cast<std::size_t>(image_count));
    sc._image_rendered_semaphores.resize(static_cast<std::size_t>(image_count));

    for(std::uint32_t i = 0; const auto& image : sc._images)
    {
        VkImageViewCreateInfo view_create_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_create_info.image = image;
        view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_create_info.format = sc._format;
        view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_create_info.subresourceRange.baseMipLevel = 0;
        view_create_info.subresourceRange.levelCount = 1;
        view_create_info.subresourceRange.baseArrayLayer = 0;
        view_create_info.subresourceRange.layerCount = 1;

        PROSERPINE_VK_CHECK_VOID(vkCreateImageView(this->_device,
                                                   &view_create_info,
                                                   nullptr,
                                                   &sc._views[i]),
                                 "Failed to create VkImageView");

        VkSemaphoreCreateInfo semaphore_create_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

        PROSERPINE_VK_CHECK_VOID(vkCreateSemaphore(this->_device,
                                                   &semaphore_create_info,
                                                   nullptr,
                                                   &sc._image_rendered_semaphores[i]),
                                 "Failed to create VkSemaphore");

        i++;
    }

    return sc;
}

inline Expected<CommandPool> VulkanContext::create_command_pool(QueueType queue_type)
{
    return CommandPool(this->_device, this->queue_family(queue_type));
}

inline void VulkanContext::wait_idle()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("VulkanContext: waiting for device idle");
        vkDeviceWaitIdle(this->_device);
    }
}

inline void VulkanContext::immediate_submit(QueueType queue_type,
                                            const std::function<void(VkCommandBuffer)>& record)
{
    // TODO: use timeline semaphore

    auto qi = static_cast<std::size_t>(queue_type);
    auto& imm_ctx = this->_immediate_contexts[qi];

    PROSERPINE_ASSERT(this->_queues[qi].valid, "Invalid queue", );

    PROSERPINE_ASSERT(imm_ctx.pool != VK_NULL_HANDLE, "Invalid CmdPool", );
    PROSERPINE_ASSERT(imm_ctx.cmd != VK_NULL_HANDLE, "Invalid CmdBuffer", );
    PROSERPINE_ASSERT(imm_ctx.fence != VK_NULL_HANDLE, "Invalid Fence", );

    vkWaitForFences(this->_device, 1, &imm_ctx.fence, VK_TRUE, PROSERPINE_WAIT_INFINITE);
    vkResetFences(this->_device, 1, &imm_ctx.fence);
    vkResetCommandPool(this->_device, imm_ctx.pool, 0);

    VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(imm_ctx.cmd, &begin);

    record(imm_ctx.cmd);

    vkEndCommandBuffer(imm_ctx.cmd);

    VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &imm_ctx.cmd;

    vkQueueSubmit(this->_queues[qi].queue, 1, &submit, imm_ctx.fence);

    vkWaitForFences(this->_device, 1, &imm_ctx.fence, VK_TRUE, PROSERPINE_WAIT_INFINITE);
}

inline void VulkanContext::submit(QueueType queue_type, const SubmitInfo& info)
{
    std::size_t qi = static_cast<std::size_t>(queue_type);
    auto queue = this->_queues[qi].queue;

    std::vector<VkCommandBuffer> cmd_handles;

    for(CommandBuffer* cmd : info.command_buffers)
    {
        cmd_handles.push_back(cmd->handle());
        cmd->_state = CommandBuffer::State::Pending;
    }

    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = static_cast<std::uint32_t>(cmd_handles.size());
    submit_info.pCommandBuffers = cmd_handles.data();

    if(info.wait_semaphores.size() > 0)
    {
        submit_info.waitSemaphoreCount = info.wait_semaphores.size();
        submit_info.pWaitSemaphores = info.wait_semaphores.data();
    }

    submit_info.pWaitDstStageMask = &info.wait_stages;

    if(info.finished_semaphores.size() > 0)
    {
        submit_info.signalSemaphoreCount = static_cast<std::uint32_t>(info.finished_semaphores.size());
        submit_info.pSignalSemaphores = info.finished_semaphores.data();
    }

    vkQueueSubmit(queue, 1, &submit_info, info.fence);
}

inline VkResult VulkanContext::present(const PresentInfo& info)
{
    VkPresentInfoKHR present_info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &info.swapchain;
    present_info.pImageIndices = &info.image_index;

    if(info.wait_semaphores.size() > 0)
    {
        present_info.waitSemaphoreCount = static_cast<std::uint32_t>(info.wait_semaphores.size());
        present_info.pWaitSemaphores = info.wait_semaphores.data();
    }

    return vkQueuePresentKHR(this->queue(QueueType::Present), &present_info);
}

inline Expected<DescriptorSet> VulkanContext::allocate_descriptor_set(VkDescriptorSetLayout layout)
{
    return this->descriptor_pool().allocate_descriptor_set(layout);
}

// ============================================================================
//  SwapChain implementation
// ============================================================================

inline SwapChain::~SwapChain()
{
    if(this->_device != VK_NULL_HANDLE && this->_swapchain != VK_NULL_HANDLE)
    {
        __LOG_TRACE("SwapChain: Destroying");

        vkDestroySwapchainKHR(this->_device, this->_swapchain, nullptr);

        this->_images.clear();

        for(auto& view : this->_views)
            vkDestroyImageView(this->_device, view, nullptr);

        for(auto& semaphore : this->_image_rendered_semaphores)
            vkDestroySemaphore(this->_device, semaphore, nullptr);

        this->_views.clear();

        this->_device = VK_NULL_HANDLE;
        this->_swapchain = VK_NULL_HANDLE;
    }
}

inline SwapChain::SwapChain(SwapChain&& other) noexcept
{
    __MOVE_AND_NULL(_device, _surface, _swapchain);
    __MOVE(_images, _views, _first_use, _image_rendered_semaphores, _format, _extent);
}

inline SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _surface, _swapchain);
        __MOVE(_images, _views, _first_use, _image_rendered_semaphores, _format, _extent);
    }

    return *this;
}

// ============================================================================
//  TimelineSemaphore implementation
// ============================================================================

inline TimelineSemaphore::TimelineSemaphore(VkDevice device,
                                            std::uint64_t initial_value) : _device(device)
{
    VkSemaphoreTypeCreateInfo type_ci{VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO};
    type_ci.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    type_ci.initialValue  = initial_value;

    VkSemaphoreCreateInfo ci{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    ci.pNext = &type_ci;

    PROSERPINE_VK_CHECK_VOID(vkCreateSemaphore(device, &ci, nullptr, &_sema),
                             "Failed to create timeline semaphore");

    __LOG_TRACE("Created a new timeline semaphore with value: " __FMT_U64 "",
                initial_value);
}

inline TimelineSemaphore::~TimelineSemaphore()
{
    if(this->_sema != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("Destroyed a timeline semaphore");
        vkDestroySemaphore(this->_device, this->_sema, nullptr);
    }
}

inline TimelineSemaphore::TimelineSemaphore(TimelineSemaphore&& other) noexcept
{
    __MOVE_AND_NULL(_device, _sema);
}

inline TimelineSemaphore& TimelineSemaphore::operator=(TimelineSemaphore&& other) noexcept
{
    if(this != &other)
    {
        if(this->_sema != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
            vkDestroySemaphore(_device, _sema, nullptr);

        __MOVE_AND_NULL(_device, _sema);
    }

    return *this;
}

inline void TimelineSemaphore::signal(std::uint64_t value)
{
    VkSemaphoreSignalInfo si{VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO};
    si.semaphore = _sema;
    si.value = value;

    PROSERPINE_VK_CHECK_VOID(vkSignalSemaphore(_device, &si),
                             "Failed to signal timeline semaphore");
}

inline VkResult TimelineSemaphore::wait(std::uint64_t value,
                                        std::uint64_t timeout_ns)
{
    VkSemaphoreWaitInfo wi{VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO};
    wi.semaphoreCount = 1;
    wi.pSemaphores = &_sema;
    wi.pValues = &value;

    return vkWaitSemaphores(_device, &wi, timeout_ns);
}

inline std::uint64_t TimelineSemaphore::counter() const
{
    std::uint64_t value = 0;
    vkGetSemaphoreCounterValue(this->_device, this->_sema, &value);
    return value;
}

// ============================================================================
//  Semaphore implementation
// ============================================================================

inline Semaphore::Semaphore(VkDevice device) : _device(device)
{
    VkSemaphoreCreateInfo info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    PROSERPINE_VK_CHECK_VOID(vkCreateSemaphore(this->_device, &info, nullptr, &this->_sema),
                             "Failed to create VkSemaphore");
}

inline Semaphore::~Semaphore()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        if(this->_sema != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(this->_device, this->_sema, nullptr);
            this->_sema = VK_NULL_HANDLE;
        }
    }
}

inline Semaphore::Semaphore(Semaphore&& other) noexcept
{
    __MOVE_AND_NULL(_device, _sema);
}

inline Semaphore& Semaphore::operator=(Semaphore&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _sema);
    }

    return *this;
}

// ============================================================================
//  TimelineCallbackSystem implementation
// ============================================================================

inline TimelineCallbackSystem::TimelineCallbackSystem(VkDevice device)
    : _device(device), _running(true)
{
    this->_worker = std::thread(&TimelineCallbackSystem::worker_loop, this);
}

inline TimelineCallbackSystem::~TimelineCallbackSystem()
{
    this->shutdown();
}

inline void TimelineCallbackSystem::shutdown()
{
    if(this->_running.exchange(false))
    {
        this->_cv.notify_all();

        if(this->_worker.joinable())
            this->_worker.join();

        std::lock_guard lock(this->_mutex);

        for(auto& pc : this->_pending)
            if(pc.callback)
                pc.callback();

        this->_pending.clear();
    }
}

inline void TimelineCallbackSystem::enqueue(VkSemaphore semaphore,
                                            std::uint64_t wait_value,
                                            std::function<void()> callback)
{
    std::lock_guard lock(this->_mutex);

    this->_pending.push_back({semaphore, wait_value, std::move(callback)});
    this->_cv.notify_one();
}

inline void TimelineCallbackSystem::cancel(VkSemaphore semaphore)
{
    std::lock_guard lock(this->_mutex);

    this->_pending.erase(std::remove_if(this->_pending.begin(),
                                        this->_pending.end(),
                                        [semaphore](const PendingCallback& pc) {
                                    return pc.semaphore == semaphore;
                                }),
                        this->_pending.end());
}

inline void TimelineCallbackSystem::worker_loop()
{
    while(this->_running)
    {
        std::vector<PendingCallback> work;

        {
            std::unique_lock lock(_mutex);
            this->_cv.wait_for(lock, std::chrono::milliseconds(10), [this] { return !this->_pending.empty() || !this->_running; });

            if(!this->_running && this->_pending.empty())
                break;

            work = std::move(this->_pending);
            this->_pending.clear();
        }

        std::vector<PendingCallback> still_pending;

        for(auto& pc : work)
        {
            VkSemaphoreWaitInfo wi{VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO};
            wi.semaphoreCount = 1;
            wi.pSemaphores = &pc.semaphore;
            wi.pValues = &pc.wait_value;

            VkResult result = vkWaitSemaphores(this->_device, &wi, 0);

            if(result == VK_SUCCESS)
            {
                if(pc.callback)
                    pc.callback();
            }
            else
            {
                still_pending.push_back(std::move(pc));
            }
        }

        if(!still_pending.empty())
        {
            std::lock_guard lock(this->_mutex);

            this->_pending.insert(this->_pending.end(),
                                  std::make_move_iterator(still_pending.begin()),
                                  std::make_move_iterator(still_pending.end()));
        }
    }
}

// =============================================================================
//  RenderDocIntegration implementation
// =============================================================================

inline RenderDocIntegration::RenderDocIntegration()
{
    __LOG_TRACE("Initializing RenderDoc integration");

    using GetApiFn = int(*)(int, void**);

#if defined(PROSERPINE_PLATFORM_LINUX)
    this->_module = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);

    if(this->_module == nullptr)
    {
        __LOG_WARN("Cannot find librenderdoc.so, disabling RenderDoc captures");
        return;
    }

    auto get_api = reinterpret_cast<GetApiFn>(dlsym(this->_module, "RENDERDOC_GetAPI"));
#elif defined(PROSERPINE_PLATFORM_WINDOWS)
    if(const char* path = std::getenv("PATH"))
    {
        const std::string_view path_view{path};
        const std::string_view delim{";"};

        for(const auto comp : std::ranges::split_view(path_view, delim))
        {
            const auto tmp_rdoc_path = std::filesystem::path(std::format("{}\\renderdoc.dll",
                                                             std::string_view(comp.data(), comp.size())));

            __LOG_TRACE("Looking for renderdoc.dll at: " __FMT_WSTR "", tmp_rdoc_path.c_str());

            if(std::filesystem::exists(tmp_rdoc_path))
            {
                __LOG_DEBUG("Found renderdoc.dll: " __FMT_WSTR "", tmp_rdoc_path.c_str());
                this->_module = LoadLibraryW(tmp_rdoc_path.c_str());
                break;
            }
        }
    }

    if(this->_module == nullptr)
    {
        __LOG_WARN("Cannot find or load renderdoc.dll, disabling RenderDoc captures");
        return;
    }

    auto get_api = reinterpret_cast<GetApiFn>(GetProcAddress(static_cast<HMODULE>(this->_module),
                                                             "RENDERDOC_GetAPI"));
#else
    return;
#endif // defined(PROSERPINE_PLATFORM_LINUX)

    __LOG_TRACE("Found RenderDoc library");

    if(get_api == nullptr)
    {
        __LOG_WARN("Cannot find RENDERDOC_GetAPI proc, disabling RenderDoc captures");
        return;
    }

    int ret = get_api(10600, &this->_api); // RENDERDOC_API_VERSION_1_6_0

    if(ret != 1 || this->_api == nullptr)
    {
        __LOG_ERROR("Cannot get RenderDoc api, disabling RenderDoc captures");
        return;
    }

    __LOG_INFO("Found RenderDoc api, enabling RenderDoc captures");

    return;
}

inline void RenderDocIntegration::start_capture()
{
    if(this->_api == nullptr)
        return;

    // renderdoc api: ((RENDERDOC_API_1_6_0*)api)->StartFrameCapture(NULL, NULL);
    // We use offset-based access to avoid including the renderdoc header
    // Offset of StartFrameCapture in RENDERDOC_API_1_6_0 struct

    using StartCaptureFn = void(*)(void*, void*);

    auto fn_table = static_cast<void**>(this->_api);
    auto start_fn = reinterpret_cast<StartCaptureFn>(fn_table[14]);

    if(start_fn != nullptr)
    {
        __LOG_TRACE("Starting RenderDoc Capture");
        start_fn(nullptr, nullptr);
    }
}

inline void RenderDocIntegration::end_capture()
{
    if(this->_api == nullptr)
        return;

    // EndFrameCapture is at index 15

    using EndCaptureFn = std::uint32_t(*)(void*, void*);

    auto fn_table = static_cast<void**>(this->_api);
    auto end_fn = reinterpret_cast<EndCaptureFn>(fn_table[15]);

    if(end_fn != nullptr)
    {
        __LOG_TRACE("Ending RenderDoc Capture");
        end_fn(nullptr, nullptr);
    }
}

// ============================================================================
//  Fence implementation
// ============================================================================

inline Fence::Fence(VkDevice device, bool signaled) : _device(device)
{
    __LOG_TRACE("Fence: Initializing");

    VkFenceCreateInfo info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    PROSERPINE_VK_CHECK_VOID(vkCreateFence(this->_device, &info, nullptr, &this->_fence),
                             "Failed to create Fence");
}

inline Fence::~Fence()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("Fence: Destroying");

        if(this->_fence != VK_NULL_HANDLE)
            vkDestroyFence(this->_device, this->_fence, nullptr);
    }
}

inline Fence::Fence(Fence&& other) noexcept
{
    __MOVE_AND_NULL(_device, _fence);
}

inline Fence& Fence::operator=(Fence&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _fence);
    }

    return *this;
}

// ============================================================================
//  Buffer implementation
// ============================================================================

inline Buffer::Buffer(VkDevice device,
                      VkPhysicalDevice physical_device,
                      const Buffer::CreateInfo& info) : _device(device),
                                                      _size(info.size)
{
    __LOG_TRACE("Buffer: Initializing Buffer (device: " __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(this->_device));

    VkBufferCreateInfo create_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    create_info.size  = info.size;
    create_info.usage = info.usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(info.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
        create_info.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    PROSERPINE_VK_CHECK_VOID(vkCreateBuffer(this->_device, &create_info, nullptr, &this->_buffer),
                             "Failed to create buffer");

    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(this->_device, this->_buffer, &mem_req);

    VkMemoryAllocateInfo alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc.allocationSize  = mem_req.size;
    alloc.memoryTypeIndex = find_memory_type(physical_device,
                                             mem_req.memoryTypeBits,
                                             info.memory_flags);

    VkMemoryAllocateFlagsInfo flags_info{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO};

    if(info.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
    {
        flags_info.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
        alloc.pNext = &flags_info;
    }

    PROSERPINE_VK_CHECK_VOID(vkAllocateMemory(this->_device, &alloc, nullptr, &this->_memory),
                             "Failed to allocate buffer memory");

    PROSERPINE_VK_CHECK_VOID(vkBindBufferMemory(this->_device, this->_buffer, this->_memory, 0),
                             "Failed to bind buffer memory");

    if(info.memory_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        vkMapMemory(this->_device, this->_memory, 0, info.size, 0, &this->_mapped);

    if(info.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
    {
        VkBufferDeviceAddressInfo addr_info{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
        addr_info.buffer = this->_buffer;
        this->_device_address = vkGetBufferDeviceAddress(this->_device, &addr_info);
    }
}

inline Buffer::~Buffer()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("Buffer: Destroying Buffer (device: " __FMT_U64H ")",
                    reinterpret_cast<std::uint64_t>(this->_device));

        if(this->_mapped)
        {
            vkUnmapMemory(this->_device, this->_memory);
            this->_mapped = nullptr;
        }

        if(this->_buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(this->_device, this->_buffer, nullptr);

        if(this->_memory != VK_NULL_HANDLE)
            vkFreeMemory(this->_device, this->_memory, nullptr);
    }
}

inline Buffer::Buffer(Buffer&& other) noexcept
{
    __MOVE_AND_NULL(_device, _buffer, _memory, _mapped);
    __MOVE_AND_ZERO(_size, _device_address);
}

inline Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    if(this != &other)
    {
        if(this->_device != VK_NULL_HANDLE)
        {
            if(this->_mapped)
                vkUnmapMemory(_device, _memory);

            if(this->_buffer != VK_NULL_HANDLE)
                vkDestroyBuffer(_device, _buffer, nullptr);

            if(this->_memory != VK_NULL_HANDLE)
                vkFreeMemory(_device, _memory, nullptr);
        }

        __MOVE_AND_NULL(_device, _buffer, _memory, _mapped);
        __MOVE_AND_ZERO(_size, _device_address);
    }

    return *this;
}

// ============================================================================
//  Image implementation
// ============================================================================

inline Image::Image(VkDevice device,
                    VkPhysicalDevice physical_device,
                    const Image::CreateInfo& info) : _device(device),
                                                   _format(info.format),
                                                   _extent(info.extent)
{
    __LOG_TRACE("Image: Initializing Image (device: " __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(device));

    VkImageCreateInfo create_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    create_info.imageType = info.type;
    create_info.format = info.format;
    create_info.extent = info.extent;
    create_info.mipLevels = info.mip_levels;
    create_info.arrayLayers = info.array_layers;
    create_info.samples = info.samples;
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage = info.usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    PROSERPINE_VK_CHECK_VOID(vkCreateImage(this->_device, &create_info, nullptr, &this->_image),
                             "Failed to create image");

    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(this->_device, this->_image, &mem_req);

    VkMemoryAllocateInfo alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc.allocationSize  = mem_req.size;
    alloc.memoryTypeIndex = find_memory_type(physical_device,
                                             mem_req.memoryTypeBits,
                                             info.memory_flags);

    PROSERPINE_VK_CHECK_VOID(vkAllocateMemory(this->_device, &alloc, nullptr, &this->_memory),
                             "Failed to allocate image memory");

    PROSERPINE_VK_CHECK_VOID(vkBindImageMemory(this->_device, this->_image, this->_memory, 0),
                             "Failed to bind image memory");

    VkImageViewCreateInfo view_create_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_create_info.image = this->_image;
    view_create_info.format = info.format;
    view_create_info.subresourceRange.aspectMask = info.aspect_mask;
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = info.mip_levels;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = info.array_layers;

    if(info.type == VK_IMAGE_TYPE_1D)
        view_create_info.viewType = info.array_layers > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
    else if (info.type == VK_IMAGE_TYPE_2D)
        view_create_info.viewType = info.array_layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
    else
        view_create_info.viewType = VK_IMAGE_VIEW_TYPE_3D;

    if(info.format == VK_FORMAT_D16_UNORM ||
       info.format == VK_FORMAT_D32_SFLOAT ||
       info.format == VK_FORMAT_D16_UNORM_S8_UINT ||
       info.format == VK_FORMAT_D24_UNORM_S8_UINT ||
       info.format == VK_FORMAT_D32_SFLOAT_S8_UINT)
        view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    PROSERPINE_VK_CHECK_VOID(vkCreateImageView(_device, &view_create_info, nullptr, &_view),
                             "Failed to create image view");
}

inline Image::~Image()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("Image: Destroying Image (device: " __FMT_U64H ")",
                    reinterpret_cast<std::uint64_t>(this->_device));

        if(this->_view != VK_NULL_HANDLE)
            vkDestroyImageView(this->_device, this->_view, nullptr);

        if(this->_image != VK_NULL_HANDLE)
            vkDestroyImage(this->_device, this->_image, nullptr);

        if(this->_memory != VK_NULL_HANDLE)
            vkFreeMemory(this->_device, this->_memory, nullptr);
    }
}

inline Image::Image(Image&& other) noexcept
{
    __MOVE_AND_NULL(_device, _image, _view, _memory);
    __MOVE(_format, _extent);
}

inline Image& Image::operator=(Image&& other) noexcept
{
    if(this != &other)
    {
        if(this->_device != VK_NULL_HANDLE)
        {
            if(this->_view   != VK_NULL_HANDLE)
                vkDestroyImageView(this->_device, this->_view, nullptr);

            if(this->_image  != VK_NULL_HANDLE)
                vkDestroyImage(this->_device, this->_image, nullptr);

            if(this->_memory != VK_NULL_HANDLE)
                vkFreeMemory(this->_device, this->_memory, nullptr);
        }

        __MOVE_AND_NULL(_device, _image, _view, _memory);
        __MOVE(_format, _extent);
    }

    return *this;
}

// ============================================================================
//  StagingBufferManager implementation
// ============================================================================

inline StagingBufferManager::StagingBufferManager(VkDevice device,
                                                  VkPhysicalDevice physical_device,
                                                  VkQueue transfer_queue,
                                                  std::uint32_t transfer_family,
                                                  VkDeviceSize capacity) : _device(device),
                                                                           _physical_device(physical_device),
                                                                           _transfer_queue(transfer_queue),
                                                                           _transfer_family(transfer_family),
                                                                           _capacity(capacity)
{
    __LOG_TRACE("StagingBufferManager: Initializing StagingBufferManager (device: " __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(device));

    VkBufferCreateInfo create_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    create_info.size = capacity;
    create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    PROSERPINE_VK_CHECK_VOID(vkCreateBuffer(this->_device, &create_info, nullptr, &this->_staging_buffer),
                             "Failed to create staging buffer");

    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(this->_device, this->_staging_buffer, &mem_req);

    VkMemoryAllocateInfo alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc.allocationSize  = mem_req.size;
    alloc.memoryTypeIndex = find_memory_type(this->_physical_device, mem_req.memoryTypeBits,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    PROSERPINE_VK_CHECK_VOID(vkAllocateMemory(this->_device, &alloc, nullptr, &this->_staging_memory),
                             "Failed to allocate staging memory");
    PROSERPINE_VK_CHECK_VOID(vkBindBufferMemory(this->_device, this->_staging_buffer, this->_staging_memory, 0),
                             "Failed to bind staging memory");

    vkMapMemory(this->_device, this->_staging_memory, 0, capacity, 0, &this->_staging_mapped);

    VkCommandPoolCreateInfo pool_create_info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_create_info.queueFamilyIndex = this->_transfer_family;
    PROSERPINE_VK_CHECK_VOID(vkCreateCommandPool(this->_device, &pool_create_info, nullptr, &this->_command_pool),
                             "Failed to create staging command pool");

    VkFenceCreateInfo fence_create_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    PROSERPINE_VK_CHECK_VOID(vkCreateFence(_device, &fence_create_info, nullptr, &this->_fence),
                             "Failed to create staging fence");
}

inline StagingBufferManager::~StagingBufferManager()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("StagingBufferManager: Destroying StagingBufferManager (device: " __FMT_U64H ")",
                    reinterpret_cast<std::uint64_t>(this->_device));

        if(!this->_pending.empty())
            this->flush();

        if(this->_staging_mapped)
            vkUnmapMemory(this->_device, this->_staging_memory);

        if(this->_fence != VK_NULL_HANDLE)
            vkDestroyFence(this->_device, this->_fence, nullptr);

        if(this->_command_pool != VK_NULL_HANDLE)
            vkDestroyCommandPool(this->_device, this->_command_pool, nullptr);

        if(this->_staging_buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(this->_device, this->_staging_buffer, nullptr);

        if(this->_staging_memory != VK_NULL_HANDLE)
            vkFreeMemory(this->_device, this->_staging_memory, nullptr);
    }
}

inline StagingBufferManager::StagingBufferManager(StagingBufferManager&& other) noexcept
{
    __MOVE_AND_NULL(_device, _physical_device, _staging_buffer, _staging_memory, _staging_mapped, _command_pool, _fence);
    __MOVE(_transfer_queue, _transfer_family, _capacity, _used, _pending);
}

inline StagingBufferManager& StagingBufferManager::operator=(StagingBufferManager&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _physical_device, _staging_buffer, _staging_memory, _staging_mapped, _command_pool, _fence);
        __MOVE(_transfer_queue, _transfer_family, _capacity, _used, _pending);
    }

    return *this;
}

inline void StagingBufferManager::upload_to_buffer(const void* data,
                                                   VkDeviceSize size,
                                                   VkBuffer dst,
                                                   VkDeviceSize dst_offset)
{
    __LOG_TRACE("StagingBufferManager: uploading to buffer");

    if(this->_used + size > this->_capacity)
        this->flush();

    VkDeviceSize offset = this->_used;
    std::memcpy(static_cast<char*>(this->_staging_mapped) + offset, data, size);
    this->_used += size;

    this->_pending.push_back({offset, size, [=, this](VkCommandBuffer cmd) {
        VkBufferCopy region{};
        region.srcOffset = offset;
        region.dstOffset = dst_offset;
        region.size = size;
        vkCmdCopyBuffer(cmd, this->_staging_buffer, dst, 1, &region);
    }});
}

inline void StagingBufferManager::upload_to_image(const void* data,
                                                  const VkExtent3D& extent,
                                                  VkFormat format,
                                                  VkImage dst,
                                                  VkImageSubresourceLayers layers)
{
    __LOG_TRACE("StagingBufferManager: uploading to image");

    // assumes tightly packed
    VkDeviceSize texel_size = 0;

    switch(format)
    {
        case VK_FORMAT_R8_UNORM: texel_size = 1; break;
        case VK_FORMAT_R8G8_UNORM: texel_size = 2; break;
        case VK_FORMAT_R8G8B8_UNORM: texel_size = 3; break;
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_B8G8R8A8_SRGB: texel_size = 4; break;
        case VK_FORMAT_R16G16B16A16_SFLOAT: texel_size = 8; break;
        case VK_FORMAT_R32G32B32A32_SFLOAT: texel_size = 16; break;
        default: texel_size = 4; break;
    }

    VkDeviceSize size = extent.width * extent.height * extent.depth * texel_size;

    if(this->_used + size > this->_capacity)
        this->flush();

    VkDeviceSize offset = this->_used;
    std::memcpy(static_cast<char*>(this->_staging_mapped) + offset, data, size);
    this->_used += size;

    VkBuffer staging_buf = this->_staging_buffer;

    this->_pending.push_back({offset, size, [=](VkCommandBuffer cmd) {
        VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = dst;
        barrier.subresourceRange = {layers.aspectMask, layers.mipLevel, 1, layers.baseArrayLayer, layers.layerCount};
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(cmd,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        VkBufferImageCopy region{};
        region.bufferOffset = offset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource = layers;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = extent;

        vkCmdCopyBufferToImage(cmd,
                               staging_buf,
                               dst,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);
    }});
}

inline void StagingBufferManager::flush()
{
    // TODO: use context command buffer from immediate context

    if(this->_pending.empty())
        return;

    __LOG_TRACE("StagingBufferManager: Flushing (pending: " __FMT_U64 ")",
                this->_pending.size());

    VkCommandBufferAllocateInfo alloc{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc.commandPool = this->_command_pool;
    alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(this->_device, &alloc, &cmd);

    VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &begin);

    for(auto& pc : this->_pending)
        pc.record(cmd);

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd;

    vkResetFences(this->_device, 1, &this->_fence);
    vkQueueSubmit(this->_transfer_queue, 1, &submit_info, this->_fence);
    vkWaitForFences(this->_device, 1, &this->_fence, VK_TRUE, PROSERPINE_WAIT_INFINITE);

    vkFreeCommandBuffers(this->_device, this->_command_pool, 1, &cmd);

    this->_pending.clear();
    this->_used = 0;

    __LOG_TRACE("StagingBufferManager: Flushed");
}

// =============================================================================
//  Descriptor Pool implementation
// =============================================================================

inline DescriptorPool::DescriptorPool()
{
    __LOG_TRACE("DescriptorPool: Initializing DescriptorPool");
}

inline DescriptorPool::DescriptorPool(VkDevice device,
                                      DescriptorPool::CreateInfo& create_info) : _device(device)
{
    __LOG_TRACE("DescriptorPool: Initializing DescriptorPool (device: " __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(this->_device));

    VkDescriptorPoolSize default_pool_sizes[] = { { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, DescriptorPool::DEFAULT_SETS_SIZE } };

    VkDescriptorPoolCreateInfo info{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT |
                 VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    info.maxSets = create_info.max_sets;
    info.poolSizeCount = create_info.pool_size_count;
    info.pPoolSizes = create_info.sizes == nullptr ? default_pool_sizes : create_info.sizes;

    PROSERPINE_VK_CHECK_VOID(vkCreateDescriptorPool(this->_device, &info, nullptr, &this->_pool),
                             "Failed to create VkDescriptorPool");
}

inline DescriptorPool::~DescriptorPool()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("DescriptorPool: Destroying DescriptorPool (device: " __FMT_U64H ")",
                    reinterpret_cast<std::uint64_t>(this->_device));

        if(this->_pool != VK_NULL_HANDLE)
        {
            if(this->_sets.size() > 0)
            {
                __LOG_TRACE("DescriptorPool: Freeing " __FMT_U64 " descriptor sets",
                            this->_sets.size());

                vkFreeDescriptorSets(this->_device,
                                     this->_pool,
                                     static_cast<std::uint32_t>(this->_sets.size()),
                                     this->_sets.data());
            }

            vkDestroyDescriptorPool(this->_device, this->_pool, nullptr);
        }
    }
}

inline DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
{
    __MOVE_AND_NULL(_device, _pool);
}

inline DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _pool);
    }

    return *this;
}


inline Expected<DescriptorSet> DescriptorPool::allocate_descriptor_set(VkDescriptorSetLayout layout)
{
    __LOG_TRACE("DescriptorPool: Allocating a descriptor set (layout: " __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(layout));

    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", Error("Invalid Device"));
    PROSERPINE_ASSERT(this->_pool != VK_NULL_HANDLE, "DescriptorPool is NULL", Error("Invalid DescriptorPool"));

    VkDescriptorSetAllocateInfo create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    create_info.descriptorPool = this->_pool;
    create_info.descriptorSetCount = 1;
    create_info.pSetLayouts = &layout;

    VkDescriptorSet set;

    PROSERPINE_VK_CHECK(vkAllocateDescriptorSets(this->_device, &create_info, &set),
                        "Failed to allocate a descriptor set");

    this->_sets.push_back(set);

    return DescriptorSet(this->_device, set);
}

// =============================================================================
//  Descriptor Set implementation
// =============================================================================

inline DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
{
    __MOVE_AND_NULL(_device, _set);
    __MOVE(_writes, _buffer_infos);
}

inline DescriptorSet::~DescriptorSet()
{
}

inline DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _set);
        __MOVE(_writes, _buffer_infos);
    }

    return *this;
}

inline DescriptorSet& DescriptorSet::write(std::uint32_t binding,
                                           const Buffer& buffer,
                                           VkDescriptorType type,
                                           VkDeviceSize offset,
                                           VkDeviceSize range)
{
    __LOG_TRACE("Adding a Write Descriptor Set: binding: " __FMT_U32 ", type: " __FMT_U32 "",
                binding,
                static_cast<std::uint32_t>(type));

    this->_buffer_infos.push_back({ buffer.handle(), offset, range });

    this->_writes.push_back({ binding, type, this->_buffer_infos.size() - 1 });

    return *this;
}

inline DescriptorSet& DescriptorSet::update()
{
    __LOG_TRACE("Updating Descriptor Set (" __FMT_U64 " writes)",
                this->_writes.size());

    std::vector<VkWriteDescriptorSet> writes;

    for(const auto pending_write : this->_writes)
    {
        VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = this->_set;
        write.dstBinding = pending_write.binding;
        write.descriptorCount = 1;
        write.descriptorType = pending_write.type;
        write.pBufferInfo = &this->_buffer_infos.at(pending_write.buffer_index);

        __LOG_TRACE("Updating Write Descriptor Set: binding: " __FMT_U32 ", type: " __FMT_U32 "",
                    pending_write.binding,
                    static_cast<std::uint32_t>(pending_write.type));

        writes.push_back(write);
    }

    vkUpdateDescriptorSets(this->_device,
                           static_cast<std::uint32_t>(writes.size()),
                           writes.data(),
                           0,
                           nullptr);

    this->_writes.clear();
    this->_buffer_infos.clear();

    return *this;
}

// =============================================================================
//  Command Buffer implementation
// =============================================================================

inline CommandBuffer::CommandBuffer(VkDevice device,
                                    VkCommandPool pool,
                                    CommandBuffer::Level level) : _device(device),
                                                                  _pool(pool),
                                                                  _state(State::Initial)
{
    VkCommandBufferAllocateInfo info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    info.commandBufferCount = 1;
    info.commandPool = pool;

    if(level == CommandBuffer::Level::Primary)
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    else
        info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

    PROSERPINE_VK_CHECK_VOID(vkAllocateCommandBuffers(this->_device, &info, &this->_buffer),
                             "Failed to allocate command buffer");

    this->_state = State::Initial;
}

inline CommandBuffer::~CommandBuffer()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("CommandBuffer: Destroying");

        vkFreeCommandBuffers(this->_device, this->_pool, 1, &this->_buffer);
    }
}

inline CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
{
    __MOVE_AND_NULL(_device, _pool, _buffer);
    __MOVE(_state);
}

inline CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _pool, _buffer);
        __MOVE(_state);
    }

    return *this;
}

inline void CommandBuffer::reset()
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);

    this->_state = State::Initial;
    vkResetCommandBuffer(this->_buffer, 0);
}

inline void CommandBuffer::begin(VkCommandBufferUsageFlags flags)
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Initial, "CommandBuffer is not in initial state",);

    this->_state = State::Recording;

    VkCommandBufferBeginInfo info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    info.flags = flags;

    PROSERPINE_VK_CHECK_VOID(vkBeginCommandBuffer(this->_buffer, &info),
                             "Failed to begin CommandBuffer recording");
}

inline void CommandBuffer::end()
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    this->_state = State::Executable;

    PROSERPINE_VK_CHECK_VOID(vkEndCommandBuffer(this->_buffer),
                             "Failed to end CommandBuffer recording");
}

inline void CommandBuffer::transition_image(VkImage image,
                                            VkImageLayout old_layout,
                                            VkImageLayout new_layout,
                                            VkImageAspectFlags aspect)
{
    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;

    if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
       new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
            new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
            new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
            new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
            new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;
        src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dst_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
            new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(old_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
            new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        PROSERPINE_ASSERT(false, "Unsupported layout transition",);
    }

    vkCmdPipelineBarrier(this->_buffer,
                         src_stage,
                         dst_stage,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);
}

inline void CommandBuffer::begin_render(const RenderingInfo& info)
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    std::vector<VkRenderingAttachmentInfo> colors;
    colors.reserve(info.color_attachments.size());

    for(const auto& c : info.color_attachments)
    {
        VkRenderingAttachmentInfo ra_info{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        ra_info.imageView = c.view;
        ra_info.imageLayout = c.layout;
        ra_info.loadOp = c.load_op;
        ra_info.storeOp = c.store_op;
        ra_info.clearValue = c.clear;

        colors.push_back(ra_info);
    }

    VkRenderingAttachmentInfo depth{};

    if(info.depth_attachment)
    {
        const auto depth_attachment = info.depth_attachment.value();
        depth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth.imageView = depth_attachment.view;
        depth.imageLayout = depth_attachment.layout;
        depth.loadOp = depth_attachment.load_op;
        depth.storeOp = depth_attachment.store_op;
        depth.clearValue = depth_attachment.clear;
    }

    VkRenderingInfo render_info{VK_STRUCTURE_TYPE_RENDERING_INFO};
    render_info.renderArea = info.render_area;
    render_info.layerCount = info.layer_count;
    render_info.colorAttachmentCount = static_cast<std::int32_t>(colors.size());
    render_info.pColorAttachments = colors.data();

    if(info.depth_attachment)
        render_info.pDepthAttachment = &depth;

    __LOG_TRACE("CommandBuffer: Beginning render (color attachments: " __FMT_U32 ", depth attachment: " __FMT_BOOL ")",
                static_cast<std::uint32_t>(info.color_attachments.size()),
                static_cast<bool>(info.depth_attachment));

    vkCmdBeginRendering(this->_buffer, &render_info);
}

inline void CommandBuffer::end_render()
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    __LOG_TRACE("CommandBuffer: Ending render");

    vkCmdEndRendering(this->_buffer);
}

inline void CommandBuffer::bind_graphics_pipeline(VkPipeline pipeline)
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    vkCmdBindPipeline(this->_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

inline void CommandBuffer::bind_compute_pipeline(VkPipeline pipeline)
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    vkCmdBindPipeline(this->_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

inline void CommandBuffer::set_viewport(float width,
                                        float height,
                                        float x,
                                        float y,
                                        float min_depth,
                                        float max_depth)
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    VkViewport viewport{};
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = min_depth;
    viewport.maxDepth = max_depth;

    vkCmdSetViewport(this->_buffer, 0, 1, &viewport);
}

inline void CommandBuffer::set_scissor(std::uint32_t width,
                                       std::uint32_t height,
                                       std::int32_t offset_x,
                                       std::int32_t offset_y)
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    VkRect2D scissor{};
    scissor.offset = { offset_x, offset_y };
    scissor.extent = { width, height };

    vkCmdSetScissor(this->_buffer, 0, 1, &scissor);
}

inline void CommandBuffer::draw(std::uint32_t vertex_count,
                                std::uint32_t instance_count,
                                std::uint32_t first_vertex,
                                std::uint32_t first_instance)
{
    PROSERPINE_ASSERT(this->_buffer != VK_NULL_HANDLE, "buffer is NULL",);
    PROSERPINE_ASSERT(this->_state == State::Recording, "CommandBuffer is not in recording state",);

    __LOG_TRACE("CommandBuffer: drawcall (" __FMT_U32 "," __FMT_U32 "," __FMT_U32 "," __FMT_U32 ")",
                vertex_count, instance_count, first_vertex, first_instance);

    vkCmdDraw(this->_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

// =============================================================================
//  Command Pool implementation
// =============================================================================

inline CommandPool::CommandPool(VkDevice device, std::uint32_t queue_family_index) : _device(device)
{
    __LOG_TRACE("CommandPool: Initializing");

    VkCommandPoolCreateInfo info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    info.queueFamilyIndex = queue_family_index;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    PROSERPINE_VK_CHECK_VOID(vkCreateCommandPool(this->_device, &info, nullptr, &this->_pool),
                             "Failed to create CommandPool");
}

inline CommandPool::~CommandPool()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("CommandPool: Destroying");

        if(this->_pool != VK_NULL_HANDLE)
            vkDestroyCommandPool(this->_device, this->_pool, nullptr);
    }
}

inline CommandPool::CommandPool(CommandPool&& other) noexcept
{
    __MOVE_AND_NULL(_device, _pool);
}

inline CommandPool& CommandPool::operator=(CommandPool&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _pool);
    }

    return *this;
}

inline CommandBuffer CommandPool::allocate(CommandBuffer::Level level)
{
    return CommandBuffer(this->_device, this->_pool, level);
}

// =============================================================================
//  Command Profiler implementation
// =============================================================================

inline CommandProfiler::CommandProfiler(VkDevice device, VkPhysicalDevice physical_device) : _device(device)
{
    __LOG_TRACE("CommandProfiler: Initializing");

    PROSERPINE_ASSERT(device != VK_NULL_HANDLE, "Device is NULL", );
    PROSERPINE_ASSERT(physical_device != VK_NULL_HANDLE, "Physical Device is NULL", );

    VkQueryPoolCreateInfo create_info{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    create_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
    create_info.queryCount = 2;

    PROSERPINE_VK_CHECK_VOID(vkCreateQueryPool(this->_device, &create_info, nullptr, &this->_pool),
                             "Failed to create Query Pool");

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    this->_timestamp_period = static_cast<double>(properties.limits.timestampPeriod);

    __LOG_TRACE("CommandProfiler: Initialized (timestamp period: " __FMT_FLT32 ")",
                this->_timestamp_period);
}

inline CommandProfiler::~CommandProfiler()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("CommandProfiler: Destroying");

        if(this->_pool != VK_NULL_HANDLE)
            vkDestroyQueryPool(this->_device, this->_pool, nullptr);
    }
}

inline CommandProfiler::CommandProfiler(CommandProfiler&& other) noexcept
{
    __MOVE_AND_NULL(_device, _pool);
}

inline CommandProfiler& CommandProfiler::operator=(CommandProfiler&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device, _pool);
    }

    return *this;
}

inline void CommandProfiler::start(VkCommandBuffer cmd) noexcept
{
    __LOG_TRACE("CommandProfiler: Writing start timestamp");

    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", );
    PROSERPINE_ASSERT(this->_pool != VK_NULL_HANDLE, "QueryPool is NULL",);

    vkCmdResetQueryPool(cmd, this->_pool, 0, 2);
    vkCmdWriteTimestamp(cmd,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        this->_pool,
                        0);
}

inline void CommandProfiler::end(VkCommandBuffer cmd) noexcept
{
    __LOG_TRACE("CommandProfiler: Writing end timestamp");

    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", );
    PROSERPINE_ASSERT(this->_pool != VK_NULL_HANDLE, "QueryPool is NULL",);

    vkCmdWriteTimestamp(cmd,
                        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                        this->_pool,
                        1);
}

inline double CommandProfiler::elapsed(ElapsedUnit unit) noexcept
{
    __LOG_TRACE("CommandProfiler: Querying results");

    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE, "Device is NULL", CommandProfiler::INVALID_MEASURE);
    PROSERPINE_ASSERT(this->_pool != VK_NULL_HANDLE, "QueryPool is NULL", CommandProfiler::INVALID_MEASURE);

    std::uint64_t timestamps[2];
    vkGetQueryPoolResults(this->_device,
                          this->_pool,
                          0,
                          2,
                          sizeof(timestamps),
                          timestamps,
                          sizeof(std::uint64_t),
                          VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

    double divisor = 1.0f;

    switch(unit)
    {
        case CommandProfiler::ElapsedUnit::Milliseconds: divisor = 1e6; break;
        case CommandProfiler::ElapsedUnit::Microseconds: divisor = 1e3; break;
        case CommandProfiler::ElapsedUnit::Seconds: divisor = 1e9; break;
        default: break;
    }

    return static_cast<double>(timestamps[1] - timestamps[0]) * this->_timestamp_period / divisor;
}

// =============================================================================
//  ShaderModule implementation (with minimal SPIR-V reflection)
// =============================================================================

inline Expected<SPIRVByteCode> load_spirv_file(const char* file_path) noexcept
{
    __LOG_TRACE("Loading a SPIR-V bytecode file (" __FMT_STR ")",
                file_path);

    SPIRVByteCode spirv;

    std::FILE* file = std::fopen(file_path, "rb");

    if(file == nullptr)
        return Error("Cannot open SPIR-V bytecode file");

    std::fseek(file, 0, SEEK_END);
    std::size_t file_sz = std::ftell(file);
    std::rewind(file);

    if(file_sz == 0)
        return Error("SPIR-V bytecode file is empty");

    spirv.resize(file_sz / 4);
    std::memset(spirv.data(), 0, spirv.size() * sizeof(std::uint32_t));

    if(std::fread(spirv.data(), sizeof(std::uint8_t), file_sz, file) != file_sz)
        return Error("Error during read of SPIR-V bytecode file");

    if(std::ferror(file))
        return Error("Error during read of SPIR-V bytecode file");

    __LOG_TRACE("Loaded SPIR-V bytecode file (" __FMT_STR ")",
                file_path);

    return spirv;
}

inline ShaderModule::ShaderModule()
{
    __LOG_TRACE("ShaderModule: Initializing ShaderModule");
}

inline Expected<ShaderModule> ShaderModule::create_compute_shader(VkDevice device,
                                                                  const char* path)
{
    const auto spirv = load_spirv_file(path);

    if(spirv.has_error())
        return spirv.error();

    return ShaderModule::create(device, spirv.value(), VK_SHADER_STAGE_COMPUTE_BIT);
}

inline Expected<ShaderModule> ShaderModule::create_vertex_shader(VkDevice device,
                                                                 const char* path)
{
    const auto spirv = load_spirv_file(path);

    if(spirv.has_error())
        return spirv.error();

    return ShaderModule::create(device, spirv.value(), VK_SHADER_STAGE_VERTEX_BIT);
}

inline Expected<ShaderModule> ShaderModule::create_fragment_shader(VkDevice device,
                                                                   const char* path)
{
    const auto spirv = load_spirv_file(path);

    if(spirv.has_error())
        return spirv.error();

    return ShaderModule::create(device, spirv.value(), VK_SHADER_STAGE_FRAGMENT_BIT);
}

inline Expected<ShaderModule> ShaderModule::create(VkDevice device,
                                                   const std::vector<std::uint32_t>& spirv,
                                                   VkShaderStageFlagBits stage)
{
    __LOG_TRACE("ShaderModule: Creating a new shader module (" __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(device));

    ShaderModule sm;
    sm._device = device;
    sm._stage  = stage;

    VkShaderModuleCreateInfo ci{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    ci.codeSize = spirv.size() * sizeof(std::uint32_t);
    ci.pCode = spirv.data();

    VkResult result = vkCreateShaderModule(device, &ci, nullptr, &sm._module);

    if(result != VK_SUCCESS)
    {
        __LOG_ERROR("ShaderModule: Failed to create a new shader module");
        return Error(result, "Failed to create shader module");
    }

    sm.reflect_spirv(spirv);

    __LOG_TRACE("ShaderModule: creatded a new shader module");

    return sm;
}

inline ShaderModule::~ShaderModule()
{

    if(this->_module != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("ShaderModule: Destroying ShaderModule");
        vkDestroyShaderModule(this->_device, this->_module, nullptr);
    }
}

inline ShaderModule::ShaderModule(ShaderModule&& other) noexcept : _stage(other._stage)
{
    __MOVE_AND_NULL(_device, _module);
    __MOVE(_set_layouts, _push_ranges, _vertex_inputs);
}

inline ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept
{
    if(this != &other)
    {
        if(this->_module != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
            vkDestroyShaderModule(this->_device, this->_module, nullptr);

        __MOVE_AND_NULL(_device, _module);
        __MOVE(_set_layouts, _push_ranges, _vertex_inputs);

        this->_stage = other._stage;
    }

    return *this;
}

// Minimal SPIR-V parser for descriptor and push constant reflection.
// This avoids an external dependency on spirv-reflect.
// It reads OpDecorate, OpMemberDecorate, OpVariable, and type instructions.
inline void ShaderModule::reflect_spirv(const std::vector<std::uint32_t>& spirv)
{
    __LOG_TRACE("ShaderModule: Reflecting SPIR-V");

    if(spirv.size() < 5)
    {
        __LOG_TRACE("ShaderModule: SPIR-V code size less than 5");
        return;
    }

    // SPIR-V magic check
    if(spirv[0] != 0x07230203)
    {
        __LOG_TRACE("ShaderModule: Unexpected SPIR-V header magic " __FMT_U32 "",
                    spirv[0]);
        return;
    }

    // We'll do a simplified pass collecting descriptorSet/binding decorations
    // and variable declarations

    struct DecorationInfo
    {
        std::uint32_t id = 0;
        std::uint32_t descriptor_set = std::numeric_limits<std::uint32_t>::max();
        std::uint32_t binding = std::numeric_limits<std::uint32_t>::max();
        std::uint32_t location = std::numeric_limits<std::uint32_t>::max();
        std::uint32_t offset = std::numeric_limits<std::uint32_t>::max();
    };

    struct VariableInfo
    {
        std::uint32_t id = 0;
        std::uint32_t type_id = 0;
        std::uint32_t storage_class = 0;
    };

    struct TypeInfo
    {
        std::uint32_t id = 0;
        std::uint32_t opcode = 0;
        std::uint32_t element_type = 0; // for pointer/array types
        std::uint32_t array_length = 0;
    };

    std::vector<DecorationInfo> decorations;
    std::vector<VariableInfo> variables;
    std::vector<TypeInfo> types;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> constants; // id -> value

    // Opcodes we care about
    constexpr std::uint32_t SpvOpDecorate = 71;
    constexpr std::uint32_t SpvOpVariable = 59;
    constexpr std::uint32_t SpvOpTypePointer = 32;
    constexpr std::uint32_t SpvOpTypeImage = 25;
    constexpr std::uint32_t SpvOpTypeSampler = 26;
    constexpr std::uint32_t SpvOpTypeSampledImage = 27;
    constexpr std::uint32_t SpvOpTypeStruct = 30;
    constexpr std::uint32_t SpvOpTypeArray = 28;
    constexpr std::uint32_t SpvOpTypeRuntimeArray = 29;
    constexpr std::uint32_t SpvOpConstant = 43;

    constexpr std::uint32_t SpvDecorationDescriptorSet = 34;
    constexpr std::uint32_t SpvDecorationBinding = 33;
    constexpr std::uint32_t SpvDecorationLocation = 30;
    constexpr std::uint32_t SpvDecorationOffset = 35;

    constexpr std::uint32_t SpvStorageClassUniformConstant = 0;
    constexpr std::uint32_t SpvStorageClassUniform = 2;
    constexpr std::uint32_t SpvStorageClassStorageBuffer = 12;
    constexpr std::uint32_t SpvStorageClassInput = 1;
    constexpr std::uint32_t SpvStorageClassPushConstant = 9;

    auto find_or_add_decoration = [&](std::uint32_t id) -> DecorationInfo& {
        for(auto& d : decorations)
            if(d.id == id)
                return d;

        decorations.push_back({id});

        return decorations.back();
    };

    std::size_t i = 5; // skip header

    while(i < spirv.size())
    {
        std::uint32_t word = spirv[i];
        std::uint32_t opcode = word & 0xFFFF;
        std::uint32_t length = word >> 16;

        if(length == 0)
            break;

        if(opcode == SpvOpDecorate && length >= 4)
        {
            std::uint32_t target = spirv[i + 1];
            std::uint32_t decoration = spirv[i + 2];
            std::uint32_t value = spirv[i + 3];

            auto& d = find_or_add_decoration(target);

            if(decoration == SpvDecorationDescriptorSet)
                d.descriptor_set = value;
            else if(decoration == SpvDecorationBinding)
                d.binding = value;
            else if(decoration == SpvDecorationLocation)
                d.location = value;
            else if(decoration == SpvDecorationOffset)
                d.offset = value;
        }
        else if(opcode == SpvOpVariable && length >= 4)
        {
            std::uint32_t type_id = spirv[i + 1];
            std::uint32_t result_id = spirv[i + 2];
            std::uint32_t storage_class = spirv[i + 3];

            variables.push_back({result_id, type_id, storage_class});
        }
        else if (opcode == SpvOpTypePointer && length >= 4)
        {
            types.push_back({spirv[i + 1], opcode, spirv[i + 3], 0});
        }
        else if (opcode == SpvOpTypeSampledImage && length >= 3)
        {
            types.push_back({spirv[i + 1], opcode, spirv[i + 2], 0});
        }
        else if (opcode == SpvOpTypeImage && length >= 3)
        {
            types.push_back({spirv[i + 1], opcode, 0, 0});
        }
        else if (opcode == SpvOpTypeSampler && length >= 2)
        {
            types.push_back({spirv[i + 1], opcode, 0, 0});
        }
        else if (opcode == SpvOpTypeStruct)
        {
            types.push_back({spirv[i + 1], opcode, 0, 0});
        }
        else if (opcode == SpvOpTypeArray && length >= 4)
        {
            types.push_back({spirv[i + 1], opcode, spirv[i + 2], spirv[i + 3]});
        }
        else if (opcode == SpvOpTypeRuntimeArray && length >= 3)
        {
            types.push_back({spirv[i + 1], opcode, spirv[i + 2], 0});
        }
        else if (opcode == SpvOpConstant && length >= 4)
        {
            constants.push_back({spirv[i + 2], spirv[i + 3]});
        }

        i += length;
    }

    auto find_type = [&](std::uint32_t id) -> const TypeInfo* {
        for(auto& t : types)
            if(t.id == id)
                return &t;

        return nullptr;
    };

    auto find_constant = [&](std::uint32_t id) -> std::uint32_t {
        for(auto& c : constants)
            if(c.first == id)
                return c.second;

        return 1;
    };

    // Resolve pointer types to their pointed-to type
    auto resolve_pointer = [&](std::uint32_t type_id) -> const TypeInfo* {
        auto* t = find_type(type_id);

        if(t && t->opcode == SpvOpTypePointer)
            return find_type(t->element_type);

        return t;
    };

    auto infer_descriptor_type = [&](std::uint32_t type_id, std::uint32_t storage_class) -> VkDescriptorType {
        auto* pointed = resolve_pointer(type_id);

        if(!pointed)
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        if(pointed->opcode == SpvOpTypeSampledImage)
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        if(pointed->opcode == SpvOpTypeImage)
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        if(pointed->opcode == SpvOpTypeSampler)
            return VK_DESCRIPTOR_TYPE_SAMPLER;

        if(storage_class == SpvStorageClassStorageBuffer)
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        if(storage_class == SpvStorageClassUniform)
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        if(storage_class == SpvStorageClassUniformConstant)
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    };

    // Build descriptor set layouts and push constant ranges
    std::vector<DescriptorSetLayoutInfo> sets;

    auto find_or_add_set = [&](std::uint32_t set_number) -> DescriptorSetLayoutInfo& {
        for(auto& s : sets)
            if(s.set == set_number)
                return s;

        sets.push_back({set_number, {}});

        return sets.back();
    };

    for(auto& var : variables)
    {
        DecorationInfo* dec = nullptr;

        for(auto& d : decorations)
        {
            if(d.id == var.id)
            {
                dec = &d;
                break;
            }
        }

        if(var.storage_class == SpvStorageClassPushConstant)
        {
            auto* pointed = resolve_pointer(var.type_id);
            PushConstantRange range;
            range.stage_flags = this->_stage;
            range.offset = 0;
            range.size = 128; // TODO: find better default value
            this->_push_ranges.push_back(range);
            continue;
        }

        if(var.storage_class == SpvStorageClassInput && dec && dec->location != UINT32_MAX)
        {
            VertexInputAttribute attr;
            attr.location = dec->location;
            attr.binding  = 0;
            attr.format = VK_FORMAT_R32G32B32A32_SFLOAT; // TODO: need type walk
            attr.offset = 0;

            this->_vertex_inputs.push_back(attr);

            continue;
        }

        if(!dec || dec->descriptor_set == UINT32_MAX || dec->binding == UINT32_MAX)
            continue;

        auto& set = find_or_add_set(dec->descriptor_set);
        DescriptorBindingInfo binding;
        binding.binding = dec->binding;
        binding.descriptor_type = infer_descriptor_type(var.type_id, var.storage_class);
        binding.descriptor_count = 1;
        binding.stage_flags = this->_stage;

        // Check for array types
        auto* pointed = resolve_pointer(var.type_id);

        if(pointed && pointed->opcode == SpvOpTypeArray)
        {
            binding.descriptor_count = find_constant(pointed->array_length);
        }
        else if(pointed && pointed->opcode == SpvOpTypeRuntimeArray)
        {
            binding.descriptor_count = 1024; // variable count
            binding.variable_count = true;
        }

        set.bindings.push_back(binding);
    }

    this->_set_layouts = std::move(sets);

    __LOG_TRACE("ShaderModule: SPIR-V reflection successful (sets: " __FMT_U64 ")",
                this->_set_layouts.size());
}

// =============================================================================
//  PipelineLayoutBuilder implementation
// =============================================================================

inline PipelineLayoutBuilder& PipelineLayoutBuilder::add_set(std::uint32_t set,
                                                             const DescriptorSetLayoutInfo& info)
{
    __LOG_TRACE("PipelineLayoutBuilder: Added a new set (" __FMT_U32 ")",
                info.set);

    this->_sets.push_back({set, info});

    return *this;
}

inline PipelineLayoutBuilder& PipelineLayoutBuilder::add_push_constant_range(VkShaderStageFlags stages,
                                                                             std::uint32_t offset,
                                                                             std::uint32_t size)
{
    __LOG_TRACE("PipelineLayoutBuilder: Added a push constant range (" __FMT_U32 ", " __FMT_U32 ")",
                offset,
                size);

    VkPushConstantRange range;
    range.stageFlags = stages;
    range.offset = offset;
    range.size = size;

    this->_push_ranges.push_back(range);

    return *this;
}

inline Expected<PipelineLayout> PipelineLayoutBuilder::build()
{
    __LOG_TRACE("PipelineLayoutBuilder: Building a new pipeline layout");

    PipelineLayout pl;
    pl._device = this->_device;

    // Merge sets: find max set index
    std::uint32_t max_set = 0;

    for(auto& [set_idx, info] : this->_sets)
        if(set_idx > max_set)
            max_set = set_idx;

    pl._set_layouts.resize(max_set + 1, VK_NULL_HANDLE);

    std::vector<std::vector<DescriptorBindingInfo>> merged(max_set + 1);

    for(auto& [set_idx, info] : this->_sets)
    {
        for(auto& b : info.bindings)
        {
            // Check for duplicate bindings, merge stage flags
            bool found = false;

            for(auto& existing : merged[set_idx])
            {
                if(existing.binding == b.binding)
                {
                    existing.stage_flags |= b.stage_flags;
                    found = true;
                    break;
                }
            }

            if(!found)
                merged[set_idx].push_back(b);
        }
    }

    for(std::uint32_t s = 0; s <= max_set; ++s)
    {
        if(merged[s].empty())
        {
            // Create empty set layout
            VkDescriptorSetLayoutCreateInfo ci{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
            ci.bindingCount = 0;

            VkResult result = vkCreateDescriptorSetLayout(_device, &ci, nullptr, &pl._set_layouts[s]);

            if(result != VK_SUCCESS)
            {
                __LOG_ERROR("PipelineLayoutBuilder: Failed to create a new pipeline layout (failed to create empty descriptor set layout)");
                return Error(result, "Failed to create empty descriptor set layout");
            }

            continue;
        }

        std::vector<VkDescriptorSetLayoutBinding> vk_bindings;
        std::vector<VkDescriptorBindingFlags> binding_flags;
        bool has_variable_count = false;

        for(auto& b : merged[s])
        {
            VkDescriptorSetLayoutBinding vk_b{};
            vk_b.binding = b.binding;
            vk_b.descriptorType = b.descriptor_type;
            vk_b.descriptorCount = b.descriptor_count;
            vk_b.stageFlags = b.stage_flags;
            vk_bindings.push_back(vk_b);

            VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                                             VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

            if(b.variable_count)
            {
                flags |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
                has_variable_count = true;
            }

            binding_flags.push_back(flags);
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo flags_create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO};
        flags_create_info.bindingCount  = static_cast<std::uint32_t>(binding_flags.size());
        flags_create_info.pBindingFlags = binding_flags.data();

        VkDescriptorSetLayoutCreateInfo sl_create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        sl_create_info.pNext = &flags_create_info;
        sl_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
        sl_create_info.bindingCount = static_cast<std::uint32_t>(vk_bindings.size());
        sl_create_info.pBindings = vk_bindings.data();

        VkResult result = vkCreateDescriptorSetLayout(this->_device, &sl_create_info, nullptr, &pl._set_layouts[s]);

        if(result != VK_SUCCESS)
        {
            for(std::uint32_t j = 0; j < s; ++j)
            {
                if(pl._set_layouts[j] != VK_NULL_HANDLE)
                    vkDestroyDescriptorSetLayout(this->_device, pl._set_layouts[j], nullptr);
            }

            __LOG_ERROR("PipelineLayoutBuilder: Failed to create a new pipeline layout (failed to create the descriptor set layout)");

            return Error(result, "Failed to create descriptor set layout");
        }
    }

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pl_create_info{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pl_create_info.setLayoutCount = static_cast<std::uint32_t>(pl._set_layouts.size());
    pl_create_info.pSetLayouts = pl._set_layouts.data();
    pl_create_info.pushConstantRangeCount = static_cast<std::uint32_t>(this->_push_ranges.size());
    pl_create_info.pPushConstantRanges = this->_push_ranges.data();

    VkResult result = vkCreatePipelineLayout(this->_device, &pl_create_info, nullptr, &pl._layout);

    if(result != VK_SUCCESS)
    {
        for(auto sl : pl._set_layouts)
            if (sl != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(this->_device, sl, nullptr);

        __LOG_ERROR("PipelineLayoutBuilder: Failed to create a new pipeline layout");

        return Error(result, "Failed to create pipeline layout");
    }

    __LOG_TRACE("PipelineLayoutBuilder: Built a new pipeline layout");

    return pl;
}

// =============================================================================
//  PipelineLayout destructor / move
// =============================================================================

inline PipelineLayout::PipelineLayout()
{
    __LOG_TRACE("PipelineLayout: Initializing Pipeline Layout");
}

inline PipelineLayout::~PipelineLayout()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("PipelineLayout: Destroying Pipeline Layout");

        if(this->_layout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(this->_device, this->_layout, nullptr);

        for(auto sl : this->_set_layouts)
            if(sl != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(this->_device, sl, nullptr);
    }
}

inline PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
{
    __MOVE_AND_NULL(_device, _layout);
    __MOVE(_set_layouts);
}

inline PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other) noexcept
{
    if(this != &other)
    {
        if(this->_device != VK_NULL_HANDLE)
        {
            if(this->_layout != VK_NULL_HANDLE)
                vkDestroyPipelineLayout(this->_device, this->_layout, nullptr);

            for(auto sl : this->_set_layouts)
                if(sl != VK_NULL_HANDLE)
                    vkDestroyDescriptorSetLayout(this->_device, sl, nullptr);
        }

        __MOVE_AND_NULL(_device, _layout);
        __MOVE(_set_layouts);
    }

    return *this;
}

// =============================================================================
//  ShaderStages helper
// =============================================================================

inline ShaderStages& ShaderStages::add(const ShaderModule& mod, const char* entry)
{
    __LOG_TRACE("Adding a new ShaderStage entry: " __FMT_STR " (" __FMT_U64H ")",
                entry,
                reinterpret_cast<std::uint64_t>(mod.handle()));

    VkPipelineShaderStageCreateInfo create_info{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    create_info.stage  = mod.stage();
    create_info.module = mod.handle();
    create_info.pName  = entry;

    stages.push_back(create_info);

    return *this;
}

// =============================================================================
//  GraphicsPipeline creation
// =============================================================================

inline Expected<GraphicsPipeline> create_graphics_pipeline(VkDevice device,
                                                           const GraphicsPipeline::CreateInfo& info,
                                                           const ShaderStages& stages,
                                                           const VertexInputState& vertex_input,
                                                           VkPipelineLayout layout)
{
    __LOG_TRACE("Creating a new graphics pipeline");

    GraphicsPipeline gp;
    gp._device = device;

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertex_input_ci{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertex_input_ci.vertexBindingDescriptionCount = static_cast<std::uint32_t>(vertex_input.bindings.size());
    vertex_input_ci.pVertexBindingDescriptions = vertex_input.bindings.data();
    vertex_input_ci.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertex_input.attributes.size());
    vertex_input_ci.pVertexAttributeDescriptions = vertex_input.attributes.data();

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    input_assembly.topology = info.topology;
    input_assembly.primitiveRestartEnable = info.primitive_restart_enable;

    // Viewport / scissor (dynamic)
    VkPipelineViewportStateCreateInfo viewport_state{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_state.viewportCount = info.viewport_count;
    viewport_state.scissorCount  = info.scissor_count;

    // Depth/stencil
    VkPipelineDepthStencilStateCreateInfo depth_stencil{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    if(info.depth_format != VK_FORMAT_UNDEFINED)
    {
        depth_stencil.depthTestEnable = info.depth_test_enable;
        depth_stencil.depthWriteEnable = info.depth_write_enable;
        depth_stencil.depthCompareOp = info.depth_compare_op;
    }

    // Color blend
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = info.color_write_mask;

    VkPipelineColorBlendStateCreateInfo color_blend{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &color_blend_attachment;

    // Dynamic states
    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;

    // Dynamic rendering
    VkPipelineRenderingCreateInfo rendering_ci{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    rendering_ci.colorAttachmentCount = 1;
    rendering_ci.pColorAttachmentFormats = &info.color_format;
    rendering_ci.depthAttachmentFormat = info.depth_format;

    VkGraphicsPipelineCreateInfo create_info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    create_info.pNext = &rendering_ci;
    create_info.stageCount = static_cast<std::uint32_t>(stages.stages.size());
    create_info.pStages = stages.stages.data();
    create_info.pVertexInputState = &vertex_input_ci;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &info.rasterization;
    create_info.pMultisampleState = &info.multisample;
    create_info.pDepthStencilState = &depth_stencil;
    create_info.pColorBlendState = &color_blend;
    create_info.pDynamicState = &dynamic_state;
    create_info.layout = layout;
    create_info.renderPass = VK_NULL_HANDLE; // we use dynamic rendering

    VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info, nullptr, &gp._pipeline);

    if(result != VK_SUCCESS)
    {
        __LOG_ERROR("Failed to create a new graphics pipeline (" __FMT_I32 "",
                    static_cast<std::int32_t>(result));

        return Error(result, "Failed to create graphics pipeline");
    }

    __LOG_TRACE("Created a new graphics pipeline");

    return gp;
}

inline GraphicsPipeline::GraphicsPipeline()
{
    __LOG_DEBUG("Graphic Pipeline: Initializing GraphicsPipeline");
}

inline GraphicsPipeline::~GraphicsPipeline()
{
    if(this->_pipeline != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
    {
        __LOG_DEBUG("Graphic Pipeline: Destroying GraphicsPipeline");

        vkDestroyPipeline(this->_device, this->_pipeline, nullptr);
    }
}

inline GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
{
    __MOVE_AND_NULL(_device, _pipeline);
}

inline GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
{
    if(this != &other)
    {
        if(this->_pipeline != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
            vkDestroyPipeline(this->_device, this->_pipeline, nullptr);

        __MOVE_AND_NULL(_device, _pipeline);
    }

    return *this;
}

// =============================================================================
//  ComputePipeline creation
// =============================================================================

inline Expected<ComputePipeline> create_compute_pipeline(VkDevice device,
                                                         const ShaderModule& shader,
                                                         VkPipelineLayout layout,
                                                         const char* entry)
{
    __LOG_TRACE("Creating a new compute pipeline");

    ComputePipeline cp;
    cp._device = device;

    VkPipelineShaderStageCreateInfo stage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    stage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = shader.handle();
    stage.pName  = entry;

    VkComputePipelineCreateInfo ci{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    ci.stage  = stage;
    ci.layout = layout;

    VkResult result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &ci, nullptr, &cp._pipeline);

    if(result != VK_SUCCESS)
    {
        __LOG_ERROR("Failed to create a new compute pipeline (" __FMT_I32 ")",
                    static_cast<std::int32_t>(result));

        return Error(result, "Failed to create compute pipeline");
    }

    __LOG_TRACE("Created a new compute pipeline");

    return cp;
}

inline ComputePipeline::ComputePipeline()
{
    __LOG_TRACE("ComputePipeline: Initializing ComputePipeline");
}

inline ComputePipeline::~ComputePipeline()
{
    if(this->_pipeline != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
    {
        __LOG_TRACE("ComputePipeline: Destroying ComputePipeline");

        vkDestroyPipeline(this->_device, this->_pipeline, nullptr);
    }
}

inline ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept : _device(other._device),
                                                                            _pipeline(other._pipeline)
{
    other._device = VK_NULL_HANDLE;
    other._pipeline = VK_NULL_HANDLE;
}

inline ComputePipeline& ComputePipeline::operator=(ComputePipeline&& other) noexcept
{
    if(this != &other)
    {
        if(this->_pipeline != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
            vkDestroyPipeline(_device, _pipeline, nullptr);

        this->_device = other._device;
        this->_pipeline = other._pipeline;

        other._device = VK_NULL_HANDLE;
        other._pipeline = VK_NULL_HANDLE;
    }

    return *this;
}

// =============================================================================
//  FencePool implementation
// =============================================================================

inline FencePool::FencePool(VkDevice device) : _device(device)
{
    __LOG_DEBUG("FencePool: Initializing FencePool");
}

inline FencePool::~FencePool()
{
    __LOG_DEBUG("FencePool: Destroying FencePool");

    for(auto f : this->_all)
        vkDestroyFence(this->_device, f, nullptr);
}

inline FencePool::FencePool(FencePool&& other) noexcept
{
    __MOVE_AND_NULL(_device);
    __MOVE(_free, _all);
}

inline FencePool& FencePool::operator=(FencePool&& other) noexcept
{
    if(this != &other)
    {
        __MOVE_AND_NULL(_device);
        __MOVE(_free, _all);
    }

    return *this;
}

inline VkFence FencePool::acquire(bool signaled)
{
    __LOG_TRACE("FencePool: Acquiring a fence (signaled: " __FMT_BOOL ")",
                static_cast<std::int32_t>(signaled));

    if(!this->_free.empty())
    {
        VkFence f = this->_free.back();
        this->_free.pop_back();

        if(!signaled)
            vkResetFences(this->_device, 1, &f);

        return f;
    }

    VkFenceCreateInfo create_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    if(signaled)
        create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkFence fence;
    PROSERPINE_VK_CHECK_VOID(vkCreateFence(this->_device, &create_info, nullptr, &fence),
                             "Failed to create fence in pool");

    this->_all.push_back(fence);

    __LOG_TRACE("FencePool: Acquired a fence (" __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(fence));

    return fence;
}

inline void FencePool::release(VkFence fence)
{
    __LOG_TRACE("FencePool: Releasing a fence (" __FMT_U64H ")",
                reinterpret_cast<std::uint64_t>(fence));

    this->_free.push_back(fence);
}

} // namespace proserpine

#endif // PROSERPINE_IMPLEMENTATION

#endif // !defined(__PROSERPINE)
