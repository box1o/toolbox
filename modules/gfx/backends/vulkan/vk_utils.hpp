#pragma once

#include "ct/gfx/api/device.hpp"
#include <ct/base/types/types.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace ct::gfx::vk::detail {

u32 PickInstanceApiVersion();
bool HasExtensionSupport(const char* extensionName);



bool IsInstanceLayerAvailable(const char* name);
bool IsInstanceExtensionAvailable(const char* name);
bool IsDeviceExtensionAvailable(VkPhysicalDevice device, const char* name);


std::vector<const char*> CollectValidationLayers(bool enable);
std::vector<const char*> CollectInstanceExtensions(bool validation);


void PopulateDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& ci, const DeviceInfo& info) ;

}
