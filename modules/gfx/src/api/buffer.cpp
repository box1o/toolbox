#include "toolbox/gfx/api/buffer.hpp"
#include "toolbox/gfx/api/device.hpp"

namespace ct {

namespace {

wgpu::BufferUsage ToWGPUUsage(BufferType type, bool mappable) {
    wgpu::BufferUsage usage = wgpu::BufferUsage::CopyDst;

    switch (type) {
    case BufferType::Vertex:
        usage |= wgpu::BufferUsage::Vertex;
        break;
    case BufferType::Index:
        usage |= wgpu::BufferUsage::Index;
        break;
    case BufferType::Uniform:
        usage |= wgpu::BufferUsage::Uniform;
        break;
    case BufferType::Storage:
        usage |= wgpu::BufferUsage::Storage;
        break;
    }

    if (mappable) {
        usage |= wgpu::BufferUsage::MapWrite;
        usage |= wgpu::BufferUsage::CopySrc;
    }

    return usage;
}

// NOTE: WebGPU requires buffer sizes to be aligned to 4 bytes
constexpr u64 AlignBufferSize(u64 size) noexcept { return (size + 3) & ~static_cast<u64>(3); }

} // namespace

result<ref<Buffer>> Buffer::Create(ref<Device> device, const BufferInfo& info) noexcept {
    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");
    if (info.size == 0)
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Buffer size cannot be zero");

    ref<Buffer> buffer(new Buffer());
    buffer->mDevice = device.get();
    buffer->mType = info.type;
    buffer->mSize = info.size;
    buffer->mMappable = info.mappable;

    if (!buffer->Init(*device, info)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create buffer");
    }

    if (info.initialData) {
        buffer->Write(info.initialData, info.size);
    }

    return buffer;
}

Buffer::~Buffer() {
    if (mBuffer) {
        mBuffer.Destroy();
        mBuffer = nullptr;
    }
}

bool Buffer::Init(const Device& device, const BufferInfo& info) {
    wgpu::BufferDescriptor desc{};
    desc.size = AlignBufferSize(info.size);
    desc.usage = ToWGPUUsage(info.type, info.mappable);
    desc.mappedAtCreation = false;

    mBuffer = device.GetDevice().CreateBuffer(&desc);
    return mBuffer != nullptr;
}

void Buffer::Write(const void* data, u64 size, u64 offset) {
    if (!mDevice || !mBuffer || !data || size == 0) return;
    mDevice->GetQueue().WriteBuffer(mBuffer, offset, data, static_cast<size_t>(size));
}

BufferType Buffer::GetType() const noexcept { return mType; }
u64 Buffer::GetSize() const noexcept { return mSize; }
bool Buffer::IsMappable() const noexcept { return mMappable; }
wgpu::Buffer Buffer::GetHandle() const noexcept { return mBuffer; }

} // namespace ct
