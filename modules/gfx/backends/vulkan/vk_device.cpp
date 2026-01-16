#include "vk_device.hpp"
#include "vk_utils.hpp"

#include <ct/base/base.hpp>
#include <vulkan/vulkan_core.h>

#include <vector>

namespace ct::gfx::vk {

static const char* ToString(QueueType t) noexcept {
    switch (t) {
    case QueueType::Graphics: return "Graphics";
    case QueueType::Compute:  return "Compute";
    case QueueType::Transfer: return "Transfer";
    case QueueType::Present:  return "Present";
    }
    return "Unknown";
}

result<ref<Device>> VKDeviceImpl::Create(const DeviceInfo& info) {
    ref<VKDeviceImpl> device(new VKDeviceImpl(info));

    device->mInstance = device->CreateInstance();
    if (device->mInstance == VK_NULL_HANDLE) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
                   "Failed to create Vulkan instance");
    }

    if (info.validate) {
        detail::SetupDebugMessenger(device->mInstance, device->mDebugMessenger, info);
    }

    device->mPhysicalDevice = device->PickPhysicalDevice();
    if (device->mPhysicalDevice == VK_NULL_HANDLE) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
                   "Failed to pick Vulkan physical device");
    }

    device->queueFamilyIndices_ = device->QueryQueueFamilies(device->mPhysicalDevice);
    if (!device->queueFamilyIndices_.HasGraphics() || !device->queueFamilyIndices_.IsComplete()) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
                   "Failed to resolve required Vulkan queue families");
    }

    // NOTE: Logical device + vkGetDeviceQueue not shown in your snippet.
    // graphicsQueue_/computeQueue_/transferQueue_ will remain VK_NULL_HANDLE until you create VkDevice.
    // This code is now safe against optional access, but queue handles will be null until device creation.

    return device;
}

VKDeviceImpl::VKDeviceImpl(const DeviceInfo& info)
    : mInfo(info) {}

VKDeviceImpl::~VKDeviceImpl() {
    detail::DestroyDebugMessenger(mInstance, mDebugMessenger);

    if (mInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(mInstance, nullptr);
        mInstance = VK_NULL_HANDLE;
    }
}

VkInstance VKDeviceImpl::CreateInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = mInfo.name.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(mInfo.major, mInfo.minor, mInfo.patch);
    appInfo.pEngineName = "toolbox";
    appInfo.engineVersion = VK_MAKE_VERSION(mInfo.major, mInfo.minor, mInfo.patch);
    appInfo.apiVersion = detail::PickInstanceApiVersion();

    auto layers = detail::CollectValidationLayers(mInfo.validate);
    auto exts   = detail::CollectInstanceExtensions(mInfo.validate);

    VkDebugUtilsMessengerCreateInfoEXT dbgCi{};
    if (mInfo.validate) {
        detail::PopulateDebugCreateInfo(dbgCi, mInfo);
    }

    VkInstanceCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo = &appInfo;
    ci.enabledLayerCount = static_cast<u32>(layers.size());
    ci.ppEnabledLayerNames = layers.data();
    ci.enabledExtensionCount = static_cast<u32>(exts.size());
    ci.ppEnabledExtensionNames = exts.data();
    ci.pNext = mInfo.validate ? &dbgCi : nullptr;

    VkInstance instance = VK_NULL_HANDLE;
    const VkResult res = vkCreateInstance(&ci, nullptr, &instance);
    if (res != VK_SUCCESS) {
        log::Critical("[vk] vkCreateInstance failed: {}", static_cast<u32>(res));
        return VK_NULL_HANDLE;
    }

    log::Info("[vk] Vulkan Instance created");
    return instance;
}

VkPhysicalDevice VKDeviceImpl::PickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        log::Critical("[vk] No Vulkan physical devices found.");
        return VK_NULL_HANDLE;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    u32 bestScore = 0;

    for (VkPhysicalDevice dev : devices) {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(dev, &props);

        if (!detail::IsDeviceSuitable(dev)) {
            log::Info("[vk] Device '{}' rejected (missing required capabilities).", props.deviceName);
            continue;
        }

        const u32 score = detail::ScorePhysicalDevice(dev);
        if (score > bestScore) {
            bestScore = score;
            bestDevice = dev;
        }
    }

    if (bestDevice == VK_NULL_HANDLE) {
        log::Critical("[vk] No suitable Vulkan device found.");
        return VK_NULL_HANDLE;
    }

    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(bestDevice, &props);
    log::Info("[vk] Selected physical device: {}", props.deviceName);

    return bestDevice;
}

QueueFamilyIndices VKDeviceImpl::QueryQueueFamilies(VkPhysicalDevice device) {
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    if (queueFamilyCount == 0) {
        log::Error("[vk] Physical device reports 0 queue families.");
        return {};
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    std::optional<u32> graphics;
    std::optional<u32> computeDedicated;
    std::optional<u32> transferDedicated;

    for (u32 i = 0; i < queueFamilyCount; ++i) {
        const auto& q = queueFamilies[i];
        if (q.queueCount == 0) {
            continue;
        }

        const bool hasGraphics = (q.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
        const bool hasCompute  = (q.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
        const bool hasTransfer = (q.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0;

        if (hasGraphics && !graphics) {
            graphics = i;
        }

        // Dedicated compute: compute but NOT graphics
        if (hasCompute && !hasGraphics && !computeDedicated) {
            computeDedicated = i;
        }

        // Dedicated transfer: transfer but NOT graphics and NOT compute
        if (hasTransfer && !hasGraphics && !hasCompute && !transferDedicated) {
            transferDedicated = i;
        }
    }

    QueueFamilyIndices out{};
    out.graphics = graphics;

    if (!out.graphics) {
        log::Error("[vk] No graphics queue family found.");
        return {};
    }

    // Fallback strategy: compute/transfer fall back to graphics if not dedicated.
    out.compute  = computeDedicated.has_value() ? computeDedicated : out.graphics;
    out.transfer = transferDedicated.has_value() ? transferDedicated : out.graphics;

    log::Info("[vk] Queue families resolved: graphics={} compute={} transfer={}",
              out.graphics.value(),
              out.compute.value(),
              out.transfer.value());

    return out;
}

bool VKDeviceImpl::EnsureQueuesResolved() const noexcept {
    if (!queueFamilyIndices_.graphics.has_value()) {
        log::Critical("[vk] Queue family indices are not initialized.");
        return false;
    }
    if (!queueFamilyIndices_.compute.has_value()) {
        log::Critical("[vk] Compute queue family index is not initialized.");
        return false;
    }
    if (!queueFamilyIndices_.transfer.has_value()) {
        log::Critical("[vk] Transfer queue family index is not initialized.");
        return false;
    }
    return true;
}

u32 VKDeviceImpl::GetQueueFamilyIndex(QueueType type) const {
    if (type == QueueType::Present) {
        log::Critical("[vk] Present queue depends on VkSurfaceKHR; resolve in Surface/Swapchain layer.");
        std::abort();
    }

    if (!EnsureQueuesResolved()) {
        log::Critical("[vk] GetQueueFamilyIndex({}) called before queue families were resolved.", ToString(type));
        std::abort();
    }

    switch (type) {
    case QueueType::Graphics: return queueFamilyIndices_.graphics.value();
    case QueueType::Compute:  return queueFamilyIndices_.compute.value();
    case QueueType::Transfer: return queueFamilyIndices_.transfer.value();
    case QueueType::Present:  break;
    }

    log::Critical("[vk] Invalid QueueType.");
    std::abort();
}

void* VKDeviceImpl::GetQueueHandle(QueueType type) const {
    if (type == QueueType::Present) {
        log::Critical("[vk] Present queue depends on VkSurfaceKHR; resolve in Surface/Swapchain layer.");
        std::abort();
    }

    // NOTE: These are VK_NULL_HANDLE until you create VkDevice and call vkGetDeviceQueue.
    switch (type) {
    case QueueType::Graphics: return reinterpret_cast<void*>(graphicsQueue_);
    case QueueType::Compute:  return reinterpret_cast<void*>(computeQueue_);
    case QueueType::Transfer: return reinterpret_cast<void*>(transferQueue_);
    case QueueType::Present:  break;
    }

    log::Critical("[vk] Invalid QueueType.");
    std::abort();
}

} 
