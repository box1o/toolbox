#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/types.hpp"

namespace ct {

class Window;
class Device;
class Texture;

enum class PresentMode : u8 {
    Immediate,
    VSync,
    Mailbox,
};

struct SurfaceInfo {
    PresentMode presentMode{PresentMode::VSync};
    bool enableDepth{true};
    TextureFormat depthFormat{TextureFormat::Depth24PlusStencil8};
};

struct Frame {
    wgpu::TextureView colorView{nullptr};
    wgpu::TextureView depthView{nullptr};
    u32 width{0};
    u32 height{0};
};

class Surface {
public:
    ~Surface();

    [[nodiscard]] result<Frame> BeginFrame() noexcept;
    void Present() noexcept;

    void Resize(u32 width, u32 height);

    [[nodiscard]] TextureFormat GetFormat() const noexcept;
    [[nodiscard]] TextureFormat GetDepthFormat() const noexcept;
    [[nodiscard]] bool HasDepth() const noexcept;
    [[nodiscard]] u32 GetWidth() const noexcept;
    [[nodiscard]] u32 GetHeight() const noexcept;
    [[nodiscard]] f32 GetAspectRatio() const noexcept;
    [[nodiscard]] PresentMode GetPresentMode() const noexcept;

    [[nodiscard]] wgpu::Surface GetHandle() const noexcept;

    [[nodiscard]] static result<ref<Surface>> Create(
        ref<Window> window, ref<Device> device, const SurfaceInfo& info = {}) noexcept;

private:
    Surface() = default;
    bool CreateNativeSurface(const Window& window, const Device& device);
    bool Configure(const Device& device, u32 width, u32 height);
    bool CreateDepthTexture(const Device& device, u32 width, u32 height);

    wgpu::Surface mSurface{nullptr};
    wgpu::TextureFormat mFormat{wgpu::TextureFormat::BGRA8Unorm};
    PresentMode mPresentMode{PresentMode::VSync};
    u32 mWidth{0};
    u32 mHeight{0};

    bool mDepthEnabled{false};
    TextureFormat mDepthFormat{TextureFormat::Depth24PlusStencil8};
    wgpu::Texture mDepthTexture{nullptr};
    wgpu::TextureView mDepthView{nullptr};

    weak<Device> mDevice;
    weak<Window> mWindow;
};

namespace detail {
[[nodiscard]] wgpu::Surface CreateWindowNativeSurface(
    const wgpu::Instance& instance, const Window& window);
}

} // namespace ct
