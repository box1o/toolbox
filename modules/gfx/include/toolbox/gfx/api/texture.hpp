#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/types.hpp"
#include <filesystem>

namespace ct {

struct TextureInfo {
    u32 width{1};
    u32 height{1};
    u32 depth{1};
    u32 mipLevels{1};
    u32 arrayLayers{1};
    TextureFormat format{TextureFormat::RGBA8Unorm};
    TextureDimension dimension{TextureDimension::D2};
    TextureUsageFlags usage{TextureUsageFlags::Sampled | TextureUsageFlags::CopyDst};
};

struct TextureLoadInfo {
    bool flipOnLoad{true};
    bool forceRGBA{true};
};

class Device;

class Texture {
public:
    ~Texture();

    [[nodiscard]] u32 GetWidth() const noexcept;
    [[nodiscard]] u32 GetHeight() const noexcept;
    [[nodiscard]] u32 GetDepth() const noexcept;
    [[nodiscard]] u32 GetMipLevels() const noexcept;
    [[nodiscard]] u32 GetArrayLayers() const noexcept;
    [[nodiscard]] TextureFormat GetFormat() const noexcept;
    [[nodiscard]] TextureDimension GetDimension() const noexcept;
    [[nodiscard]] TextureUsageFlags GetUsage() const noexcept;
    [[nodiscard]] const TextureInfo& GetInfo() const noexcept;

    [[nodiscard]] wgpu::Texture GetHandle() const noexcept;
    [[nodiscard]] wgpu::TextureView GetView() const noexcept;

    void Write(const void* data, u64 dataSize, u32 mipLevel = 0, u32 arrayLayer = 0);

    [[nodiscard]] static result<ref<Texture>> Create(
        ref<Device> device, const TextureInfo& info,
        const void* data = nullptr, u64 dataSize = 0) noexcept;

    [[nodiscard]] static result<ref<Texture>> FromFile(
        ref<Device> device,
        const std::filesystem::path& path,
        const TextureLoadInfo& loadInfo = {}) noexcept;

private:
    Texture() = default;
    bool Init(const Device& device, const TextureInfo& info);
    bool CreateView();

    TextureInfo mInfo{};
    weak<Device> mDevice;
    wgpu::Texture mTexture{nullptr};
    wgpu::TextureView mView{nullptr};
};

} // namespace ct
