#include "vk_device.hpp"

namespace ct::gfx::vk {

VKDeviceImpl::VKDeviceImpl(const DeviceInfo& info)
    : mInfo(info) {
    log::Info("Vulkan Device created");
}

VKDeviceImpl::~VKDeviceImpl(){}


}
