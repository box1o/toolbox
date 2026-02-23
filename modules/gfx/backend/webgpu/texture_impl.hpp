#pragma once
#include <toolbox/gfx/api/texture_resource.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class TextureViewImpl final : public TextureView {
public:
    explicit TextureViewImpl(wgpu::TextureView v) : mView(v) {}
    ~TextureViewImpl() override = default;

    [[nodiscard]] void* GetNativeView() const noexcept override { return (void*)mView.Get(); }
    [[nodiscard]] void* GetNativeViewPtr() const noexcept override { return (void*)&mView; } // <—

    [[nodiscard]] const wgpu::TextureView& Handle() const noexcept { return mView; }

private:
    wgpu::TextureView mView;
};

class TextureImpl final : public Texture {
public:
    TextureImpl() = default;
    ~TextureImpl() override = default;

    result<void> Initialize(ref<Device> device, const TextureDesc& desc) noexcept;

    [[nodiscard]] u32 GetWidth() const noexcept override { return mWidth; }
    [[nodiscard]] u32 GetHeight() const noexcept override { return mHeight; }
    [[nodiscard]] TextureFormat GetFormat() const noexcept override { return mFormat; }

    [[nodiscard]] void* GetNativeTexture() const noexcept override { return (void*)mTexture.Get(); }

    [[nodiscard]] result<ref<TextureView>> CreateView(const TextureViewDesc& desc) noexcept override;
    static result<ref<Texture>> FromFile(ref<Device> device, const std::string& path, const TextureFromFileDesc& desc) noexcept;

private:
    static wgpu::TextureFormat ToWGPU(TextureFormat fmt) noexcept;
    static wgpu::TextureUsage ToWGPUUsage(TextureUsageFlags usage) noexcept;

private:
    wgpu::Device mDevice;
    wgpu::Queue  mQueue;
    wgpu::Texture mTexture;

    u32 mWidth{1};
    u32 mHeight{1};
    TextureFormat mFormat{TextureFormat::Undefined};
    TextureUsageFlags mUsage{TextureUsageFlags::None};
};

} // namespace ct::gfx::webgpu
