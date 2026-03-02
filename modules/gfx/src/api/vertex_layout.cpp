#include "../../include/toolbox/gfx/api/vertex_layout.hpp"

namespace ct::gfx {

u32 VertexLayout::GetTotalAttributeCount() const noexcept {
    u32 count = 0;
    for (const auto& buf : mBuffers) count += static_cast<u32>(buf.attributes.size());
    return count;
}


VertexLayout::BufferBuilder VertexLayout::Builder::AddBuffer(VertexStepMode stepMode) {
    return BufferBuilder{*this, stepMode};
}

VertexLayout VertexLayout::Builder::Build() noexcept {
    VertexLayout layout;
    layout.mBuffers = std::move(mBuffers);
    return layout;
}


VertexLayout::BufferBuilder::BufferBuilder(Builder& parent, VertexStepMode stepMode)
    : mParent(parent) {
    mCurrent.stepMode = stepMode;
}

VertexLayout::BufferBuilder& VertexLayout::BufferBuilder::Attribute(
    u32 shaderLocation, VertexFormat format) {

    VertexAttribute attr{};
    attr.shaderLocation = shaderLocation;
    attr.format = format;
    attr.offset = mRunningOffset;

    mRunningOffset += VertexFormatSize(format);
    mCurrent.attributes.push_back(attr);
    return *this;
}

VertexLayout::BufferBuilder& VertexLayout::BufferBuilder::AttributeAt(
    u32 shaderLocation, VertexFormat format, u64 manualOffset) {

    VertexAttribute attr{};
    attr.shaderLocation = shaderLocation;
    attr.format = format;
    attr.offset = manualOffset;

    u64 end = manualOffset + VertexFormatSize(format);
    if (end > mRunningOffset) mRunningOffset = end;

    mCurrent.attributes.push_back(attr);
    return *this;
}

VertexLayout::BufferBuilder VertexLayout::BufferBuilder::AddBuffer(VertexStepMode stepMode) {
    Finalize();
    return BufferBuilder{mParent, stepMode};
}

VertexLayout VertexLayout::BufferBuilder::Build() noexcept {
    Finalize();
    return mParent.Build();
}

void VertexLayout::BufferBuilder::Finalize() noexcept {
    if (mFinalized) return;
    mFinalized = true;
    mCurrent.stride = mRunningOffset;
    mParent.mBuffers.push_back(std::move(mCurrent));
}

} // namespace ct::gfx
