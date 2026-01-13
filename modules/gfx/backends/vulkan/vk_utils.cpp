#include "vk_utils.hpp"
#include <ct/base/base.hpp>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstring>
#include <vector>

namespace ct::gfx::vk::detail {

u32 PickInstanceApiVersion() {
    u32 loaderVersion = VK_API_VERSION_1_0;
    auto* fn = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
        vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
    if (fn) {
        fn(&loaderVersion);
    }
    constexpr u32 requestedVersion = VK_API_VERSION_1_3;
    const u32 selectedVersion = (loaderVersion >= requestedVersion) ? requestedVersion : loaderVersion;
    log::Info("[vk] Selected API version: {}.{}.{}", VK_VERSION_MAJOR(selectedVersion), VK_VERSION_MINOR(selectedVersion), VK_VERSION_PATCH(selectedVersion));
    return selectedVersion;
}

bool IsInstanceLayerAvailable(const char* name) {
    if (!name) { return false; }

    u32 count = 0;
    if (vkEnumerateInstanceLayerProperties(&count, nullptr) != VK_SUCCESS) {
        return false;
    }

    if (count == 0) {
        return false;
    }

    std::vector<VkLayerProperties> layers(count);
    if (vkEnumerateInstanceLayerProperties(&count, layers.data()) != VK_SUCCESS) {
        return false;
    }

    return std::any_of(layers.begin(), layers.end(), [name](const VkLayerProperties& layer) {
        return std::strcmp(layer.layerName, name) == 0;
    });
}

bool IsInstanceExtensionAvailable(const char* name) {
    if (!name) { return false; }

    u32 count = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr) != VK_SUCCESS) { return false; }

    if (count == 0) { return false; }

    std::vector<VkExtensionProperties> extensions(count);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data()) != VK_SUCCESS) {
        return false;
    }

    return std::any_of(extensions.begin(), extensions.end(), 
                       [name](const VkExtensionProperties& ext) {
                       return std::strcmp(ext.extensionName, name) == 0;
                       });
}

bool IsDeviceExtensionAvailable(VkPhysicalDevice device, const char* name) {
    if (!device || !name) { return false; }

    u32 count = 0;
    if (vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr) != VK_SUCCESS) { return false; }

    if (count == 0) { return false; }

    std::vector<VkExtensionProperties> extensions(count);
    if (vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data()) != VK_SUCCESS) {
        return false;
    }

    return std::any_of(extensions.begin(), extensions.end(), 
                       [name](const VkExtensionProperties& ext) {
                       return std::strcmp(ext.extensionName, name) == 0;
                       });
}


namespace {
constexpr const char* kValidationLayers[] = {
    "VK_LAYER_KHRONOS_validation",
};
} 

std::vector<const char*> CollectValidationLayers(bool enable) {
    std::vector<const char*> layers;

    if (!enable) {
        log::Info("[vk] Validation layers disabled");
        return layers;
    }

    for (const char* layer : kValidationLayers) {
        if (IsInstanceLayerAvailable(layer)) {
            layers.push_back(layer);
        } else {
            log::Warn("[vk] Validation layer not available: {}", layer);
        }
    }

    if (layers.empty()) {
        log::Critical("[vk] Validation requested, but no validation layers are available");
        std::abort();
    }

    return layers;
}


namespace {

void AppendIfAvailable(std::vector<const char*>& out, const char* extension) {
    if (IsInstanceExtensionAvailable(extension)) {
        out.push_back(extension);
    } else {
        log::Debug("[vk] Instance extension not available: {}", extension);
    }
}

void AppendRequired(std::vector<const char*>& out, const char* extension) {
    if (IsInstanceExtensionAvailable(extension)) {
        out.push_back(extension);
    } else {
        log::Critical("[vk] Required instance extension not available: {}", extension);
        std::abort();
    }
}

} 

std::vector<const char*> CollectInstanceExtensions(bool validation) {

    std::vector<const char*> extensions;

    AppendRequired(extensions, VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(__linux__)

    // Wayland support
    AppendIfAvailable(extensions, "VK_KHR_wayland_surface");

    // X11 support (XCB and Xlib)
    AppendIfAvailable(extensions, "VK_KHR_xcb_surface");
    AppendIfAvailable(extensions, "VK_KHR_xlib_surface");

    // Headless rendering support (optional but useful)
    AppendIfAvailable(extensions, "VK_EXT_headless_surface");

    // Display extensions (for direct display access)
    AppendIfAvailable(extensions, "VK_KHR_display");
    AppendIfAvailable(extensions, "VK_EXT_direct_mode_display");
    AppendIfAvailable(extensions, "VK_EXT_acquire_drm_display");
    AppendIfAvailable(extensions, "VK_EXT_acquire_xlib_display");

#elif defined(__APPLE__)
    // Metal surface for macOS/iOS (MoltenVK)
    AppendRequired(extensions, "VK_EXT_metal_surface");
    // Portability enumeration (required for MoltenVK)
    AppendRequired(extensions, "VK_KHR_portability_enumeration");

#elif defined(_WIN32) || defined(_WIN64)
    // Win32 surface
    AppendRequired(extensions, "VK_KHR_win32_surface");
#elif defined(__ANDROID__)
    // Android surface
    AppendRequired(extensions, "VK_KHR_android_surface");
#else
    #warning "Unknown platform - surface extensions may not be configured correctly"
    log::Warn("[vk] Unknown platform detected");
#endif

    if (validation) {
        log::Info("[vk] Validation enabled, adding debug extensions...");

        // Debug utils (preferred modern debug extension)
        if (IsInstanceExtensionAvailable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            log::Info("[vk] Enabled debug extension: {}", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        } else {
            log::Critical("[vk] Validation requested, but {} is not available",
                          VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            std::abort();
        }

        // Debug report (legacy, but still useful as fallback)
        AppendIfAvailable(extensions, VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }


    // Physical device properties (useful for querying features)
    AppendIfAvailable(extensions, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    // External memory/sync capabilities
    AppendIfAvailable(extensions, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
    AppendIfAvailable(extensions, VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);
    AppendIfAvailable(extensions, VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME);

    // Enhanced surface capabilities
    AppendIfAvailable(extensions, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);

    // Surface maintenance (check if available, as these are newer)
    AppendIfAvailable(extensions, "VK_KHR_surface_maintenance1");
    AppendIfAvailable(extensions, "VK_EXT_surface_maintenance1");

    // Display surface counter (useful for presentation timing)
    AppendIfAvailable(extensions, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);

    // Swapchain colorspace (use string literal as constant may not be defined)
    AppendIfAvailable(extensions, "VK_EXT_swapchain_colorspace");

    // Device group support
    AppendIfAvailable(extensions, VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME);

    if (extensions.empty()) {
        log::Critical("[vk] No usable Vulkan instance extensions found");
        std::abort();
    }

    log::Info("[vk] Total instance extensions enabled: {}", extensions.size());

    return extensions;
}


// NOTE: Debug stuff 
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback_(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT /*type*/, const VkDebugUtilsMessengerCallbackDataEXT* cb,
    void* /*user*/) {
    const char* msg = (cb && cb->pMessage) ? cb->pMessage : "(null)";

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) { log::Error("[vk] {}", msg);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) { log::Warn("[vk] {}", msg);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) { log::Info("[vk] {}", msg);
    } else { log::Debug("[vk] {}", msg); }

    return VK_FALSE;
}

void PopulateDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& ci, const DeviceInfo& info) {
    ci = {};
    ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    VkDebugUtilsMessageSeverityFlagsEXT severityFlags = 0;
    if (info.verbose) {
        severityFlags |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    }

    severityFlags |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    ci.messageSeverity = severityFlags;
    ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    ci.pfnUserCallback = DebugCallback_;
    ci.pUserData = nullptr;
}



PFN_vkCreateDebugUtilsMessengerEXT LoadCreateDebugUtilsMessenger(VkInstance instance) {
    return reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
}

PFN_vkDestroyDebugUtilsMessengerEXT LoadDestroyDebugUtilsMessenger(VkInstance instance) {
    return reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
}




void SetupDebugMessenger(VkInstance mInstance, VkDebugUtilsMessengerEXT& mDebugMessenger, const DeviceInfo& mInfo) {
    if (mInstance == VK_NULL_HANDLE) {
        log::Critical("[vk] SetupDebugMessenger called with null VkInstance.");
        std::abort();
    }
    if (mDebugMessenger != VK_NULL_HANDLE) {
        return;
    }

    auto* fn = detail::LoadCreateDebugUtilsMessenger(mInstance);
    if (!fn) {
        log::Critical("[vk] vkCreateDebugUtilsMessengerEXT not present (missing extension?).");
        std::abort();
    }

    VkDebugUtilsMessengerCreateInfoEXT ci{};
    detail::PopulateDebugCreateInfo(ci, mInfo);

    const VkResult r = fn(mInstance, &ci, nullptr, &mDebugMessenger);
    if (r != VK_SUCCESS || mDebugMessenger == VK_NULL_HANDLE) {
        log::Critical("[vk] vkCreateDebugUtilsMessengerEXT failed: {}", static_cast<int>(r));
        std::abort();
    }

    log::Info("[vk] Vulkan debug messenger created successfully");
}



void DestroyDebugMessenger(VkInstance mInstance, VkDebugUtilsMessengerEXT& mDebugMessenger) {
    if (mInstance == VK_NULL_HANDLE || mDebugMessenger == VK_NULL_HANDLE) {
        return;
    }

    auto* fn = detail::LoadDestroyDebugUtilsMessenger(mInstance);
    if (fn) {
        fn(mInstance, mDebugMessenger, nullptr);
    }
    mDebugMessenger = VK_NULL_HANDLE;
}



bool HasQueueFamilyFlags(VkPhysicalDevice device, VkQueueFlags required) {
    u32 qfCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qfCount, nullptr);
    if (qfCount == 0) {
        return false;
    }

    std::vector<VkQueueFamilyProperties> qfps(qfCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &qfCount, qfps.data());

    for (u32 i = 0; i < qfCount; ++i) {
        if (qfps[i].queueCount == 0) {
            continue;
        }
        if ((qfps[i].queueFlags & required) == required) {
            return true;
        }
    }
    return false;
}

bool IsDeviceSuitable(VkPhysicalDevice device) {
    if (!HasQueueFamilyFlags(device, VK_QUEUE_GRAPHICS_BIT)) {
        return false;
    }
    if (!IsDeviceExtensionAvailable(device, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        return false;
    }
    return true;
}



u32 ScorePhysicalDevice(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties props{};
    VkPhysicalDeviceMemoryProperties mem{};
    vkGetPhysicalDeviceProperties(device, &props);
    vkGetPhysicalDeviceMemoryProperties(device, &mem);

    u32 score = 0;

    switch (props.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        score += 10000;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        score += 2000;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        score += 500;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        score += 100;
        break;
    default:
        break;
    }

    for (u32 i = 0; i < mem.memoryHeapCount; ++i) {
        if (mem.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            const u64 gib = mem.memoryHeaps[i].size / (1024ull * 1024ull * 1024ull);
            score += static_cast<u32>(gib) * 100;
        }
    }

    const u32 major = VK_VERSION_MAJOR(props.apiVersion);
    const u32 minor = VK_VERSION_MINOR(props.apiVersion);
    if (major > 1 || (major == 1 && minor >= 3)) {
        score += 500;
    }

    if (IsDeviceExtensionAvailable(device, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        score += 1000;
    }
    if (IsDeviceExtensionAvailable(device, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        score += 2000;
    }
    if (IsDeviceExtensionAvailable(device, VK_EXT_MESH_SHADER_EXTENSION_NAME)) {
        score += 1000;
    }

    return score;
}


} // namespace ct::gfx::vk::detail
