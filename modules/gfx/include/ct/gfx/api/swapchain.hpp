#pragma once
#include "ct/base/base.hpp"

#include "ct/gfx/api/device.hpp"
#include "ct/gfx/window/window.hpp"


namespace ct::gfx {


struct SwapchainInfo {
};


class Swapchain {
public:


static result<scope<Swapchain>> Create(weak<Window> window, weak<Device> device,const SwapchainInfo& info);
private:

};

}

