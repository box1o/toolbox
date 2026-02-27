#include "../../include/toolbox/gfx/api/bind_group_layout.hpp"

namespace ct::gfx {

BindGroupLayout::Builder& BindGroupLayout::Builder::AddUniform(
    u32 binding, u8 visibility, u64 bufferSize, bool dynamicOffset) {
    BindGroupEntry e{};
    e.binding = binding;
    e.type = BindingType::UniformBuffer;
    e.visibility = visibility;
    e.bufferSize = bufferSize;
    e.bufferHasDynamicOffset = dynamicOffset;
    mEntries.push_back(e);
    return *this;
}

BindGroupLayout::Builder& BindGroupLayout::Builder::AddStorage(
    u32 binding, u8 visibility, u64 bufferSize, bool readOnly) {
    BindGroupEntry e{};
    e.binding = binding;
    e.type = readOnly ? BindingType::ReadOnlyStorageBuffer : BindingType::StorageBuffer;
    e.visibility = visibility;
    e.bufferSize = bufferSize;
    mEntries.push_back(e);
    return *this;
}

BindGroupLayout::Builder& BindGroupLayout::Builder::AddSampler(u32 binding, u8 visibility) {
    BindGroupEntry e{};
    e.binding = binding;
    e.type = BindingType::Sampler;
    e.visibility = visibility;
    mEntries.push_back(e);
    return *this;
}

BindGroupLayout::Builder& BindGroupLayout::Builder::AddTexture(
    u32 binding, u8 visibility, TextureSampleType sampleType, TextureViewDimension dim) {
    BindGroupEntry e{};
    e.binding = binding;
    e.type = BindingType::Texture;
    e.visibility = visibility;
    e.textureSampleType = sampleType;
    e.textureViewDimension = dim;
    mEntries.push_back(e);
    return *this;
}

BindGroupLayout::Builder& BindGroupLayout::Builder::AddStorageTexture(
    u32 binding, u8 visibility, TextureFormat format, TextureViewDimension dim) {
    BindGroupEntry e{};
    e.binding = binding;
    e.type = BindingType::StorageTexture;
    e.visibility = visibility;
    e.storageTextureFormat = format;
    e.textureViewDimension = dim;
    mEntries.push_back(e);
    return *this;
}

BindGroupLayout BindGroupLayout::Builder::Build() noexcept {
    BindGroupLayout layout;
    layout.mEntries = std::move(mEntries);
    return layout;
}

} // namespace ct::gfx
