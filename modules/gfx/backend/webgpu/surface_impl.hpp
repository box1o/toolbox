#pragma once
#include <toolbox/gfx/api/surface.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class SurfaceImpl final : public Surface {
public:
    SurfaceImpl() = default;
    ~SurfaceImpl() override = default;

    bool Init(ref<Device> device, ref<Window> window, const SurfaceDesc& desc) noexcept;

    [[nodiscard]] void* GetNativeSurface() const noexcept override { return (void*)mSurface.Get(); }
    [[nodiscard]] const wgpu::Surface& SurfaceHandle() const noexcept { return mSurface; }

private:
    wgpu::Surface mSurface;
};

wgpu::Surface CreateNativeSurface(const wgpu::Instance& instance, ref<Window> window);

} // namespace ct::gfx::webgpu
