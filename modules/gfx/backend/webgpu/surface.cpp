#include "surface.hpp"
#include "toolbox/base/logger/logger.hpp"

namespace ct::gfx {

bool WGPUSurfaceImpl::CreateSurface(const wgpu::Instance& instance, ref<Window> window) noexcept {
    if (!instance) {
        log::Error("WGPUSurfaceImpl::CreateSurface: instance is null");
        return false;
    }
    if (!window) {
        log::Error("WGPUSurfaceImpl::CreateSurface: window is null");
        return false;
    }

    mSurface = detail::CreateWindowNativeSurface(instance, window);
    if (!mSurface) {
        log::Error("Failed to create native surface");
        return false;
    }
    return true;
}

} // namespace ct::gfx
