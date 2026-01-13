#pragma once

#include "ct/gfx/api/device.hpp"
#include <vulkan/vulkan.h>

namespace ct::gfx::vk {

class VKDeviceImpl final: public Device {
public:
    ~VKDeviceImpl() override;

    void* GetInstanceHandle()       override{ return mInstance;};
    void* GetPhysicalDeviceHandle() override { return mPhysicalDevice;};
    void* GetDeviceHandle()         override { return nullptr;};

    static result<ref<Device>> Create(const DeviceInfo& info);
private:
    explicit VKDeviceImpl(const DeviceInfo& info);

    VkInstance CreateInstance();
    VkPhysicalDevice PickPhysicalDevice();

private:
    DeviceInfo mInfo;
    VkInstance mInstance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT mDebugMessenger{VK_NULL_HANDLE};

    VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
};

} 
