#pragma once
#include "ct/gfx/api/device.hpp"
#include <vulkan/vulkan.h>

namespace ct::gfx::vk {
class VKDeviceImpl final : public Device {
public:
    VKDeviceImpl(const DeviceInfo& info);
    ~VKDeviceImpl() override;


private:
    DeviceInfo mInfo;
};

}
