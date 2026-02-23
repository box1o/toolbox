#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/common.hpp>

namespace ct::gfx {

enum class Backend : u8 { Auto = 0, Vulkan, DirectX12, Metal, OpenGL, WebGPU };
enum class PowerPreference : u8 { LowPower = 0, HighPerformance };

struct DeviceDesc {
    Backend backend{Backend::Auto};
    PowerPreference powerPreference{PowerPreference::HighPerformance};
    bool enableValidation{true};
};

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
