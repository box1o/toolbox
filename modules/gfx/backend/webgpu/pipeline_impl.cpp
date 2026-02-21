#include "pipeline_impl.hpp"
#include "device_impl.hpp"
#include "shader_impl.hpp"
#include "bindings_impl.hpp"

#include <toolbox/base/logger/logger.hpp>
#include <cstddef>

namespace ct::gfx::webgpu {

wgpu::VertexFormat RenderPipelineImpl::ToWGPU(VertexFormat f) noexcept {
    switch (f) {
        case VertexFormat::Float2:     return wgpu::VertexFormat::Float32x2;
        case VertexFormat::Float3:     return wgpu::VertexFormat::Float32x3;
        case VertexFormat::Float4:     return wgpu::VertexFormat::Float32x4;
        case VertexFormat::UByte4Norm: return wgpu::VertexFormat::Unorm8x4;
        default:                       return wgpu::VertexFormat::Float32x3;
    }
}

wgpu::VertexStepMode RenderPipelineImpl::ToWGPU(VertexInputRate r) noexcept {
    return (r == VertexInputRate::PerInstance) ? wgpu::VertexStepMode::Instance : wgpu::VertexStepMode::Vertex;
}

wgpu::TextureFormat RenderPipelineImpl::ToWGPU(TextureFormat f) noexcept {
    switch (f) {
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

wgpu::PrimitiveTopology RenderPipelineImpl::ToWGPU(PrimitiveTopology t) noexcept {
    switch (t) {
        case PrimitiveTopology::TriangleList:  return wgpu::PrimitiveTopology::TriangleList;
        case PrimitiveTopology::TriangleStrip: return wgpu::PrimitiveTopology::TriangleStrip;
        case PrimitiveTopology::LineList:      return wgpu::PrimitiveTopology::LineList;
        case PrimitiveTopology::LineStrip:     return wgpu::PrimitiveTopology::LineStrip;
        case PrimitiveTopology::PointList:     return wgpu::PrimitiveTopology::PointList;
        default:                               return wgpu::PrimitiveTopology::TriangleList;
    }
}

wgpu::CullMode RenderPipelineImpl::ToWGPU(CullMode c) noexcept {
    switch (c) {
        case CullMode::None:  return wgpu::CullMode::None;
        case CullMode::Front: return wgpu::CullMode::Front;
        case CullMode::Back:  return wgpu::CullMode::Back;
        default:              return wgpu::CullMode::Back;
    }
}

wgpu::FrontFace RenderPipelineImpl::ToWGPU(FrontFace f) noexcept {
    return (f == FrontFace::CW) ? wgpu::FrontFace::CW : wgpu::FrontFace::CCW;
}

bool RenderPipelineImpl::Init(ref<Device> device, const RenderPipelineDesc& desc) noexcept {
    if (!device || !desc.vertexShader || !desc.fragmentShader) {
        log::Error("RenderPipelineImpl: missing device/shaders");
        return false;
    }

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("RenderPipelineImpl: device is not WebGPU");
        return false;
    }

    auto* vs = dynamic_cast<ShaderModuleImpl*>(desc.vertexShader.get());
    auto* fs = dynamic_cast<ShaderModuleImpl*>(desc.fragmentShader.get());
    if (!vs || !fs) {
        log::Error("RenderPipelineImpl: shader backend mismatch");
        return false;
    }

    mDevice = dev->DeviceHandle();
    if (!mDevice) return false;

    const auto& vdesc = desc.vertexLayout.GetDesc();

    for (const auto& b : vdesc.bindings) {
        if (b.stride == 0) {
            log::Error("RenderPipelineImpl: binding {} stride=0", b.binding);
            return false;
        }
    }

    std::vector<std::vector<wgpu::VertexAttribute>> attrsPerBinding;
    attrsPerBinding.resize(vdesc.bindings.size());

    for (const auto& a : vdesc.attributes) {
        size_t bi = (size_t)-1;
        for (size_t i = 0; i < vdesc.bindings.size(); ++i) {
            if (vdesc.bindings[i].binding == a.binding) { bi = i; break; }
        }
        if (bi == (size_t)-1) {
            log::Error("RenderPipelineImpl: attribute refers to missing binding {}", a.binding);
            return false;
        }

        wgpu::VertexAttribute va{};
        va.shaderLocation = a.location;
        va.format = ToWGPU(a.format);
        va.offset = a.offset;
        attrsPerBinding[bi].push_back(va);
    }

    std::vector<wgpu::VertexBufferLayout> vbLayouts;
    vbLayouts.resize(vdesc.bindings.size());

    for (size_t i = 0; i < vdesc.bindings.size(); ++i) {
        const auto& b = vdesc.bindings[i];
        auto& layout = vbLayouts[i];
        layout.arrayStride = b.stride;
        layout.stepMode = ToWGPU(b.rate);
        layout.attributeCount = (uint32_t)attrsPerBinding[i].size();
        layout.attributes = attrsPerBinding[i].data();
    }

    // ----- Pipeline layout -----
    wgpu::PipelineLayout pipelineLayout{nullptr};

    if (desc.layout) {
        auto* pl = dynamic_cast<PipelineLayoutImpl*>(desc.layout.get());
        if (!pl) {
            log::Error("RenderPipelineImpl: PipelineLayout backend mismatch");
            return false;
        }
        pipelineLayout = pl->Handle();
    } else {
        wgpu::PipelineLayoutDescriptor pld{};
        pld.bindGroupLayoutCount = 0;
        pld.bindGroupLayouts = nullptr;
        pipelineLayout = mDevice.CreatePipelineLayout(&pld);
        if (!pipelineLayout) {
            log::Error("RenderPipelineImpl: CreatePipelineLayout failed");
            return false;
        }
    }

    wgpu::ColorTargetState colorTarget{};
    colorTarget.format = ToWGPU(desc.colorFormat);
    colorTarget.writeMask = wgpu::ColorWriteMask::All;

    wgpu::FragmentState frag{};
    frag.module = fs->Handle();
    frag.entryPoint = fs->GetEntryPoint().c_str();
    frag.targetCount = 1;
    frag.targets = &colorTarget;

    wgpu::VertexState vert{};
    vert.module = vs->Handle();
    vert.entryPoint = vs->GetEntryPoint().c_str();
    vert.bufferCount = (uint32_t)vbLayouts.size();
    vert.buffers = vbLayouts.data();

    wgpu::PrimitiveState prim{};
    prim.topology = ToWGPU(desc.raster.topology);
    prim.cullMode = ToWGPU(desc.raster.cull);
    prim.frontFace = ToWGPU(desc.raster.frontFace);

    wgpu::DepthStencilState depth{};
    wgpu::DepthStencilState* depthPtr = nullptr;
    if (desc.enableDepth) {
        depth.format = ToWGPU(desc.depthFormat);
        depth.depthWriteEnabled = true;
        depth.depthCompare = wgpu::CompareFunction::Less;
        depthPtr = &depth;
    }

    wgpu::RenderPipelineDescriptor rpd{};
    if (!desc.debugName.empty()) rpd.label = desc.debugName.c_str();
    rpd.layout = pipelineLayout;
    rpd.vertex = vert;
    rpd.primitive = prim;
    rpd.fragment = &frag;
    rpd.depthStencil = depthPtr;
    rpd.multisample.count = 1;

    mPipeline = mDevice.CreateRenderPipeline(&rpd);
    if (!mPipeline) {
        log::Error("RenderPipelineImpl: CreateRenderPipeline failed");
        return false;
    }

    return true;
}

} // namespace ct::gfx::webgpu
