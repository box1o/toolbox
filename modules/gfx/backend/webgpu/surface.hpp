#pragma once
#include <toolbox/gfx/api/surface.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {

class WGPUSurfaceImpl final : public Surface {
public:
    WGPUSurfaceImpl() = default;
    ~WGPUSurfaceImpl() override = default;

    [[nodiscard]] void* GetSurface() const noexcept override { return (void*)mSurface.Get(); }

    bool CreateSurface(const wgpu::Instance& instance, ref<Window> window) noexcept;

private:
    wgpu::Surface mSurface;
};

namespace detail {

[[nodiscard]] wgpu::Surface CreateWindowNativeSurface(
    const wgpu::Instance& instance, ref<ct::gfx::Window> window);

} // namespace detail

} // namespace ct::gfx
