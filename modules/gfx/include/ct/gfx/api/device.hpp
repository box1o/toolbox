#pragma once

#include "ct/base/base.hpp"

namespace ct::gfx {

enum class DeviceBackend : u8 {
    Vulkan,
    Metal
};

struct DeviceInfo {
    bool validate{true};
    bool verbose{false};
    u32 major{1};
    u32 minor{0};
    u32 patch{0};
    std::string name{"gfx"};
    DeviceBackend backend{DeviceBackend::Vulkan};
};


enum class QueueType : u8 {
    Graphics,
    Compute,
    Transfer,
    Present,
};

class Device {
public:
    virtual ~Device() = default;

    [[nodiscard]] virtual void* GetInstanceHandle() = 0;
    [[nodiscard]] virtual void* GetPhysicalDeviceHandle() = 0;
    [[nodiscard]] virtual void* GetDeviceHandle() = 0;

    [[nodiscard]] virtual void* GetQueueHandle(QueueType type) const = 0;
    [[nodiscard]] virtual u32 GetQueueFamilyIndex(QueueType type) const = 0;



    static ref<Device> Create(const DeviceInfo& info);

protected:
    Device() = default;
};

}
