#pragma once
#include "common.hpp"

#include <toolbox/base/base.hpp>

namespace ct::gfx {
enum class PowerProfile : u8 {
    LowPower,
    HighPerformance,
};

struct DeviceDesc {
    PowerProfile powerPreference{PowerProfile::HighPerformance};
};

class Device {
public:
    virtual ~Device() = default;

    [[nodiscard]] virtual Limits& GetLimits() noexcept = 0;
    [[nodiscard]] virtual AdapterInfo& GetAdapterInfo() noexcept = 0;

    [[nodiscard]] virtual void* GetNativeInstanceHandle() noexcept = 0;
    [[nodiscard]] virtual void* GetNativeDeviceHandle() noexcept = 0;
    [[nodiscard]] virtual void* GetNativeAdapterHandle() noexcept = 0;
    [[nodiscard]] virtual void* GetNativeQueueHandle() noexcept = 0;

    virtual void Tick() const noexcept {
    } // For some backends, we may need to tick the device to process events, etc.

    [[nodiscard]] static result<ref<Device>> Create(const DeviceDesc& desc = {}) noexcept;

protected:
    virtual result<void> Initialize() noexcept = 0;
    Device() = default;

};

} // namespace ct::gfx
