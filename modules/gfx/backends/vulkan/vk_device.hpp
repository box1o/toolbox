#pragma once

#include "ct/gfx/api/device.hpp"

#include <optional>
#include <vulkan/vulkan.h>

namespace ct::gfx::vk {

struct QueueFamilyIndices {
    std::optional<u32> graphics{};
    std::optional<u32> compute{};
    std::optional<u32> transfer{};

    [[nodiscard]] bool HasGraphics() const noexcept { return graphics.has_value(); }
    [[nodiscard]] bool IsComplete() const noexcept {
        return graphics.has_value() && compute.has_value() && transfer.has_value();
    }
};

class VKDeviceImpl final : public Device {
public:
    VKDeviceImpl(const DeviceInfo& info);
    ~VKDeviceImpl() override;

    void* GetInstanceHandle() override { return mInstance; }
    void* GetPhysicalDeviceHandle() override { return mPhysicalDevice; }
    void* GetDeviceHandle() override { return nullptr; }

    [[nodiscard]] void* GetQueueHandle(QueueType type) const override;
    [[nodiscard]] u32 GetQueueFamilyIndex(QueueType type) const override;

    [[nodiscard]] VkQueue GetGraphicsQueue() const { return graphicsQueue_; }
    [[nodiscard]] VkQueue GetComputeQueue() const { return computeQueue_; }
    [[nodiscard]] VkQueue GetTransferQueue() const { return transferQueue_; }


private:

    VkInstance CreateInstance();
    VkPhysicalDevice PickPhysicalDevice();
    QueueFamilyIndices QueryQueueFamilies(VkPhysicalDevice device);

    [[nodiscard]] bool EnsureQueuesResolved() const noexcept;

private:
    DeviceInfo mInfo{};
    VkInstance mInstance{ VK_NULL_HANDLE };
    VkDebugUtilsMessengerEXT mDebugMessenger{ VK_NULL_HANDLE };

    VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };

    QueueFamilyIndices queueFamilyIndices_{};
    VkQueue graphicsQueue_{ VK_NULL_HANDLE };
    VkQueue computeQueue_{ VK_NULL_HANDLE };
    VkQueue transferQueue_{ VK_NULL_HANDLE };
};

} 
