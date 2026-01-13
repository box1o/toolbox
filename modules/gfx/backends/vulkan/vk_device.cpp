#include "vk_device.hpp"
#include "vk_utils.hpp"

#include <ct/base/base.hpp>

namespace ct::gfx::vk {

result<ref<Device>> VKDeviceImpl::Create(const DeviceInfo& info) {
    ref<VKDeviceImpl> device(new VKDeviceImpl(info));

    device->mInstance = device->CreateInstance();
    if (device->mInstance == VK_NULL_HANDLE) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Failed to create Vulkan Instance"
        );
    }


    if (info.validate) {
        detail::SetupDebugMessenger(
            device->mInstance,
            device->mDebugMessenger,
            info
        );
    }



    device->mPhysicalDevice = device->PickPhysicalDevice();
    if (device->mPhysicalDevice == VK_NULL_HANDLE) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Failed to pick Vulkan Physical Device"
        );
    }




    return device;
}

VKDeviceImpl::VKDeviceImpl(const DeviceInfo& info)
: mInfo(info) {}

VKDeviceImpl::~VKDeviceImpl() {
    detail::DestroyDebugMessenger( mInstance, mDebugMessenger);

    if (mInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(mInstance, nullptr);
        mInstance = VK_NULL_HANDLE;
    }
}

VkInstance VKDeviceImpl::CreateInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = mInfo.name.c_str();
    appInfo.applicationVersion =
        VK_MAKE_VERSION(mInfo.major, mInfo.minor, mInfo.patch);
    appInfo.pEngineName = "toolbox";
    appInfo.engineVersion =
        VK_MAKE_VERSION(mInfo.major, mInfo.minor, mInfo.patch);
    appInfo.apiVersion = detail::PickInstanceApiVersion();

    auto layers = detail::CollectValidationLayers(mInfo.validate);
    auto exts   = detail::CollectInstanceExtensions(mInfo.validate);

    // Debug stuff 
    VkDebugUtilsMessengerCreateInfoEXT dbgCi{};
    if (mInfo.validate) {
        detail::PopulateDebugCreateInfo(dbgCi, mInfo);
    }

    VkInstanceCreateInfo ci{};
    ci.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo        = &appInfo;
    ci.enabledLayerCount       = static_cast<u32>(layers.size());
    ci.ppEnabledLayerNames     = layers.data();
    ci.enabledExtensionCount   = static_cast<u32>(exts.size());
    ci.ppEnabledExtensionNames = exts.data();
    ci.pNext = mInfo.validate? &dbgCi : nullptr;

    VkInstance instance = VK_NULL_HANDLE;
    VkResult res = vkCreateInstance(&ci, nullptr, &instance);
    if (res != VK_SUCCESS) {
        log::Critical( "vkCreateInstance failed: {}", static_cast<u32>(res)
                      );
        return VK_NULL_HANDLE;
    }

    log::Info("Vulkan Instance created");
    return instance;
}




VkPhysicalDevice VKDeviceImpl::PickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        log::Critical("[vk] No Vulkan physical devices found.");
        std::abort();
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    u32 bestScore = 0;

    for (VkPhysicalDevice dev : devices) {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(dev, &props);

        if (!detail::IsDeviceSuitable(dev)) {
            log::Info( "[vk] Device '{}' rejected (missing required capabilities).", props.deviceName); 
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








}
