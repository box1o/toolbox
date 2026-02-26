#pragma once

#include <toolbox/base/base.hpp>

namespace ct::gfx {

enum class TextureFormat : u8 {
    Undefined = 0,

    RGBA8Unorm,
    RGBA8UnormSrgb,
    BGRA8Unorm,
    BGRA8UnormSrgb,

    Depth16Unorm,
    Depth24Plus,
    Depth24PlusStencil8,
    Depth32Float,
    Depth32FloatStencil8,
};

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
    TextureFormat colorFormat{TextureFormat::BGRA8Unorm};
    TextureFormat depthFormat{TextureFormat::Depth24PlusStencil8};
    bool enableDepth{true};
};

// fwd
class Device;
class Window;
class Swapchain {
public:
    virtual ~Swapchain() = default;

    [[nodiscard]] virtual TextureFormat GetColorFormat() const noexcept = 0;
    [[nodiscard]] virtual TextureFormat GetDepthFormat() const noexcept = 0;
    // [[nodiscard]] virtual bool HasDepth() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetWidth() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetHeight() const noexcept = 0;
    // [[nodiscard]] virtual f32 GetAspectRatio() const noexcept = 0;

    virtual void Resize(u32 width, u32 height) noexcept = 0;

    [[nodiscard]] virtual void* GetNativeSurfaceHandle() noexcept = 0;

    [[nodiscard]] virtual result<Frame> AcquireNextFrame() noexcept = 0;
    virtual result<void> Present() noexcept = 0;

    [[nodiscard]] static result<ref<Swapchain>> Create(
        ref<Device> device, ref<Window> window, const SwapchainDesc& desc = {}) noexcept;

protected:
    virtual result<void> Initialize() noexcept = 0;
    Swapchain() = default;
};

} // namespace ct::gfx
