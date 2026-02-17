#include "toolbox/gfx/api/pipeline.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/api/shader.hpp"
#include "toolbox/gfx/api/bind_group.hpp"

namespace ct {

namespace {

wgpu::PrimitiveTopology ToWGPU(PrimitiveTopology t) {
    switch (t) {
    case PrimitiveTopology::PointList:     return wgpu::PrimitiveTopology::PointList;
    case PrimitiveTopology::LineList:      return wgpu::PrimitiveTopology::LineList;
    case PrimitiveTopology::LineStrip:     return wgpu::PrimitiveTopology::LineStrip;
    case PrimitiveTopology::TriangleList:  return wgpu::PrimitiveTopology::TriangleList;
    case PrimitiveTopology::TriangleStrip: return wgpu::PrimitiveTopology::TriangleStrip;
    }
    return wgpu::PrimitiveTopology::TriangleList;
}

wgpu::CullMode ToWGPU(CullMode m) {
    switch (m) {
    case CullMode::None:  return wgpu::CullMode::None;
    case CullMode::Front: return wgpu::CullMode::Front;
    case CullMode::Back:  return wgpu::CullMode::Back;
    }
    return wgpu::CullMode::Back;
}

wgpu::FrontFace ToWGPU(FrontFace f) {
    switch (f) {
    case FrontFace::CCW: return wgpu::FrontFace::CCW;
    case FrontFace::CW:  return wgpu::FrontFace::CW;
    }
    return wgpu::FrontFace::CCW;
}

wgpu::BlendFactor ToWGPU(BlendFactor f) {
    switch (f) {
    case BlendFactor::Zero:              return wgpu::BlendFactor::Zero;
    case BlendFactor::One:               return wgpu::BlendFactor::One;
    case BlendFactor::SrcAlpha:          return wgpu::BlendFactor::SrcAlpha;
    case BlendFactor::OneMinusSrcAlpha:  return wgpu::BlendFactor::OneMinusSrcAlpha;
    case BlendFactor::DstAlpha:          return wgpu::BlendFactor::DstAlpha;
    case BlendFactor::OneMinusDstAlpha:  return wgpu::BlendFactor::OneMinusDstAlpha;
    case BlendFactor::SrcColor:          return wgpu::BlendFactor::Src;
    case BlendFactor::OneMinusSrcColor:  return wgpu::BlendFactor::OneMinusSrc;
    case BlendFactor::DstColor:          return wgpu::BlendFactor::Dst;
    case BlendFactor::OneMinusDstColor:  return wgpu::BlendFactor::OneMinusDst;
    }
    return wgpu::BlendFactor::One;
}

wgpu::BlendOperation ToWGPU(BlendOp op) {
    switch (op) {
    case BlendOp::Add:             return wgpu::BlendOperation::Add;
    case BlendOp::Subtract:        return wgpu::BlendOperation::Subtract;
    case BlendOp::ReverseSubtract: return wgpu::BlendOperation::ReverseSubtract;
    case BlendOp::Min:             return wgpu::BlendOperation::Min;
    case BlendOp::Max:             return wgpu::BlendOperation::Max;
    }
    return wgpu::BlendOperation::Add;
}

} // namespace

result<ref<RenderPipeline>> RenderPipeline::Create(
    ref<Device> device, const RenderPipelineInfo& info) noexcept {

    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");
    if (!info.shader) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Shader is null");

    auto wgpuDevice = device->GetDevice();

    //NOTE: build vertex buffer layouts from VertexLayout data
    std::vector<std::vector<wgpu::VertexAttribute>> attrStorage;
    std::vector<wgpu::VertexBufferLayout> vertexBuffers;

    attrStorage.resize(info.vertexLayout.buffers.size());
    vertexBuffers.reserve(info.vertexLayout.buffers.size());

    for (size_t i = 0; i < info.vertexLayout.buffers.size(); ++i) {
        const auto& buf = info.vertexLayout.buffers[i];
        auto& attrs = attrStorage[i];
        attrs.reserve(buf.attributes.size());

        for (const auto& attr : buf.attributes) {
            wgpu::VertexAttribute wa{};
            wa.format = ToWGPU(attr.format);
            wa.offset = attr.offset;
            wa.shaderLocation = attr.location;
            attrs.push_back(wa);
        }

        wgpu::VertexBufferLayout vbl{};
        vbl.arrayStride = buf.stride;
        vbl.stepMode = ToWGPU(buf.stepMode);
        vbl.attributeCount = static_cast<u32>(attrs.size());
        vbl.attributes = attrs.data();
        vertexBuffers.push_back(vbl);
    }

    //NOTE: build pipeline layout from bind group layouts
    std::vector<wgpu::BindGroupLayout> bgLayouts;
    bgLayouts.reserve(info.bindGroupLayouts.size());
    for (const auto& bgl : info.bindGroupLayouts) {
        bgLayouts.push_back(bgl->GetHandle());
    }

    wgpu::PipelineLayoutDescriptor plDesc{};
    plDesc.bindGroupLayoutCount = static_cast<u32>(bgLayouts.size());
    plDesc.bindGroupLayouts = bgLayouts.data();
    wgpu::PipelineLayout pipelineLayout = wgpuDevice.CreatePipelineLayout(&plDesc);

    if (!pipelineLayout) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create pipeline layout");
    }

    //NOTE: build color targets
    std::vector<wgpu::ColorTargetState> colorTargets;
    std::vector<wgpu::BlendState> blendStates;

    colorTargets.reserve(info.colorFormats.size());
    blendStates.reserve(info.colorFormats.size());

    for (const auto& fmt : info.colorFormats) {
        wgpu::ColorTargetState ct{};
        ct.format = ToWGPU(fmt);
        ct.writeMask = wgpu::ColorWriteMask::All;

        if (info.blend.enabled) {
            wgpu::BlendState bs{};
            bs.color.srcFactor = ToWGPU(info.blend.srcColor);
            bs.color.dstFactor = ToWGPU(info.blend.dstColor);
            bs.color.operation = ToWGPU(info.blend.colorOp);
            bs.alpha.srcFactor = ToWGPU(info.blend.srcAlpha);
            bs.alpha.dstFactor = ToWGPU(info.blend.dstAlpha);
            bs.alpha.operation = ToWGPU(info.blend.alphaOp);
            blendStates.push_back(bs);
            ct.blend = &blendStates.back();
        }

        colorTargets.push_back(ct);
    }

    //NOTE: fragment state
    wgpu::FragmentState fragment{};
    fragment.module = info.shader->GetHandle();
    fragment.entryPoint = info.shader->GetFragmentEntry().c_str();
    fragment.targetCount = static_cast<u32>(colorTargets.size());
    fragment.targets = colorTargets.data();

    //NOTE: depth/stencil state
    wgpu::DepthStencilState depthStencil{};
    if (info.hasDepth) {
        depthStencil.format = ToWGPU(info.depthStencil.format);
        depthStencil.depthWriteEnabled = info.depthStencil.depthWriteEnabled;
        depthStencil.depthCompare = info.depthStencil.depthTestEnabled
            ? ToWGPU(info.depthStencil.depthCompare)
            : wgpu::CompareFunction::Always;
    }

    //NOTE: assemble the pipeline descriptor
    wgpu::RenderPipelineDescriptor desc{};
    desc.layout = pipelineLayout;

    desc.vertex.module = info.shader->GetHandle();
    desc.vertex.entryPoint = info.shader->GetVertexEntry().c_str();
    desc.vertex.bufferCount = static_cast<u32>(vertexBuffers.size());
    desc.vertex.buffers = vertexBuffers.data();

    desc.primitive.topology = ToWGPU(info.topology);
    desc.primitive.frontFace = ToWGPU(info.frontFace);
    desc.primitive.cullMode = ToWGPU(info.cullMode);

    desc.multisample.count = info.sampleCount;

    desc.fragment = &fragment;

    if (info.hasDepth) {
        desc.depthStencil = &depthStencil;
    }

    ref<RenderPipeline> pipeline(new RenderPipeline());
    pipeline->mPipeline = wgpuDevice.CreateRenderPipeline(&desc);

    if (!pipeline->mPipeline) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create render pipeline");
    }

    log::Info("[wgpu] RenderPipeline created");
    return pipeline;
}

RenderPipeline::~RenderPipeline() {
    mPipeline = nullptr;
}

wgpu::RenderPipeline RenderPipeline::GetHandle() const noexcept { return mPipeline; }

} // namespace ct
