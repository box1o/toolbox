#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/types.hpp>
#include <toolbox/gfx/common.hpp>

namespace ct::gfx {

struct AdapterInfo {
    std::string vendor{};
    std::string architecture{};
    std::string device{};
    std::string description{};
    u32 backendType{0};
    u32 adapterType{0};
};

struct Limits {
    u32 maxVertexAttributes{0};
    u32 maxColorAttachments{0};
    u32 maxTextureDimension2D{0};
    u64 maxBufferSize{0};
    u32 maxBindGroups{0};
};

struct DeviceDesc {
    Backend backend{Backend::Auto};
    PowerPreference powerPreference{PowerPreference::HighPerformance};
    bool enableValidation{true};
    DebugConfig debug{};
    std::string debugName{"Device"};
};

// Not included by gfx.hpp on purpose.
// Used by backend code (Surface/Swapchain later) without exposing WebGPU headers.
struct DeviceNativeHandles {
    void* instance{nullptr};
    void* adapter{nullptr};
    void* device{nullptr};
    void* queue{nullptr};
};

class Device {
public:
    virtual ~Device() = default;

    [[nodiscard]] virtual const AdapterInfo& GetAdapterInfo() const noexcept = 0;
    [[nodiscard]] virtual const Limits& GetLimits() const noexcept = 0;


    [[nodiscard]] virtual DeviceNativeHandles GetNative() const noexcept = 0;

    virtual void Tick() const noexcept = 0;

    [[nodiscard]] static result<ref<Device>> Create(const DeviceDesc& desc = {}) noexcept;

protected:
    Device() = default;
};

} // namespace ct::gfx
