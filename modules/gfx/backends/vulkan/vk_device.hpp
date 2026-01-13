#pragma once

#include "ct/gfx/api/device.hpp"
#include <vulkan/vulkan.h>

namespace ct::gfx::vk {

class VKDeviceImpl final: public Device {
public:
    ~VKDeviceImpl() override;

    static result<ref<Device>> Create(const DeviceInfo& info);

private:
    explicit VKDeviceImpl(const DeviceInfo& info);

    VkInstance CreateInstance();

private:
    DeviceInfo mInfo;
    VkInstance mInstance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT mDebugMessenger{VK_NULL_HANDLE};
};

} 
