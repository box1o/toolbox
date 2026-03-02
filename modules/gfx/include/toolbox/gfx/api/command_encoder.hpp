#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/render_pass.hpp>
#include <toolbox/gfx/api/command_buffer.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <string>

namespace ct::gfx {

class Device;
class Buffer;
class Texture;

struct CommandEncoderDesc {
    std::string debugName{"CommandEncoder"};
};

struct BufferCopyView {
    ref<Buffer> buffer{nullptr};
    u64 offset{0};
    u32 bytesPerRow{0};
    u32 rowsPerImage{0};
};

struct TextureCopyView {
    ref<Texture> texture{nullptr};
    u32 mipLevel{0};
    u32 originX{0};
    u32 originY{0};
    u32 originZ{0};
};

struct Extent3D {
    u32 width{1};
    u32 height{1};
    u32 depthOrArrayLayers{1};
};

class CommandEncoder {
public:
    virtual ~CommandEncoder() = default;

    //NOTE: Render pass
    [[nodiscard]] virtual result<ref<RenderPassEncoder>> BeginRenderPass(
        const RenderPassDesc& desc) noexcept = 0;

    //NOTE: Buffer operations
    virtual result<void> CopyBufferToBuffer(
        ref<Buffer> src, u64 srcOffset,
        ref<Buffer> dst, u64 dstOffset,
        u64 size) noexcept = 0;

    virtual result<void> ClearBuffer(
        ref<Buffer> buffer,
        u64 offset = 0,
        u64 size = 0) noexcept = 0;

    //NOTE: Buffer <-> Texture copies
    virtual result<void> CopyBufferToTexture(
        const BufferCopyView& src,
        const TextureCopyView& dst,
        const Extent3D& copySize) noexcept = 0;

    virtual result<void> CopyTextureToBuffer(
        const TextureCopyView& src,
        const BufferCopyView& dst,
        const Extent3D& copySize) noexcept = 0;

    virtual result<void> CopyTextureToTexture(
        const TextureCopyView& src,
        const TextureCopyView& dst,
        const Extent3D& copySize) noexcept = 0;

    //NOTE: Finish encoding
    [[nodiscard]] virtual result<ref<CommandBuffer>> Finish(
        const CommandBufferDesc& desc = {}) noexcept = 0;

    [[nodiscard]] static result<ref<CommandEncoder>> Create(
        ref<Device> device, const CommandEncoderDesc& desc = {}) noexcept;

protected:
    CommandEncoder() = default;
};

} // namespace ct::gfx
