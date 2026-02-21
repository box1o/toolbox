#pragma once
#include "texture.hpp"

namespace ct::gfx {

struct Frame {
    void* colorTexture{nullptr};
    void* colorView{nullptr};
    void* depthTexture{nullptr};
    void* depthView{nullptr};

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
    TextureFormat preferredFormat{TextureFormat::BGRA8Unorm};
    bool enableDepth{true};
    TextureFormat depthFormat{TextureFormat::Depth24PlusStencil8};
};
class Device;
class Window;
class Swapchain {
public:
    virtual ~Swapchain() = default;

    void Resize(u32 width, u32 height);

    // [[nodiscard]] virtual TextureFormat GetFormat() const noexcept = 0;
    // [[nodiscard]] virtual TextureFormat GetDepthFormat() const noexcept = 0;
    // [[nodiscard]] virtual bool HasDepth() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetWidth() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetHeight() const noexcept = 0;
    // [[nodiscard]] virtual f32 GetAspectRatio() const noexcept = 0;
    // [[nodiscard]] virtual PresentMode GetPresentMode() const noexcept = 0;

    [[nodiscard]] virtual void* GetSurface() const noexcept = 0;

    [[nodiscard]] virtual Frame AcquireNextFrame() noexcept = 0;
    virtual void Present() noexcept = 0;

    [[nodiscard]] static result<ref<Swapchain>> Create(
        ref<Device> device, ref<Window> window) noexcept;

protected:
    Swapchain() = default;
};

} // namespace ct::gfx
