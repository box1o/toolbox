#pragma once

#include "enums.hpp"
#include <toolbox/base/base.hpp>

namespace ct::gfx {

class Device;
class Surface;

enum class PresentMode : u8 {
    Immediate,
    VSync,
    Mailbox,
};

struct SwapchainDesc {
    PresentMode presentMode{PresentMode::VSync};
    TextureFormat preferredFormat{TextureFormat::BGRA8Unorm};
    bool enableDepth{true};
    TextureFormat depthFormat{TextureFormat::Depth24PlusStencil8};
    std::string debugName{"Swapchain"};
};

struct Frame {
    void* colorTexture{nullptr};
    void* colorView{nullptr};
    void* depthTexture{nullptr};
    void* depthView{nullptr};
    u32 width{0};
    u32 height{0};
};

class Swapchain {
public:
    virtual ~Swapchain() = default;

    [[nodiscard]] virtual TextureFormat GetColorFormat() const noexcept = 0;
    [[nodiscard]] virtual TextureFormat GetDepthFormat() const noexcept = 0;
    [[nodiscard]] virtual bool HasDepth() const noexcept = 0;

    [[nodiscard]] virtual u32 GetWidth() const noexcept = 0;
    [[nodiscard]] virtual u32 GetHeight() const noexcept = 0;
    [[nodiscard]] virtual f32 GetAspectRatio() const noexcept = 0;

    virtual void Resize(u32 width, u32 height) noexcept = 0;

    [[nodiscard]] virtual result<Frame> AcquireNextFrame() noexcept = 0;
    virtual result<void> Present() noexcept = 0;

    [[nodiscard]] static result<ref<Swapchain>> Create(
        ref<Device> device, ref<Surface> surface, const SwapchainDesc& desc = {}) noexcept;

protected:
    Swapchain() = default;
};

} // namespace ct::gfx
