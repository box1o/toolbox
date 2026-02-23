#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

class Device;
class Window;

struct SurfaceDesc {
    // Placeholder for future surface configuration options
};

class Surface {
public:
    virtual ~Surface() = default;

    [[nodiscard]] virtual void* GetNativeSurface() const noexcept = 0;

    [[nodiscard]] static result<ref<Surface>> Create(
        ref<Device> device, ref<Window> window, const SurfaceDesc& desc = {}) noexcept;

protected:
    Surface() = default;
};

} // namespace ct::gfx
