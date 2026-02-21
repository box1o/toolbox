#include "texture_impl.hpp"
#include "device_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

#include <cstring>
#include <stb_image.h>

namespace ct::gfx::webgpu {

static inline u32 AlignTo(u32 v, u32 a) { return (v + (a - 1)) & ~(a - 1); }

wgpu::TextureFormat TextureImpl::ToWGPU(TextureFormat fmt) noexcept {
    switch (fmt) {
        case TextureFormat::RGBA8Unorm:      return wgpu::TextureFormat::RGBA8Unorm;
        case TextureFormat::RGBA8UnormSrgb:  return wgpu::TextureFormat::RGBA8UnormSrgb;
        case TextureFormat::BGRA8Unorm:      return wgpu::TextureFormat::BGRA8Unorm;
        case TextureFormat::BGRA8UnormSrgb:  return wgpu::TextureFormat::BGRA8UnormSrgb;
        case TextureFormat::Depth16Unorm:    return wgpu::TextureFormat::Depth16Unorm;
        case TextureFormat::Depth24Plus:     return wgpu::TextureFormat::Depth24Plus;
        case TextureFormat::Depth24PlusStencil8: return wgpu::TextureFormat::Depth24PlusStencil8;
        case TextureFormat::Depth32Float:    return wgpu::TextureFormat::Depth32Float;
        case TextureFormat::Depth32FloatStencil8: return wgpu::TextureFormat::Depth32FloatStencil8;
        default: return wgpu::TextureFormat::Undefined;
    }
}

wgpu::TextureUsage TextureImpl::ToWGPUUsage(TextureUsageFlags usage) noexcept {
    wgpu::TextureUsage out = wgpu::TextureUsage::None;
    if (HasFlag(usage, TextureUsageFlags::Sampled))      out |= wgpu::TextureUsage::TextureBinding;
    if (HasFlag(usage, TextureUsageFlags::Storage))      out |= wgpu::TextureUsage::StorageBinding;
    if (HasFlag(usage, TextureUsageFlags::RenderTarget)) out |= wgpu::TextureUsage::RenderAttachment;
    if (HasFlag(usage, TextureUsageFlags::CopySrc))      out |= wgpu::TextureUsage::CopySrc;
    if (HasFlag(usage, TextureUsageFlags::CopyDst))      out |= wgpu::TextureUsage::CopyDst;
    return out;
}

bool TextureImpl::Init(ref<Device> device, const TextureDesc& desc) noexcept {
    if (!device) return false;
    if (desc.width == 0 || desc.height == 0) return false;

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("TextureImpl: device is not WebGPU device");
        return false;
    }

    mDevice = dev->DeviceHandle();
    mQueue  = dev->QueueHandle();
    if (!mDevice || !mQueue) return false;

    mWidth  = desc.width;
    mHeight = desc.height;
    mFormat = desc.format;
    mUsage  = desc.usage;

    wgpu::TextureDescriptor td{};
    td.dimension = wgpu::TextureDimension::e2D;
    td.size = { mWidth, mHeight, 1 };
    td.format = ToWGPU(mFormat);
    td.mipLevelCount = desc.mipLevels;
    td.sampleCount = 1;
    td.usage = ToWGPUUsage(mUsage);
    if (!desc.debugName.empty()) td.label = desc.debugName.c_str();

    mTexture = mDevice.CreateTexture(&td);
    return mTexture != nullptr;
}

result<ref<TextureView>> TextureImpl::CreateView(const TextureViewDesc& desc) noexcept {
    if (!mTexture) return err(ErrorCode::INVALID_STATE, "TextureView: texture not initialized");

    wgpu::TextureViewDescriptor vd{};
    if (!desc.debugName.empty()) vd.label = desc.debugName.c_str();

    auto v = mTexture.CreateView(&vd);
    if (!v) return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "TextureView: CreateView failed");

    return ok(createRef<TextureViewImpl>(v));
}

result<ref<Texture>> TextureImpl::FromFile(ref<Device> device, const std::string& path, const TextureFromFileDesc& desc) noexcept {
    int w = 0, h = 0, comp = 0;
    stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &comp, 4);
    if (!pixels || w <= 0 || h <= 0) {
        if (pixels) stbi_image_free(pixels);
        return err(ErrorCode::FILE_NOT_FOUND, "Texture::FromFile: failed to load image");
    }

    const u32 width  = (u32)w;
    const u32 height = (u32)h;
    const u32 bpp = 4;
    const u32 unalignedBpr = width * bpp;
    const u32 alignedBpr = AlignTo(unalignedBpr, 256);

    std::vector<u8> upload;
    const u8* src = (const u8*)pixels;

    if (alignedBpr == unalignedBpr) {
        upload.assign(src, src + (size_t)unalignedBpr * height);
    } else {
        upload.resize((size_t)alignedBpr * height);
        for (u32 y = 0; y < height; ++y) {
            std::memcpy(upload.data() + (size_t)alignedBpr * y,
                        src + (size_t)unalignedBpr * y,
                        unalignedBpr);
        }
    }

    stbi_image_free(pixels);

    TextureDesc td{};
    td.width = width;
    td.height = height;
    td.mipLevels = 1;
    td.format = desc.srgb ? TextureFormat::RGBA8UnormSrgb : TextureFormat::RGBA8Unorm;
    td.usage = desc.usage | TextureUsageFlags::CopyDst | TextureUsageFlags::Sampled;
    td.debugName = desc.debugName;

    auto tex = TRY(Texture::Create(device, td));
    auto* impl = dynamic_cast<TextureImpl*>(tex.get());
    if (!impl) return err(ErrorCode::INVALID_STATE, "Texture::FromFile: backend mismatch");

    wgpu::TexelCopyTextureInfo dst{};
    dst.texture = impl->mTexture;
    dst.mipLevel = 0;
    dst.origin = {0,0,0};
    dst.aspect = wgpu::TextureAspect::All;

    wgpu::TexelCopyBufferLayout layout{};
    layout.offset = 0;
    layout.bytesPerRow = alignedBpr;
    layout.rowsPerImage = height;

    wgpu::Extent3D extent{ width, height, 1 };
    impl->mQueue.WriteTexture(&dst, upload.data(), upload.size(), &layout, &extent);

    return tex;
}

} // namespace ct::gfx::webgpu
