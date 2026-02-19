#pragma once
#include "texture.hpp"
#include <toolbox/gfx/api/surface.hpp>

namespace ct::gfx {

struct Frame {
    wgpu::Texture colorTexture{nullptr};
    wgpu::TextureView colorView{nullptr};

    // std::optional<wgpu::Texture> depthTexture{nullptr}; // optional (owned by Surface if
    // persistent) std::optional<wgpu::TextureView> depthView{nullptr};

    u32 width{0};
    u32 height{0};
};

enum class PresentMode : u8 {
    Immediate,
    VSync,
    Mailbox,
};

struct SwapchainDesc {
    PresentMode presentMode{PresentMode::VSync};
    TextureFormat preferredFormat{TextureFormat::BGRA8Unorm}; // hint
    bool enableDepth{true};
    TextureFormat depthFormat{TextureFormat::Depth24PlusStencil8};
};

class Swapchain {
public:
    virtual ~Swapchain() = default;

    void Resize(u32 width, u32 height);


    [[nodiscard]] virtual Frame AcquireNextFrame() noexcept = 0;
    virtual void Present() noexcept = 0;

    [[nodiscard]] static result<ref<Swapchain>> Create(
        ref<Surface> surface, ref<Device> device) noexcept;

protected:
    Swapchain() = default;
};

} // namespace ct::gfx
