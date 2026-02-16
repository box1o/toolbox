#include "toolbox/gfx/api/texture.hpp"
#include "toolbox/gfx/api/device.hpp"
#include <cstring>
#include <vector>

#include <stb_image.h>

namespace ct {

result<ref<Texture>> Texture::Create(
    ref<Device> device, const TextureInfo& info, const void* data, u64 dataSize) noexcept {

    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");
    if (info.width == 0 || info.height == 0) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Texture dimensions cannot be zero");
    }

    ref<Texture> tex(new Texture());
    tex->mDevice = device;

    if (!tex->Init(*device, info)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create texture");
    }
    if (!tex->CreateView()) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create texture view");
    }

    if (data && dataSize > 0) {
        tex->Write(data, dataSize);
    }

    log::Info("[wgpu] Texture created ({}x{} {}mip)", info.width, info.height, info.mipLevels);
    return tex;
}

result<ref<Texture>> Texture::FromFile(ref<Device> device, const std::filesystem::path& path,
    const TextureLoadInfo& loadInfo) noexcept {

    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");

    stbi_set_flip_vertically_on_load(loadInfo.flipOnLoad ? 1 : 0);

    int w = 0, h = 0, channels = 0;
    int desiredChannels = loadInfo.forceRGBA ? 4 : 0;

    stbi_uc* pixels = stbi_load(path.string().c_str(), &w, &h, &channels, desiredChannels);
    if (!pixels) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE,
            "Failed to load texture: " + std::string(stbi_failure_reason()));
    }

    int actualChannels = loadInfo.forceRGBA ? 4 : channels;
    u64 dataSize = static_cast<u64>(w) * static_cast<u64>(h) * static_cast<u64>(actualChannels);

    TextureInfo info{};
    info.width = static_cast<u32>(w);
    info.height = static_cast<u32>(h);
    info.format = ChannelsToFormat(actualChannels);
    info.usage = TextureUsageFlags::Sampled | TextureUsageFlags::CopyDst;

    auto result = Create(device, info, pixels, dataSize);

    stbi_image_free(pixels);
    stbi_set_flip_vertically_on_load(0);

    return result;
}

Texture::~Texture() {
    mView = nullptr;
    if (mTexture) {
        mTexture.Destroy();
        mTexture = nullptr;
    }
}

bool Texture::Init(const Device& device, const TextureInfo& info) {
    mInfo = info;

    wgpu::TextureDescriptor desc{};
    desc.size = {info.width, info.height, info.depth};
    desc.mipLevelCount = info.mipLevels;
    desc.sampleCount = 1;
    desc.dimension = ToWGPUDimension(info.dimension);
    desc.format = ToWGPU(info.format);
    desc.usage = ToWGPUUsage(info.usage);

    mTexture = device.GetDevice().CreateTexture(&desc);
    return mTexture != nullptr;
}

bool Texture::CreateView() {
    wgpu::TextureViewDescriptor viewDesc{};
    viewDesc.format = ToWGPU(mInfo.format);
    viewDesc.mipLevelCount = mInfo.mipLevels;
    viewDesc.arrayLayerCount = mInfo.arrayLayers;

    switch (mInfo.dimension) {
    case TextureDimension::D2:
        viewDesc.dimension = wgpu::TextureViewDimension::e2D;
        break;
    case TextureDimension::D3:
        viewDesc.dimension = wgpu::TextureViewDimension::e3D;
        break;
    case TextureDimension::Cube:
        viewDesc.dimension = wgpu::TextureViewDimension::Cube;
        break;
    case TextureDimension::Array2D:
        viewDesc.dimension = wgpu::TextureViewDimension::e2DArray;
        break;
    }

    mView = mTexture.CreateView(&viewDesc);
    return mView != nullptr;
}

void Texture::Write(const void* data, u64 dataSize, u32 mipLevel, u32 arrayLayer) {
    auto dev = mDevice.lock();
    if (!dev || !data || dataSize == 0) return;

    u32 mipWidth = std::max(1u, mInfo.width >> mipLevel);
    u32 mipHeight = std::max(1u, mInfo.height >> mipLevel);
    u32 bytesPerPixel = TextureFormatBytesPerPixel(mInfo.format);
    u32 bytesPerRow = mipWidth * bytesPerPixel;

    u32 alignedBytesPerRow = (bytesPerRow + 255) & ~255u;

    wgpu::TexelCopyBufferLayout dataLayout{};
    dataLayout.offset = 0;
    dataLayout.bytesPerRow = alignedBytesPerRow;
    dataLayout.rowsPerImage = mipHeight;

    wgpu::TexelCopyTextureInfo dest{};
    dest.texture = mTexture;
    dest.mipLevel = mipLevel;
    dest.origin = {0, 0, arrayLayer};

    wgpu::Extent3D writeSize = {mipWidth, mipHeight, 1};

    if (alignedBytesPerRow == bytesPerRow) {
        dev->GetQueue().WriteTexture(
            &dest, data, static_cast<size_t>(dataSize), &dataLayout, &writeSize);
    } else {
        size_t packedSize = static_cast<size_t>(alignedBytesPerRow) * mipHeight;
        std::vector<u8> padded(packedSize, 0);
        auto* src = static_cast<const u8*>(data);
        for (u32 row = 0; row < mipHeight; ++row) {
            std::memcpy(
                padded.data() + row * alignedBytesPerRow, src + row * bytesPerRow, bytesPerRow);
        }
        dev->GetQueue().WriteTexture(&dest, padded.data(), packedSize, &dataLayout, &writeSize);
    }
}

u32 Texture::GetWidth() const noexcept { return mInfo.width; }
u32 Texture::GetHeight() const noexcept { return mInfo.height; }
u32 Texture::GetDepth() const noexcept { return mInfo.depth; }
u32 Texture::GetMipLevels() const noexcept { return mInfo.mipLevels; }
u32 Texture::GetArrayLayers() const noexcept { return mInfo.arrayLayers; }
TextureFormat Texture::GetFormat() const noexcept { return mInfo.format; }
TextureDimension Texture::GetDimension() const noexcept { return mInfo.dimension; }
TextureUsageFlags Texture::GetUsage() const noexcept { return mInfo.usage; }
const TextureInfo& Texture::GetInfo() const noexcept { return mInfo; }
wgpu::Texture Texture::GetHandle() const noexcept { return mTexture; }
wgpu::TextureView Texture::GetView() const noexcept { return mView; }

} // namespace ct
