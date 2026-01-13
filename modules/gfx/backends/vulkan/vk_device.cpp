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

    return device;
}

VKDeviceImpl::VKDeviceImpl(const DeviceInfo& info)
: mInfo(info) {}

VKDeviceImpl::~VKDeviceImpl() {
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

    // ci.pNext = info_.validation ? &dbgCi : nullptr;

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

}
