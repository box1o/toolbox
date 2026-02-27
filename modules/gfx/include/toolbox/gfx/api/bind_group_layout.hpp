#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <vector>

namespace ct::gfx {

enum class BindingType : u8 {
    UniformBuffer,
    StorageBuffer,
    ReadOnlyStorageBuffer,
    Sampler,
    Texture,
    StorageTexture,
};

enum class TextureSampleType : u8 {
    Float,
    UnfilterableFloat,
    Depth,
    Sint,
    Uint,
};

enum class TextureViewDimension : u8 {
    e1D,
    e2D,
    e2DArray,
    e3D,
    Cube,
    CubeArray,
};

//NOTE: visibility is raw u8 bitmask matching ShaderStage values
// Vertex=1, Fragment=2, Compute=4 — caller casts via static_cast<u8>(ShaderStage::X)
struct BindGroupEntry {
    u32 binding{0};
    BindingType type{BindingType::UniformBuffer};
    u8 visibility{0};
    u64 bufferSize{0};
    bool bufferHasDynamicOffset{false};
    TextureSampleType textureSampleType{TextureSampleType::Float};
    TextureViewDimension textureViewDimension{TextureViewDimension::e2D};
    TextureFormat storageTextureFormat{TextureFormat::Undefined};
};

struct BindGroupLayoutDesc {
    std::vector<BindGroupEntry> entries{};
};

class BindGroupLayout {
public:
    [[nodiscard]] const std::vector<BindGroupEntry>& GetEntries() const noexcept { return mEntries; }
    [[nodiscard]] u32 GetEntryCount() const noexcept { return static_cast<u32>(mEntries.size()); }

    class Builder {
    public:
        Builder() = default;

        // clang-format off
        Builder& AddUniform(u32 binding, u8 visibility, u64 bufferSize, bool dynamicOffset = false);
        Builder& AddStorage(u32 binding, u8 visibility, u64 bufferSize, bool readOnly = false);
        Builder& AddSampler(u32 binding, u8 visibility);
        Builder& AddTexture(u32 binding, u8 visibility,
                            TextureSampleType sampleType = TextureSampleType::Float,
                            TextureViewDimension dim = TextureViewDimension::e2D);
        Builder& AddStorageTexture(u32 binding, u8 visibility,
                                   TextureFormat format,
                                   TextureViewDimension dim = TextureViewDimension::e2D);
        // clang-format on

        [[nodiscard]] BindGroupLayout Build() noexcept;

    private:
        std::vector<BindGroupEntry> mEntries{};
    };

    [[nodiscard]] static Builder Create() noexcept { return Builder{}; }

private:
    std::vector<BindGroupEntry> mEntries{};
};

} // namespace ct::gfx
