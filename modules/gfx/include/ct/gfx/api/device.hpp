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

class Device {
public:
    virtual ~Device() = default;

    virtual void* GetInstanceHandle() = 0;
    virtual void* GetPhysicalDeviceHandle() = 0;
    virtual void* GetDeviceHandle() = 0;

    static result<ref<Device>> Create(const DeviceInfo& info);

protected:
    Device() = default;
};

}
