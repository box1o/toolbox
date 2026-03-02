#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/bind_group_layout.hpp>
#include <vector>

namespace ct::gfx {

class Device;
class Buffer;
class Texture;
class TextureView;
class Sampler;

struct BufferBinding {
    u32 binding{0};
    ref<Buffer> buffer{nullptr};
    u64 offset{0};
    u64 size{0}; //NOTE: 0 = whole buffer
};

struct TextureBinding {
    u32 binding{0};
    ref<TextureView> view{nullptr};
};

struct SamplerBinding {
    u32 binding{0};
    ref<Sampler> sampler{nullptr};
};

struct BindGroupDesc {
    BindGroupLayout layout{};
    std::vector<BufferBinding> buffers{};
    std::vector<TextureBinding> textures{};
    std::vector<SamplerBinding> samplers{};
    std::string debugName{"BindGroup"};
};

class BindGroup {
public:
    virtual ~BindGroup() = default;

    [[nodiscard]] virtual void* GetNativeHandle() noexcept = 0;

    class Builder {
    public:
        Builder(ref<Device> dev, const BindGroupLayout& layout)
            : mDevice(std::move(dev)), mLayout(layout) {}

        // clang-format off
        Builder& AddBuffer(u32 binding, ref<Buffer> buffer, u64 offset = 0, u64 size = 0);
        Builder& AddTexture(u32 binding, ref<TextureView> view);
        Builder& AddSampler(u32 binding, ref<Sampler> sampler);
        Builder& SetDebugName(const std::string& name);
        // clang-format on

        result<ref<BindGroup>> Build() noexcept;

    private:
        ref<Device> mDevice{};
        BindGroupLayout mLayout{};
        std::vector<BufferBinding> mBuffers{};
        std::vector<TextureBinding> mTextures{};
        std::vector<SamplerBinding> mSamplers{};
        std::string mDebugName{"BindGroup"};
    };

    [[nodiscard]] static Builder Create(ref<Device> device, const BindGroupLayout& layout) noexcept {
        return Builder{std::move(device), layout};
    }

protected:
    virtual result<void> Initialize() noexcept = 0;
    BindGroup() = default;
};

} // namespace ct::gfx
