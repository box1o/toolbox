#pragma once

#include <toolbox/base/base.hpp>
#include <webgpu/webgpu_cpp.h>

#include "toolbox/gfx/window/window.hpp"
#include "toolbox/gfx/api/device.hpp"

namespace ct::gfx {

class Surface {
public:
    virtual ~Surface() = default;


    // [[nodiscard]] virtual TextureFormat GetFormat() const noexcept = 0;
    // [[nodiscard]] virtual TextureFormat GetDepthFormat() const noexcept = 0;
    // [[nodiscard]] virtual bool HasDepth() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetWidth() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetHeight() const noexcept = 0;
    // [[nodiscard]] virtual f32 GetAspectRatio() const noexcept = 0;
    // [[nodiscard]] virtual PresentMode GetPresentMode() const noexcept = 0;

    [[nodiscard]] virtual void* GetSurface() const noexcept = 0;

    [[nodiscard]] static result<ref<Surface>> Create(ref<Device> device, ref<Window> window) noexcept;

protected:
    Surface() = default;
};

} // namespace ct::gfx
