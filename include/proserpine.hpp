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
#elif defined(PROSERPINE_LOG_FORMAT_PRINTF)
#define __FMT_STR "%s"
#define __FMT_WSTR "%ls"
#define __FMT_U32 "%d"
#define __FMT_I32 "%i"
#define __FMT_U64 "%zu"
#define __FMT_U64H "0x%016zx"
#define __FMT_I64 "%zi"
#else
#define __FMT_STR ""
#define __FMT_WSTR ""
#define __FMT_U32 ""
#define __FMT_I32 ""
#define __FMT_U64 ""
#define __FMT_U64H ""
#define __FMT_I64 ""
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

#if !defined(PROSERPINE_ASSERT)
#include <cassert>
#define PROSERPINE_ASSERT(expr) assert(expr)
#endif // !defined(PROSERPINE_ASSERT)

#define PROSERPINE_VK_CHECK(expr, msg)      \
    do {                                    \
        VkResult __vk_res = (expr);         \
        if(__vk_res != VK_SUCCESS)          \
            return Error(__vk_res, (msg));  \
    } while (0)

#define PROSERPINE_VK_CHECK_VOID(expr, msg)                                 \
    do {                                                                    \
        VkResult __vk_res = (expr);                                         \
        if(__vk_res != VK_SUCCESS)                                          \
            __LOG_ERROR(__FMT_I32 ": " __FMT_STR "", __vk_res, msg);        \
    } while (0)

// =============================================================================
//  Required headers
// =============================================================================

#if defined(PROSERPINE_INCLUDE_VULKAN)
#include <vulkan/vulkan.h>
#endif // defined(PROSERPINE_INCLUDE_VULKAN)

#include <algorithm>
#include <atomic>
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
class TimelineSemaphore;
class TimelineCallbackSystem;
class RenderDocIntegration;
class StagingBufferManager;
class DescriptorPool;
class DescriptorSet;
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

template <typename T>
class Expected
{
public:
    Expected(T value) : _data(std::move(value)) {}
    Expected(Error err) : _data(std::move(err)) {}

    explicit operator bool() const { return std::holds_alternative<T>(this->_data); }
    bool has_value() const { return std::holds_alternative<T>(this->_data); }

    bool has_error() const { return std::holds_alternative<Error>(this->_data); }

    T& value() { return std::get<T>(this->_data); }
    const T& value() const { return std::get<T>(this->_data); }

    T& value_or(std::function<void(const Error&)> lbd) { if(this->has_error()) lbd(this->error()); return this->value(); }
    const T& value_or(std::function<void(const Error&)> lbd) const { if(this->has_error()) lbd(this->error()); return this->value(); }

    const Error& error() const { return std::get<Error>(this->_data); }

    T* operator->()       { return std::addressof(this->value()); }
    const T* operator->() const { return std::addressof(this->value()); }
    T& operator*() { return this->value(); }
    const T& operator*() const { return this->value(); }

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
    SharderFloat64,
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
    std::uint32_t min_api_version = VK_API_VERSION_1_3;

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
    bool timeline_semaphore     = true;
    bool buffer_device_address  = true;
    bool descriptor_indexing    = true;
    bool synchronization2       = true;
    bool dynamic_rendering      = true;

    // Opt-in advanced features
    bool ray_query              = false;
    bool ray_tracing_pipeline   = false;
    bool mesh_shader            = false;
    bool shader_int64           = false;
    bool shader_float64         = false;

    std::vector<const char*> extra_device_extensions;
};

// ============================================================================
//  Vulkan handle wrapper
// ============================================================================

template <typename HandleType>
class UniqueVulkanHandle
{
public:
    using deleter_fn = std::function<void(HandleType)>;

    UniqueVulkanHandle() = default;

    UniqueVulkanHandle(HandleType handle, deleter_fn deleter) : _handle(handle),
                                                                _deleter(std::move(deleter)) {}

    ~UniqueVulkanHandle() { this->reset(); }

    UniqueVulkanHandle(const UniqueVulkanHandle&) = delete;
    UniqueVulkanHandle& operator=(const UniqueVulkanHandle&) = delete;

    UniqueVulkanHandle(UniqueVulkanHandle&& other) noexcept : _handle(other._handle),
                                                              _deleter(std::move(other._deleter))
    {
        other._handle = VK_NULL_HANDLE;
    }

    UniqueVulkanHandle& operator=(UniqueVulkanHandle&& other) noexcept
    {
        if(this != &other)
        {
            this->reset();
            this->_handle  = other._handle;
            this->_deleter = std::move(other._deleter);
            other._handle = VK_NULL_HANDLE;
        }

        return *this;
    }

    HandleType get() const { return this->_handle; }
    operator HandleType() const { return this->_handle; }

    inline void reset(HandleType new_handle = VK_NULL_HANDLE)
    {
        if(this->_handle != VK_NULL_HANDLE && this->_deleter)
            this->_deleter(this->_handle);

        this->_handle = new_handle;
    }

    inline HandleType release()
    {
        HandleType h = this->_handle;
        this->_handle = VK_NULL_HANDLE;
        return h;
    }

    inline explicit operator bool() const { return this->_handle != VK_NULL_HANDLE; }

private:
    HandleType _handle = VK_NULL_HANDLE;
    deleter_fn _deleter;
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

    TimelineSemaphore(const TimelineSemaphore&) = delete;
    TimelineSemaphore& operator=(const TimelineSemaphore&) = delete;
    TimelineSemaphore(TimelineSemaphore&& other) noexcept;
    TimelineSemaphore& operator=(TimelineSemaphore&& other) noexcept;

    inline VkSemaphore handle() const { return this->_sema; }
    inline operator VkSemaphore() const { return this->_sema; }

    void signal(std::uint64_t value);
    VkResult wait(std::uint64_t value,
                  std::uint64_t timeout_ns = std::numeric_limits<std::uint64_t>::max());
    uint64_t counter() const;

private:
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

    TimelineCallbackSystem(const TimelineCallbackSystem&) = delete;
    TimelineCallbackSystem& operator=(const TimelineCallbackSystem&) = delete;

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

// ============================================================================
//  RenderDoc integration
// ============================================================================

class RenderDocIntegration
{
public:
    RenderDocIntegration() = default;

    inline bool is_available() const noexcept { return this->_api != nullptr; }

    bool initialize();
    void start_capture();
    void end_capture();

private:
    void* _api  = nullptr; // RENDERDOC_API_1_6_0*
    void* _module = nullptr;
};

// ============================================================================
//  Buffer
// ============================================================================

struct BufferCreateInfo
{
    VkDeviceSize size = 0;
    VkBufferUsageFlags usage = 0;
    VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    const void* initial_data = nullptr; // if non-null, staged upload
};

class Buffer
{
public:
    Buffer() = default;
    Buffer(VkDevice device,
           VkPhysicalDevice physical_device,
           const BufferCreateInfo& info);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

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

struct ImageCreateInfo
{
    VkExtent3D extent = {1, 1, 1};
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    VkImageType type = VK_IMAGE_TYPE_2D;
    std::uint32_t mip_levels = 1;
    std::uint32_t array_layers = 1;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
};

class Image
{
public:
    Image() = default;
    Image(VkDevice device,
          VkPhysicalDevice physical_device,
          const ImageCreateInfo& info);
    ~Image();

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

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

    StagingBufferManager(const StagingBufferManager&) = delete;
    StagingBufferManager& operator=(const StagingBufferManager&) = delete;
    StagingBufferManager(StagingBufferManager&& other) noexcept;
    StagingBufferManager& operator=(StagingBufferManager&& other) noexcept;

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

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool& operator=(DescriptorPool&& other) noexcept;

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

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    DescriptorSet(DescriptorSet&& other) noexcept;
    DescriptorSet& operator=(DescriptorSet&& other) noexcept;

    inline const VkDescriptorSet& handle() const { return this->_set; }

    // TODO: make function per descriptor type instead of asking the user to
    // choose the type each time, cleaner
    DescriptorSet& write(std::uint32_t binding,
                         const Buffer& buffer,
                         VkDescriptorType type,
                         VkDeviceSize offset = 0,
                         VkDeviceSize range = VK_WHOLE_SIZE);

    void update();

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

std::vector<std::uint32_t> load_spirv_file(const char* file_path) noexcept;

class ShaderModule
{
public:
    ShaderModule();
    ~ShaderModule();

    ShaderModule(const ShaderModule&) = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule& operator=(ShaderModule&& other) noexcept;

    static Expected<ShaderModule> create(VkDevice device,
                                         const std::vector<std::uint32_t>& spirv,
                                         VkShaderStageFlagBits stage);

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
};

// ============================================================================
//  PipelineLayout
// ============================================================================

class PipelineLayout
{
public:
    PipelineLayout();
    ~PipelineLayout();

    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout& operator=(PipelineLayout&& other) noexcept;

    inline VkPipelineLayout handle() const { return this->_layout; }
    inline operator VkPipelineLayout() const { return this->_layout; }

    inline const std::vector<VkDescriptorSetLayout>& set_layouts() const { return this->_set_layouts; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkPipelineLayout _layout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayout> _set_layouts;

    friend class PipelineLayoutBuilder;
};

// ============================================================================
//  PipelineLayoutBuilder
// ============================================================================

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

// ============================================================================
//  Pipeline wrappers
// ============================================================================

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

// Info used to build the GraphicsPipeline.
// "Reasonable" default values so you don't have to initialize everything
struct GraphicsPipelineInfo
{
    // Input Assembly
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkBool32 primitive_restart_enable = VK_FALSE;

    // Viewport / scissor
    std::uint32_t viewport_count = 1;
    std::uint32_t scissor_count = 1;

    // Rasterization
    VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    float line_width = 1.0f;

    // Multisample
    VkSampleCountFlagBits rasterization_samples = VK_SAMPLE_COUNT_1_BIT;

    // Depth/stencil
    VkBool32 depth_test_enable = VK_TRUE;
    VkBool32 depth_write_enable = VK_TRUE;
    VkCompareOp depth_compare_op = VK_COMPARE_OP_LESS;

    // Color Blend
    VkColorComponentFlags color_write_mask = VK_COLOR_COMPONENT_R_BIT |
                                             VK_COLOR_COMPONENT_G_BIT |
                                             VK_COLOR_COMPONENT_B_BIT |
                                             VK_COLOR_COMPONENT_A_BIT;
};

class GraphicsPipeline
{
public:
    GraphicsPipeline();
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

    inline VkPipeline handle() const { return this->_pipeline; }
    inline operator VkPipeline() const { return this->_pipeline; }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;

    friend Expected<GraphicsPipeline> create_graphics_pipeline(VkDevice,
                                                               const GraphicsPipelineInfo&,
                                                               const ShaderStages&,
                                                               const VertexInputState&,
                                                               VkPipelineLayout,
                                                               VkFormat,
                                                               VkFormat,
                                                               const VkPipelineRasterizationStateCreateInfo*,
                                                               const VkPipelineMultisampleStateCreateInfo*);
};

class ComputePipeline
{
public:
    ComputePipeline();
    ~ComputePipeline();

    ComputePipeline(const ComputePipeline&) = delete;
    ComputePipeline& operator=(const ComputePipeline&) = delete;
    ComputePipeline(ComputePipeline&& other) noexcept;
    ComputePipeline& operator=(ComputePipeline&& other) noexcept;

    inline VkPipeline handle() const { return this->_pipeline; }
    inline operator VkPipeline() const { return this->_pipeline; }

private:
    VkDevice _device   = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;

    friend Expected<ComputePipeline> create_compute_pipeline(
        VkDevice device, const ShaderModule& shader,
        VkPipelineLayout layout, const char* entry);
};

Expected<GraphicsPipeline> create_graphics_pipeline(VkDevice device,
                                                    const GraphicsPipelineInfo& info,
                                                    const ShaderStages& stages,
                                                    const VertexInputState& vertex_input,
                                                    VkPipelineLayout layout,
                                                    VkFormat color_format = VK_FORMAT_B8G8R8A8_UNORM,
                                                    VkFormat depth_format = VK_FORMAT_UNDEFINED,
                                                    const VkPipelineRasterizationStateCreateInfo* rasterization = nullptr,
                                                    const VkPipelineMultisampleStateCreateInfo* multisample = nullptr);

Expected<ComputePipeline> create_compute_pipeline(VkDevice device,
                                                  const ShaderModule& shader,
                                                  VkPipelineLayout layout,
                                                  const char* entry = "main");

// ============================================================================
//  Fence / Semaphore Pool
// ============================================================================

class FencePool
{
public:
    explicit FencePool(VkDevice device);
    ~FencePool();

    FencePool(const FencePool&) = delete;
    FencePool& operator=(const FencePool&) = delete;

    VkFence acquire(bool signaled = false);
    void release(VkFence fence);

private:
    VkDevice _device;
    std::vector<VkFence> _free;
    std::vector<VkFence> _all;
};

// ============================================================================
//  Context
// ============================================================================

class VulkanContext
{
public:
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

    static Expected<VulkanContext> create(const CreateInfo& info);

    VulkanContext() = default;
    ~VulkanContext();

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;
    VulkanContext(VulkanContext&& other) noexcept;
    VulkanContext& operator=(VulkanContext&& other) noexcept;

    inline VkInstance instance() const { return this->_instance; }
    inline VkPhysicalDevice physical_device() const { return this->_physical_device; }
    inline VkDevice device() const { return this->_device; }

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

    Buffer create_buffer(const BufferCreateInfo& info);
    Image create_image(const ImageCreateInfo& info);
    TimelineSemaphore create_timeline_semaphore(std::uint64_t initial_value = 0);

    VkFence create_fence(bool signaled = false);
    void wait_idle();

    void immediate_submit(QueueType queue_type,
                          const std::function<void(VkCommandBuffer)>& record);

    Expected<DescriptorSet> allocate_descriptor_set(VkDescriptorSetLayout layout);

private:
    VkInstance _instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;
    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

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

    bool _validation_enabled = false;

    static VkResult create_instance(const CreateInfo& info,
                                    VkInstance& out_instance,
                                    VkDebugUtilsMessengerEXT& out_messenger);

    static Expected<SelectedDevice> select_device(VkInstance instance,
                                                  const DeviceFilter& filter);

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

inline Expected<SelectedDevice> VulkanContext::select_device(VkInstance instance,
                                                             const DeviceFilter& filter)
{
    std::uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);

    if(count == 0)
        return Error("No Vulkan physical devices found");

    std::vector<VkPhysicalDevice> physical_devices(count);
    vkEnumeratePhysicalDevices(instance, &count, physical_devices.data());

    struct Candidate
    {
        VkPhysicalDevice physical_device;
        DeviceProperties properties;
        std::int32_t score;
    };

    std::vector<Candidate> candidates;

    for(auto physical_device : physical_devices)
    {
        DeviceProperties device_properties;

        vkGetPhysicalDeviceProperties(physical_device, &device_properties.properties);

        // Version check
        if(device_properties.properties.apiVersion < filter.min_api_version)
            continue;

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
            continue;

        // Check queue family requirements
        bool has_graphics = false;

        for(std::uint32_t i = 0; i < qf_count; ++i)
        {
            if(device_properties.queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                has_graphics = true;
                break;
            }
        }

        if(!has_graphics)
            continue;

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
                continue;

            score = user_score;
        }

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
        if(qfams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            selected.queue_family_indices[static_cast<std::size_t>(QueueType::Graphics)] = i;
            selected.queue_family_valid[static_cast<std::size_t>(QueueType::Graphics)]   = true;
            break;
        }
    }

    std::uint32_t gfx_family = selected.queue_family_indices[static_cast<std::size_t>(QueueType::Graphics)];

    for(std::uint32_t i = 0; i < qf_count; ++i)
    {
        if((qfams[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && i != gfx_family)
        {
            selected.queue_family_indices[static_cast<std::size_t>(QueueType::Compute)] = i;
            selected.queue_family_valid[static_cast<std::size_t>(QueueType::Compute)]   = true;
            break;
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

    // Transfer queue (prefer dedicated)
    for(std::uint32_t i = 0; i < qf_count; ++i)
    {
        if((qfams[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
           !(qfams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
           !(qfams[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
        {
            selected.queue_family_indices[static_cast<std::size_t>(QueueType::Transfer)] = i;
            selected.queue_family_valid[static_cast<std::size_t>(QueueType::Transfer)]   = true;
            break;
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

    auto maybe_add_ext = [&](const char* ext) {
        if(has_extension(selected.props.available_extensions, ext))
            device_extensions.push_back(ext);
    };

    maybe_add_ext(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

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
        f12.descriptorIndexing                            = VK_TRUE;
        f12.shaderSampledImageArrayNonUniformIndexing     = VK_TRUE;
        f12.descriptorBindingVariableDescriptorCount      = VK_TRUE;
        f12.runtimeDescriptorArray                        = VK_TRUE;
        f12.descriptorBindingSampledImageUpdateAfterBind   = VK_TRUE;
        f12.descriptorBindingStorageBufferUpdateAfterBind  = VK_TRUE;
        f12.descriptorBindingStorageImageUpdateAfterBind   = VK_TRUE;
        f12.descriptorBindingUniformBufferUpdateAfterBind  = VK_TRUE;
        f12.descriptorBindingPartiallyBound               = VK_TRUE;
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
        this->_feature_flags[static_cast<std::size_t>(Feature::SharderFloat64)] = true;
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

    for(std::size_t i = 0; i < static_cast<std::size_t>(QueueType::Count); ++i)
    {
        if(selected.queue_family_valid[i])
        {
            this->_queues[i].family = selected.queue_family_indices[i];
            this->_queues[i].valid  = true;
            vkGetDeviceQueue(this->_device, this->_queues[i].family, 0, &this->_queues[i].queue);
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
            vkCreateFence(this->_device, &fence_create_info, nullptr, &this->_immediate_contexts[i].fence);
        }
    }

    __LOG_TRACE("New logical device created successfully");

    return VK_SUCCESS;
}

// ============================================================================
//  VulkanContext::create
// ============================================================================

inline Expected<VulkanContext> VulkanContext::create(const CreateInfo& info)
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

    auto selected = select_device(ctx._instance, info.device_filter);

    if(!selected)
    {
        vkDestroyInstance(ctx._instance, nullptr);
        return selected.error();
    }

    ctx._physical_device = selected->physical_device;
    ctx._device_properties = std::move(selected->props);
    ctx._selected_device = std::move(*selected);

    result = ctx.create_logical_device(ctx._selected_device, info.features);

    if(result != VK_SUCCESS)
    {
        vkDestroyInstance(ctx._instance, nullptr);
        return Error(result, "Failed to create logical device");
    }

    ctx._descriptor_pool_create_info = info.descriptor_pool_info;

    __LOG_TRACE("Created a new VulkanContext successfully");

    return ctx;
}

// ============================================================================
//  VulkanContext dtor/move
// ============================================================================

inline void VulkanContext::destroy()
{
    if(this->_device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(this->_device);

        this->_staging.reset();
        this->_timeline_callbacks.reset();
        this->_fence_pool.reset();
        this->_descriptor_pool.reset();

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

    if(this->_debug_messenger != VK_NULL_HANDLE && this->_instance != VK_NULL_HANDLE)
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->_instance, "vkDestroyDebugUtilsMessengerEXT"));

        if(func)
            func(this->_instance, this->_debug_messenger, nullptr);

        this->_debug_messenger = VK_NULL_HANDLE;
    }

    if(this->_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(this->_instance, nullptr);
        this->_instance = VK_NULL_HANDLE;
    }
}

inline VulkanContext::~VulkanContext() { this->destroy(); }

inline VulkanContext::VulkanContext(VulkanContext&& other) noexcept : _instance(other._instance),
                                                                      _debug_messenger(other._debug_messenger),
                                                                      _physical_device(other._physical_device),
                                                                      _device(other._device),
                                                                      _device_properties(std::move(other._device_properties)),
                                                                      _selected_device(std::move(other._selected_device)),
                                                                      _enabled_device_extensions(std::move(other._enabled_device_extensions)),
                                                                      _staging(std::move(other._staging)),
                                                                      _timeline_callbacks(std::move(other._timeline_callbacks)),
                                                                      _renderdoc(std::move(other._renderdoc)),
                                                                      _fence_pool(std::move(other._fence_pool)),
                                                                      _validation_enabled(other._validation_enabled)
{
    std::memcpy(this->_queues, other._queues, sizeof(_queues));
    std::memcpy(this->_immediate_contexts, other._immediate_contexts, sizeof(_immediate_contexts));
    std::memcpy(this->_feature_flags, other._feature_flags, sizeof(_feature_flags));

    other._instance = VK_NULL_HANDLE;
    other._debug_messenger = VK_NULL_HANDLE;
    other._physical_device = VK_NULL_HANDLE;
    other._device = VK_NULL_HANDLE;
    std::memset(other._queues, 0, sizeof(other._queues));
    std::memset(other._immediate_contexts, 0, sizeof(other._immediate_contexts));
}

inline VulkanContext& VulkanContext::operator=(VulkanContext&& other) noexcept
{
    if(this != &other)
    {
        this->destroy();

        this->_instance        = other._instance;
        this->_debug_messenger = other._debug_messenger;
        this->_physical_device = other._physical_device;
        this->_device          = other._device;
        this->_device_properties    = std::move(other._device_properties);
        this->_selected_device = std::move(other._selected_device);
        this->_enabled_device_extensions = std::move(other._enabled_device_extensions);
        this->_staging             = std::move(other._staging);
        this->_timeline_callbacks  = std::move(other._timeline_callbacks);
        this->_renderdoc           = std::move(other._renderdoc);
        this->_fence_pool          = std::move(other._fence_pool);
        this->_validation_enabled  = other._validation_enabled;

        std::memcpy(this->_queues, other._queues, sizeof(_queues));
        std::memcpy(this->_immediate_contexts, other._immediate_contexts, sizeof(_immediate_contexts));
        std::memcpy(this->_feature_flags, other._feature_flags, sizeof(_feature_flags));

        other._instance        = VK_NULL_HANDLE;
        other._debug_messenger = VK_NULL_HANDLE;
        other._physical_device = VK_NULL_HANDLE;
        other._device          = VK_NULL_HANDLE;
        std::memset(other._queues, 0, sizeof(other._queues));
        std::memset(other._immediate_contexts, 0, sizeof(other._immediate_contexts));
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
    PROSERPINE_ASSERT(this->_queues[i].valid);
    return this->_queues[i].queue;
}

inline std::uint32_t VulkanContext::queue_family(QueueType type) const
{
    std::size_t i = static_cast<std::size_t>(type);
    PROSERPINE_ASSERT(this->_queues[i].valid);
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
    {
        this->_renderdoc = std::make_unique<RenderDocIntegration>();
        this->_renderdoc->initialize();
    }

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

// =============================================================================
//  VulkanContext object creation helpers
// =============================================================================

inline std::uint32_t VulkanContext::find_memory_type(VkPhysicalDevice physical_device,
                                                     std::uint32_t type_filter,
                                                     VkMemoryPropertyFlags props)
{
    return proserpine::find_memory_type(physical_device, type_filter, props);
}

inline Buffer VulkanContext::create_buffer(const BufferCreateInfo& info)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE);
    PROSERPINE_ASSERT(this->_physical_device != VK_NULL_HANDLE);

    return Buffer(this->_device, this->_physical_device, info);
}

inline Image VulkanContext::create_image(const ImageCreateInfo& info)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE);
    PROSERPINE_ASSERT(this->_physical_device != VK_NULL_HANDLE);

    return Image(this->_device, this->_physical_device, info);
}

inline TimelineSemaphore VulkanContext::create_timeline_semaphore(std::uint64_t initial_value)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE);

    return TimelineSemaphore(this->_device, initial_value);
}

inline VkFence VulkanContext::create_fence(bool signaled)
{
    PROSERPINE_ASSERT(this->_device != VK_NULL_HANDLE);

    VkFenceCreateInfo ci{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    if(signaled)
        ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkFence fence = VK_NULL_HANDLE;

    PROSERPINE_VK_CHECK_VOID(vkCreateFence(this->_device, &ci, nullptr, &fence),
                             "Failed to create fence");

    return fence;
}

inline void VulkanContext::wait_idle()
{
    if(this->_device != VK_NULL_HANDLE)
        vkDeviceWaitIdle(this->_device);
}

inline void VulkanContext::immediate_submit(QueueType queue_type,
                                            const std::function<void(VkCommandBuffer)>& record)
{
    // TODO: use timeline semaphore

    auto qi = static_cast<std::size_t>(queue_type);
    auto& imm_ctx = this->_immediate_contexts[qi];

    PROSERPINE_ASSERT(this->_queues[qi].valid &&
                      imm_ctx.pool != VK_NULL_HANDLE &&
                      imm_ctx.cmd != VK_NULL_HANDLE &&
                      imm_ctx.fence != VK_NULL_HANDLE);

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

    vkWaitForFences(this->_device, 1, &imm_ctx.fence, VK_TRUE, std::numeric_limits<std::uint64_t>::max());
}

inline Expected<DescriptorSet> VulkanContext::allocate_descriptor_set(VkDescriptorSetLayout layout)
{
    return this->descriptor_pool().allocate_descriptor_set(layout);
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

inline TimelineSemaphore::TimelineSemaphore(TimelineSemaphore&& other) noexcept : _device(other._device),
                                                                                  _sema(other._sema)
{
    other._device    = VK_NULL_HANDLE;
    other._sema = VK_NULL_HANDLE;
}

inline TimelineSemaphore& TimelineSemaphore::operator=(TimelineSemaphore&& other) noexcept
{
    if(this != &other)
    {
        if(this->_sema != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
            vkDestroySemaphore(_device, _sema, nullptr);

        this->_device = other._device;
        this->_sema = other._sema;

        other._device = VK_NULL_HANDLE;
        other._sema = VK_NULL_HANDLE;
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

inline bool RenderDocIntegration::initialize()
{
    __LOG_TRACE("Initializing RenderDoc integration");

    using GetApiFn = int(*)(int, void**);

#if defined(PROSERPINE_PLATFORM_LINUX)
    this->_module = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);

    if(this->_module == nullptr)
    {
        __LOG_WARN("Cannot find librenderdoc.so, disabling RenderDoc captures");
        return false;
    }

    auto get_api = reinterpret_cast<GetApiFn>(dlsym(module_, "RENDERDOC_GetAPI"));
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
        return false;
    }

    auto get_api = reinterpret_cast<GetApiFn>(GetProcAddress(static_cast<HMODULE>(this->_module),
                                                             "RENDERDOC_GetAPI"));
#else
    return false;
#endif // defined(PROSERPINE_PLATFORM_LINUX)

    __LOG_TRACE("Found RenderDoc library");

    if(get_api == nullptr)
    {
        __LOG_WARN("Cannot find RENDERDOC_GetAPI proc, disabling RenderDoc captures");
        return false;
    }

    int ret = get_api(10600, &this->_api); // RENDERDOC_API_VERSION_1_6_0

    if(ret != 1 || this->_api == nullptr)
    {
        __LOG_ERROR("Cannot get RenderDoc api, disabling RenderDoc captures");
        return false;
    }

    __LOG_INFO("Found RenderDoc api, enabling RenderDoc captures");

    return true;
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
//  Buffer implementation
// ============================================================================

inline Buffer::Buffer(VkDevice device,
                      VkPhysicalDevice physical_device,
                      const BufferCreateInfo& info) : _device(device),
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

inline Buffer::Buffer(Buffer&& other) noexcept : _device(other._device),
                                                 _buffer(other._buffer),
                                                 _memory(other._memory),
                                                 _size(other._size),
                                                 _device_address(other._device_address),
                                                 _mapped(other._mapped)
{
    other._device = VK_NULL_HANDLE;
    other._buffer = VK_NULL_HANDLE;
    other._memory = VK_NULL_HANDLE;
    other._mapped = nullptr;
    other._size = 0;
    other._device_address = 0;
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

        this->_device = other._device;
        this->_buffer = other._buffer;
        this->_memory = other._memory;
        this->_size = other._size;
        this->_device_address = other._device_address;
        this->_mapped = other._mapped;

        other._device = VK_NULL_HANDLE;
        other._buffer = VK_NULL_HANDLE;
        other._memory = VK_NULL_HANDLE;
        other._mapped = nullptr;
        other._size = 0;
        other._device_address = 0;
    }

    return *this;
}

// ============================================================================
//  Image implementation
// ============================================================================

inline Image::Image(VkDevice device,
                    VkPhysicalDevice physical_device,
                    const ImageCreateInfo& info) : _device(device),
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
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

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
    view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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

inline Image::Image(Image&& other) noexcept : _device(other._device),
                                              _image(other._image),
                                              _view(other._view),
                                              _memory(other._memory),
                                              _format(other._format),
                                              _extent(other._extent)
{
    other._device = VK_NULL_HANDLE;
    other._image = VK_NULL_HANDLE;
    other._view = VK_NULL_HANDLE;
    other._memory = VK_NULL_HANDLE;
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

        this->_device = other._device;
        this->_image = other._image;
        this->_view = other._view;
        this->_memory = other._memory;
        this->_format = other._format;
        this->_extent = other._extent;

        other._device = VK_NULL_HANDLE;
        other._image = VK_NULL_HANDLE;
        other._view = VK_NULL_HANDLE;
        other._memory = VK_NULL_HANDLE;
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

inline StagingBufferManager::StagingBufferManager(StagingBufferManager&& other) noexcept : _device(other._device),
                                                                                           _physical_device(other._physical_device),
                                                                                           _transfer_queue(other._transfer_queue),
                                                                                           _transfer_family(other._transfer_family),
                                                                                           _staging_buffer(other._staging_buffer),
                                                                                           _staging_memory(other._staging_memory),
                                                                                           _staging_mapped(other._staging_mapped),
                                                                                           _capacity(other._capacity),
                                                                                           _used(other._used),
                                                                                           _command_pool(other._command_pool),
                                                                                           _fence(other._fence),
                                                                                           _pending(std::move(other._pending))
{
    other._device = VK_NULL_HANDLE;
    other._staging_buffer = VK_NULL_HANDLE;
    other._staging_memory = VK_NULL_HANDLE;
    other._staging_mapped = nullptr;
    other._command_pool = VK_NULL_HANDLE;
    other._fence = VK_NULL_HANDLE;
}

inline StagingBufferManager& StagingBufferManager::operator=(StagingBufferManager&& other) noexcept
{
    if(this != &other)
    {
        this->_device = other._device;
        this->_physical_device = other._physical_device;
        this->_transfer_queue = other._transfer_queue;
        this->_transfer_family = other._transfer_family;
        this->_staging_buffer = other._staging_buffer;
        this->_staging_memory = other._staging_memory;
        this->_staging_mapped = other._staging_mapped;
        this->_capacity = other._capacity;
        this->_used = other._used;
        this->_command_pool = other._command_pool;
        this->_fence = other._fence;
        this->_pending = std::move(other._pending);

        other._device = VK_NULL_HANDLE;
        other._staging_buffer = VK_NULL_HANDLE;
        other._staging_memory = VK_NULL_HANDLE;
        other._staging_mapped = nullptr;
        other._command_pool = VK_NULL_HANDLE;
        other._fence = VK_NULL_HANDLE;
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

    __LOG_TRACE("StagingBufferManager: flush");

    VkCommandBufferAllocateInfo alloc{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc.commandPool = _command_pool;
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
    vkWaitForFences(this->_device, 1, &this->_fence, VK_TRUE, std::numeric_limits<std::uint64_t>::max());

    vkFreeCommandBuffers(this->_device, this->_command_pool, 1, &cmd);

    this->_pending.clear();
    this->_used = 0;
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
                vkFreeDescriptorSets(this->_device,
                                     this->_pool,
                                     static_cast<std::uint32_t>(this->_sets.size()),
                                     this->_sets.data());

            vkDestroyDescriptorPool(this->_device, this->_pool, nullptr);
        }
    }
}

inline DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept : _device(other._device),
                                                                         _pool(other._pool)
{
    other._device = VK_NULL_HANDLE;
    other._pool = VK_NULL_HANDLE;
}

inline DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
{
    if(this != &other)
    {
        this->_device = other._device;
        this->_pool = other._pool;

        other._device = VK_NULL_HANDLE;
        other._pool = VK_NULL_HANDLE;
    }

    return *this;
}


inline Expected<DescriptorSet> DescriptorPool::allocate_descriptor_set(VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    create_info.descriptorPool = this->_pool;
    create_info.descriptorSetCount = 1;
    create_info.pSetLayouts = &layout;

    VkDescriptorSet set;

    PROSERPINE_VK_CHECK(vkAllocateDescriptorSets(this->_device, &create_info, &set),
                        "Failed to allocate descriptor sets");

    this->_sets.push_back(set);

    return DescriptorSet(this->_device, set);
}

// =============================================================================
//  Descriptor Set implementation
// =============================================================================

inline DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept : _device(other._device),
                                                                      _set(other._set),
                                                                      _writes(std::move(other._writes)),
                                                                      _buffer_infos(std::move(other._buffer_infos))
{
    other._device = VK_NULL_HANDLE;
    other._set = VK_NULL_HANDLE;
}

inline DescriptorSet::~DescriptorSet()
{
}

inline DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
{
    if(this != &other)
    {
        this->_device = other._device;
        this->_set = other._set;
        this->_writes = std::move(other._writes);
        this->_buffer_infos = std::move(other._buffer_infos);

        other._device = VK_NULL_HANDLE;
        other._set = VK_NULL_HANDLE;
    }

    return *this;
}

inline DescriptorSet& DescriptorSet::write(std::uint32_t binding,
                                           const Buffer& buffer,
                                           VkDescriptorType type,
                                           VkDeviceSize offset,
                                           VkDeviceSize range)
{
    this->_buffer_infos.push_back({ buffer.handle(), offset, range });

    this->_writes.push_back({ binding, type, this->_buffer_infos.size() - 1 });

    return *this;
}

inline void DescriptorSet::update()
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
}

// =============================================================================
//  ShaderModule implementation (with minimal SPIR-V reflection)
// =============================================================================

inline std::vector<std::uint32_t> load_spirv_file(const char* file_path) noexcept
{
    std::vector<std::uint32_t> spirv;

    std::FILE* file = std::fopen(file_path, "rb");

    if(file == nullptr)
        return spirv;

    std::fseek(file, 0, SEEK_END);
    std::size_t file_sz = std::ftell(file);
    std::rewind(file);

    spirv.resize(file_sz / 4);
    std::memset(spirv.data(), 0, spirv.size() * sizeof(std::uint32_t));

    std::fread(spirv.data(), sizeof(std::uint8_t), file_sz, file);

    return spirv;
}

inline ShaderModule::ShaderModule()
{
    __LOG_TRACE("ShaderModule : Initializing ShaderModule");
}

inline Expected<ShaderModule> ShaderModule::create(VkDevice device,
                                                   const std::vector<std::uint32_t>& spirv,
                                                   VkShaderStageFlagBits stage)
{
    __LOG_TRACE("ShaderModule : creating a new shader module (" __FMT_U64H ")",
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
        __LOG_ERROR("ShaderModule : Failed to create a new shader module");
        return Error(result, "Failed to create shader module");
    }

    sm.reflect_spirv(spirv);

    __LOG_TRACE("ShaderModule : creatded a new shader module");

    return sm;
}

inline ShaderModule::~ShaderModule()
{
    __LOG_TRACE("ShaderModule : Destroying ShaderModule");

    if(this->_module != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
        vkDestroyShaderModule(this->_device, this->_module, nullptr);
}

inline ShaderModule::ShaderModule(ShaderModule&& other) noexcept : _device(other._device),
                                                            _module(other._module),
                                                            _stage(other._stage),
                                                            _set_layouts(std::move(other._set_layouts)),
                                                            _push_ranges(std::move(other._push_ranges)),
                                                            _vertex_inputs(std::move(other._vertex_inputs))
{
    other._device = VK_NULL_HANDLE;
    other._module = VK_NULL_HANDLE;
}

inline ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept
{
    if(this != &other)
    {
        if(this->_module != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
            vkDestroyShaderModule(this->_device, _module, nullptr);

        this->_device = other._device;
        this->_module = other._module;
        this->_stage = other._stage;
        this->_set_layouts    = std::move(other._set_layouts);
        this->_push_ranges    = std::move(other._push_ranges);
        this->_vertex_inputs  = std::move(other._vertex_inputs);

        other._device = VK_NULL_HANDLE;
        other._module = VK_NULL_HANDLE;
    }

    return *this;
}

// Minimal SPIR-V parser for descriptor and push constant reflection.
// This avoids an external dependency on spirv-reflect.
// It reads OpDecorate, OpMemberDecorate, OpVariable, and type instructions.
inline void ShaderModule::reflect_spirv(const std::vector<std::uint32_t>& spirv)
{
    __LOG_TRACE("ShaderModule : reflecting spirv");

    if(spirv.size() < 5)
    {
        __LOG_TRACE("ShaderModule : spirv code size less than 5");
        return;
    }

    // SPIR-V magic check
    if(spirv[0] != 0x07230203)
    {
        __LOG_TRACE("ShaderModule : unexpected spirv header magic " __FMT_U32 "",
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

    __LOG_TRACE("ShaderModule : spirv reflection successful");
}

// =============================================================================
//  PipelineLayoutBuilder implementation
// =============================================================================

inline PipelineLayoutBuilder& PipelineLayoutBuilder::add_set(std::uint32_t set,
                                                             const DescriptorSetLayoutInfo& info)
{
    __LOG_TRACE("PipelineLayoutBuilder : Added a new set (" __FMT_U32 ")",
                info.set);

    this->_sets.push_back({set, info});

    return *this;
}

inline PipelineLayoutBuilder& PipelineLayoutBuilder::add_push_constant_range(VkShaderStageFlags stages,
                                                                             std::uint32_t offset,
                                                                             std::uint32_t size)
{
    __LOG_TRACE("PipelineLayoutBuilder : Added a push constant range (" __FMT_U32 ", " __FMT_U32 ")",
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
    __LOG_TRACE("PipelineLayoutBuilder : Building a new pipeline layout");

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
                __LOG_ERROR("PipelineLayoutBuilder : Failed to create a new pipeline layout (failed to create empty descriptor set layout)");
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

            __LOG_ERROR("PipelineLayoutBuilder : Failed to create a new pipeline layout (failed to create the descriptor set layout)");

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

        __LOG_ERROR("PipelineLayoutBuilder : Failed to create a new pipeline layout");

        return Error(result, "Failed to create pipeline layout");
    }

    __LOG_TRACE("PipelineLayoutBuilder : Built a new pipeline layout");

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
    __LOG_TRACE("PipelineLayout: Destroying Pipeline Layout");

    if(this->_device != VK_NULL_HANDLE)
    {
        if(this->_layout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(this->_device, this->_layout, nullptr);

        for(auto sl : this->_set_layouts)
            if(sl != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(this->_device, sl, nullptr);
    }
}

inline PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept : _device(other._device),
                                                                         _layout(other._layout),
                                                                         _set_layouts(std::move(other._set_layouts))
{
    other._device = VK_NULL_HANDLE;
    other._layout = VK_NULL_HANDLE;
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

        this->_device = other._device;
        this->_layout = other._layout;
        this->_set_layouts = std::move(other._set_layouts);

        other._device = VK_NULL_HANDLE;
        other._layout = VK_NULL_HANDLE;
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

// TODO: Add some create info struct for more control over this

inline Expected<GraphicsPipeline> create_graphics_pipeline(VkDevice device,
                                                           const GraphicsPipelineInfo& info,
                                                           const ShaderStages& stages,
                                                           const VertexInputState& vertex_input,
                                                           VkPipelineLayout layout,
                                                           VkFormat color_format,
                                                           VkFormat depth_format,
                                                           const VkPipelineRasterizationStateCreateInfo* rasterization,
                                                           const VkPipelineMultisampleStateCreateInfo* multisample)
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

    // Rasterization
    VkPipelineRasterizationStateCreateInfo default_rast{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    default_rast.polygonMode = info.polygon_mode;
    default_rast.cullMode = info.cull_mode;
    default_rast.frontFace = info.front_face;
    default_rast.lineWidth = info.line_width;
    const auto* rast = rasterization ? rasterization : &default_rast;

    // Multisample
    VkPipelineMultisampleStateCreateInfo default_ms{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    default_ms.rasterizationSamples = info.rasterization_samples;
    const auto* ms = multisample ? multisample : &default_ms;

    // Depth/stencil
    VkPipelineDepthStencilStateCreateInfo depth_stencil{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    if(depth_format != VK_FORMAT_UNDEFINED)
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
    rendering_ci.pColorAttachmentFormats = &color_format;
    rendering_ci.depthAttachmentFormat = depth_format;

    VkGraphicsPipelineCreateInfo create_info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    create_info.pNext = &rendering_ci;
    create_info.stageCount = static_cast<std::uint32_t>(stages.stages.size());
    create_info.pStages = stages.stages.data();
    create_info.pVertexInputState = &vertex_input_ci;
    create_info.pInputAssemblyState = &input_assembly;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = rast;
    create_info.pMultisampleState = ms;
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
    __LOG_DEBUG("Graphic Pipeline: Destroying GraphicsPipeline");

    if(this->_pipeline != VK_NULL_HANDLE && _device != VK_NULL_HANDLE)
        vkDestroyPipeline(this->_device, this->_pipeline, nullptr);
}

inline GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept : _device(other._device),
                                                                               _pipeline(other._pipeline)
{
    other._device = VK_NULL_HANDLE;
    other._pipeline = VK_NULL_HANDLE;
}

inline GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
{
    if(this != &other)
    {
        if(this->_pipeline != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
            vkDestroyPipeline(this->_device, this->_pipeline, nullptr);

        this->_device = other._device;
        this->_pipeline = other._pipeline;

        other._device = VK_NULL_HANDLE;
        other._pipeline = VK_NULL_HANDLE;
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
    __LOG_DEBUG("ComputePipeline: Initializing ComputePipeline");
}

inline ComputePipeline::~ComputePipeline()
{
    __LOG_DEBUG("ComputePipeline: Destroying ComputePipeline");

    if(this->_pipeline != VK_NULL_HANDLE && this->_device != VK_NULL_HANDLE)
        vkDestroyPipeline(this->_device, this->_pipeline, nullptr);
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

inline VkFence FencePool::acquire(bool signaled)
{
    __LOG_TRACE("FencePool: Acquiring a fence");

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

    VkFence f;
    PROSERPINE_VK_CHECK_VOID(vkCreateFence(this->_device, &create_info, nullptr, &f),
                             "Failed to create fence in pool");

    this->_all.push_back(f);

    return f;
}

inline void FencePool::release(VkFence fence)
{
    __LOG_TRACE("FencePool: Releasing a fence");

    this->_free.push_back(fence);
}

} // namespace proserpine

#endif // PROSERPINE_IMPLEMENTATION

#undef __FMT_STR
#undef __FMT_WSTR
#undef __FMT_U32
#undef __FMT_I32
#undef __FMT_U64
#undef __FMT_U64H
#undef __FMT_I64

#undef __LOG_ERROR
#undef __LOG_WARN
#undef __LOG_INFO
#undef __LOG_DEBUG
#undef __LOG_TRACE

#endif // !defined(__PROSERPINE)
