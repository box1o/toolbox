#include "../../include/toolbox/gfx/api/texture.hpp"
#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/texture_impl.hpp"
#endif

#include <stb_image.h>

namespace ct::gfx {

static inline u32 AlignTo(u32 v, u32 a) { return (v + (a - 1)) & ~(a - 1); }

result<ref<Texture>> Texture::FromFile(
    ref<Device> device, const Path& path, bool srgb, TextureUsageFlags usage) noexcept {

    u32 w{0};
    u32 h{0};
    u32 comp{0};

    auto pathStr = path.string();
    stbi_uc* pixels = stbi_load(pathStr.c_str(), (int*)&w, (int*)&h, (int*)&comp, 4);
    if (!pixels || w <= 0 || h <= 0) {
        if (pixels) stbi_image_free(pixels);
        return err(ErrorCode::FILE_NOT_FOUND, "Texture::FromFile: failed to load image");
    }

    const u32 width = (u32)w;
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
            std::memcpy(upload.data() + (size_t)alignedBpr * y, src + (size_t)unalignedBpr * y,
                unalignedBpr);
        }
    }

    stbi_image_free(pixels);

    TextureDesc td{};
    td.width = width;
    td.height = height;
    td.mipLevels = 1;
    td.format = srgb ? TextureFormat::RGBA8UnormSrgb : TextureFormat::RGBA8Unorm;
    td.usage = usage | TextureUsageFlags::CopyDst | TextureUsageFlags::Sampled;

    auto texture = createRef<webgpu::TextureImpl>(device, td);
    TRY_RETURN(texture->Initialize());
    TRY_RETURN(texture->Update(upload.data(), (u64)upload.size()));
    return ok(std::move(texture));
};

result<ref<Texture>> Texture::Create(ref<Device> device, const TextureDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto queue = createRef<webgpu::TextureImpl>(device, desc);
    TRY_RETURN(queue->Initialize());
    return ok(std::move(queue));
#else
    (void)desc;
    log::Critical("Texture creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
