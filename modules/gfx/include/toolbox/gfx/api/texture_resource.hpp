#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/texture.hpp>

namespace ct::gfx {

class Device;

enum class TextureUsageFlags : u32 {
    None         = 0,
    Sampled      = 1u << 0,
    Storage      = 1u << 1,
    RenderTarget = 1u << 2,
    CopySrc      = 1u << 3,
    CopyDst      = 1u << 4,
};

[[nodiscard]] constexpr TextureUsageFlags operator|(TextureUsageFlags a, TextureUsageFlags b) noexcept {
    return (TextureUsageFlags)((u32)a | (u32)b);
}
constexpr TextureUsageFlags& operator|=(TextureUsageFlags& a, TextureUsageFlags b) noexcept { a = a | b; return a; }
[[nodiscard]] constexpr bool HasFlag(TextureUsageFlags v, TextureUsageFlags f) noexcept {
    return ((u32)v & (u32)f) != 0u;
}

struct TextureDesc {
    u32 width{1};
    u32 height{1};
    u32 mipLevels{1};
    TextureFormat format{TextureFormat::RGBA8Unorm};
    TextureUsageFlags usage{TextureUsageFlags::Sampled | TextureUsageFlags::CopyDst};
    std::string debugName{"Texture"};
};

struct TextureViewDesc {
    std::string debugName{"TextureView"};
};

struct TextureFromFileDesc {
    TextureUsageFlags usage{TextureUsageFlags::Sampled | TextureUsageFlags::CopyDst};
    bool srgb{true};
    std::string debugName{"TextureFromFile"};
};

class TextureView {
public:
    virtual ~TextureView() = default;

    // raw backend handle (optional use)
    [[nodiscard]] virtual void* GetNativeView() const noexcept = 0;

    // IMPORTANT: pointer to backend C++ wrapper (used by RenderPassDesc)
    [[nodiscard]] virtual void* GetNativeViewPtr() const noexcept = 0;

protected:
    TextureView() = default;
};

class Texture {
public:
    virtual ~Texture() = default;

    [[nodiscard]] virtual u32 GetWidth() const noexcept = 0;
    [[nodiscard]] virtual u32 GetHeight() const noexcept = 0;
    [[nodiscard]] virtual TextureFormat GetFormat() const noexcept = 0;

    [[nodiscard]] virtual void* GetNativeTexture() const noexcept = 0;

    [[nodiscard]] virtual result<ref<TextureView>> CreateView(const TextureViewDesc& desc = {}) noexcept = 0;

    [[nodiscard]] static result<ref<Texture>> Create(ref<Device> device, const TextureDesc& desc) noexcept;
    [[nodiscard]] static result<ref<Texture>> FromFile(ref<Device> device, const std::string& path, const TextureFromFileDesc& desc = {}) noexcept;

protected:
    Texture() = default;
};

} // namespace ct::gfx
