#pragma once
#include "ct/base/base.hpp"

namespace ct::gfx {
enum class DeviceBackend : u8 {
    Vulkan,
    Metal
};


struct DeviceInfo{
    DeviceBackend backend{DeviceBackend::Vulkan};

};

class Device{
public:

    virtual ~Device() = default;




    static result<ref<Device>> Create(const DeviceInfo& info);
protected:
    Device() = default;


};




}
