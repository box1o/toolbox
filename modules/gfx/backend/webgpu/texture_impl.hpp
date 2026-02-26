#pragma once
#include <toolbox/gfx/api/texture.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class TextureViewImpl final : public TextureView {
public:
    explicit TextureViewImpl(wgpu::TextureView v) : mView(v) {}
    ~TextureViewImpl() override = default;

    [[nodiscard]] void* GetNativeTextureViewHandle() noexcept override { return (void*)&mView; }
    [[nodiscard]] const wgpu::TextureView& Handle() const noexcept { return mView; }

private:
    wgpu::TextureView mView;
};

class TextureImpl final : public Texture {
public:
    explicit TextureImpl(ref<Device> device, const TextureDesc& desc);
    ~TextureImpl() override = default;

    // basic
    [[nodiscard]] u32 GetWidth() const noexcept override { return mWidth; }
    [[nodiscard]] u32 GetHeight() const noexcept override { return mHeight; }

    [[nodiscard]] u32 GetMipLevels() const noexcept override { return mDesc.mipLevels; }
    [[nodiscard]] TextureFormat GetFormat() const noexcept override { return mDesc.format; }
    [[nodiscard]] TextureUsageFlags GetUsage() const noexcept override { return mDesc.usage; }

    [[nodiscard]] void* GetNativeTextureHandle() noexcept override {
        return static_cast<void*>(&mTexture);
    }

    // views
    // clang-format off
    [[nodiscard]]  result<ref<TextureView>> GetDefaultView() noexcept override;
    [[nodiscard]]  result<ref<TextureView>> CreateView( const TextureViewDesc& desc = {}) noexcept override;
    //clang-format on

    // upload
    result<void> Update(const void* data, u64 numBytes) noexcept override;


    // NOTE: Initialize
    result<void> Initialize() noexcept override;

private:
    TextureDesc mDesc{};
    u32 mWidth{0};
    u32 mHeight{0};

    ref<Device> mDevice{nullptr};
    wgpu::Texture mTexture;
};

} // namespace ct::gfx::webgpu
