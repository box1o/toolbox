#pragma once
#include <filesystem>
#include <toolbox/base/base.hpp>

#include <span>

using Path = std::filesystem::path;

namespace ct::gfx {


enum class TextureFormat : u16 {
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

enum class TextureUsageFlags : u32 {
    None = 0,
    Sampled = 1u << 0,      // shader read
    Storage = 1u << 1,      // shader read/write
    RenderTarget = 1u << 2, // render attachment
    CopySrc = 1u << 3,
    CopyDst = 1u << 4,
};

// bit ops
// clang-format off
[[nodiscard]] constexpr TextureUsageFlags operator|(TextureUsageFlags a, TextureUsageFlags b) noexcept { return static_cast<TextureUsageFlags>(static_cast<u32>(a) | static_cast<u32>(b)); } constexpr TextureUsageFlags& operator|=(TextureUsageFlags& a, TextureUsageFlags b) noexcept { a = a | b; return a; } [[nodiscard]] constexpr bool HasFlag(TextureUsageFlags v, TextureUsageFlags f) noexcept { return (static_cast<u32>(v) & static_cast<u32>(f)) != 0u; }
// clang-format on

struct TextureDesc {
    u32 width{1};
    u32 height{1};
    u32 mipLevels{1};
    TextureFormat format{TextureFormat::RGBA8Unorm};
    TextureUsageFlags usage{TextureUsageFlags::Sampled | TextureUsageFlags::CopyDst};
};

struct TextureViewDesc {
    u32 baseMipLevel{0};
    u32 mipLevelCount{0};
};

class TextureView {
public:
    virtual ~TextureView() = default;
    [[nodiscard]] virtual void* GetNativeTextureViewHandle() noexcept = 0;

protected:
    TextureView() = default;
};



class Device;
class Texture {
public:
    virtual ~Texture() = default;

    // basic
    [[nodiscard]] virtual u32 GetWidth() const noexcept = 0;
    [[nodiscard]] virtual u32 GetHeight() const noexcept = 0;
    [[nodiscard]] virtual u32 GetMipLevels() const noexcept = 0;
    [[nodiscard]] virtual TextureFormat GetFormat() const noexcept = 0;
    [[nodiscard]] virtual TextureUsageFlags GetUsage() const noexcept = 0;

    [[nodiscard]] virtual void* GetNativeTextureHandle() noexcept = 0;

    // views
    // clang-format off
    [[nodiscard]] virtual result<ref<TextureView>> GetDefaultView() noexcept = 0;
    [[nodiscard]] virtual result<ref<TextureView>> CreateView( const TextureViewDesc& desc = {}) noexcept = 0;
    //clang-format on

    // upload
    virtual result<void> Update(const void* data, u64 numBytes) noexcept = 0;
    template <class T> result<void> Update(std::span<const T> src) noexcept { return Update(src.data(), static_cast<u64>(src.size_bytes())); }

    [[nodiscard]] static result<ref<Texture>> Create( ref<Device> device, const TextureDesc& desc) noexcept;
    [[nodiscard]] static result<ref<Texture>> FromFile( ref<Device> device, const Path& path , bool srgb = false , TextureUsageFlags usage = TextureUsageFlags::Sampled) noexcept;

protected:
    virtual result<void> Initialize() noexcept = 0;
    Texture() = default;
};

} // namespace ct::gfx
