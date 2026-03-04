#include "pipeline_impl.hpp"
#include "common.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/api/shader.hpp"

namespace ct::gfx::webgpu {

PipelineImpl::PipelineImpl(ref<Device> device, const PipelineDesc& desc)
    : mDesc(desc), mDevice(std::move(device)) {}

result<wgpu::PipelineLayout> PipelineImpl::CreatePipelineLayout(wgpu::Device& device) noexcept {
    std::vector<wgpu::BindGroupLayout> nativeLayouts;
    nativeLayouts.reserve(mBindGroupLayouts.size());

    for (const auto& layout : mBindGroupLayouts) {
        const auto& entries = layout.GetEntries();
        std::vector<wgpu::BindGroupLayoutEntry> bglEntries;
        bglEntries.reserve(entries.size());

        for (const auto& entry : entries) {
            wgpu::BindGroupLayoutEntry bgle{};
            bgle.binding = entry.binding;
            bgle.visibility = detail::ToWGPUVisibility(entry.visibility);

            switch (entry.type) {
            case BindingType::UniformBuffer:
            case BindingType::StorageBuffer:
            case BindingType::ReadOnlyStorageBuffer: {
                wgpu::BufferBindingLayout buf{};
                buf.type = detail::ToWGPUBufferBinding(entry.type);
                buf.minBindingSize = entry.bufferSize;
                buf.hasDynamicOffset = entry.bufferHasDynamicOffset;
                bgle.buffer = buf;
                break;
            }
            case BindingType::Sampler: {
                wgpu::SamplerBindingLayout sam{};
                sam.type = wgpu::SamplerBindingType::Filtering;
                bgle.sampler = sam;
                break;
            }
            case BindingType::Texture: {
                wgpu::TextureBindingLayout tex{};
                tex.sampleType = detail::ToWGPU(entry.textureSampleType);
                tex.viewDimension = detail::ToWGPU(entry.textureViewDimension);
                tex.multisampled = false;
                bgle.texture = tex;
                break;
            }
            case BindingType::StorageTexture: {
                wgpu::StorageTextureBindingLayout stex{};
                stex.access = wgpu::StorageTextureAccess::WriteOnly;
                stex.format = detail::ToWGPU(entry.storageTextureFormat);
                stex.viewDimension = detail::ToWGPU(entry.textureViewDimension);
                bgle.storageTexture = stex;
                break;
            }
            }

            bglEntries.push_back(bgle);
        }

        wgpu::BindGroupLayoutDescriptor bgld{};
        bgld.entryCount = bglEntries.size();
        bgld.entries = bglEntries.data();

        auto nativeLayout = device.CreateBindGroupLayout(&bgld);
        if (!nativeLayout) {
            return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
                "Pipeline: failed to create bind group layout");
        }
        nativeLayouts.push_back(nativeLayout);
    }

    wgpu::PipelineLayoutDescriptor pld{};
    pld.bindGroupLayoutCount = nativeLayouts.size();
    pld.bindGroupLayouts = nativeLayouts.data();

    auto pipelineLayout = device.CreatePipelineLayout(&pld);
    if (!pipelineLayout) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Pipeline: failed to create pipeline layout");
    }

    return ok(pipelineLayout);
}

result<void> PipelineImpl::Initialize() noexcept {
    if (!mDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Pipeline: device is null");
    }
    if (!mShader) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Pipeline: shader is null");
    }

    auto* nativeDevice = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!nativeDevice || !*nativeDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Pipeline: failed to acquire device handle");
    }

    auto* shaderModule = static_cast<wgpu::ShaderModule*>(mShader->GetNativeShaderHandle());
    if (!shaderModule || !*shaderModule) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Pipeline: failed to acquire shader module handle");
    }

    //NOTE: Build vertex buffer layouts for WebGPU
    const auto& vertexBuffers = mVertexLayout.GetBuffers();
    std::vector<std::vector<wgpu::VertexAttribute>> allAttribs(vertexBuffers.size());
    std::vector<wgpu::VertexBufferLayout> nativeVBLayouts(vertexBuffers.size());

    for (size_t i = 0; i < vertexBuffers.size(); ++i) {
        const auto& vb = vertexBuffers[i];
        auto& attribs = allAttribs[i];
        attribs.reserve(vb.attributes.size());

        for (const auto& attr : vb.attributes) {
            wgpu::VertexAttribute va{};
            va.shaderLocation = attr.shaderLocation;
            va.format = detail::ToWGPU(attr.format);
            va.offset = attr.offset;
            attribs.push_back(va);
        }

        nativeVBLayouts[i].arrayStride = vb.stride;
        nativeVBLayouts[i].stepMode = detail::ToWGPU(vb.stepMode);
        nativeVBLayouts[i].attributeCount = attribs.size();
        nativeVBLayouts[i].attributes = attribs.data();
    }

    //NOTE: Pipeline layout from bind group layouts
    wgpu::PipelineLayout pipelineLayout{nullptr};
    if (!mBindGroupLayouts.empty()) {
        auto layoutResult = CreatePipelineLayout(*nativeDevice);
        if (!layoutResult) return err(layoutResult.error());
        pipelineLayout = layoutResult.value();
    }

    //NOTE: Vertex state
    wgpu::VertexState vertexState{};
    vertexState.module = *shaderModule;
    vertexState.entryPoint = wgpu::StringView{"vs_main", 7};
    vertexState.bufferCount = nativeVBLayouts.size();
    vertexState.buffers = nativeVBLayouts.data();

    //NOTE: Primitive state
    wgpu::PrimitiveState primitiveState{};
    primitiveState.topology = detail::ToWGPU(mRasterizer.topology);
    primitiveState.frontFace = detail::ToWGPU(mRasterizer.frontFace);
    primitiveState.cullMode = detail::ToWGPU(mRasterizer.cullMode);

    //NOTE: Depth stencil state
    wgpu::DepthStencilState depthStencilState{};
    bool useDepth = mDepthStencil.depthTestEnable || mDepthStencil.depthWriteEnable;

    if (useDepth) {
        depthStencilState.format = detail::ToWGPU(mDepthStencil.depthFormat);
        depthStencilState.depthWriteEnabled =
            mDepthStencil.depthWriteEnable ? wgpu::OptionalBool::True : wgpu::OptionalBool::False;
        depthStencilState.depthCompare = detail::ToWGPU(mDepthStencil.depthCompareOp);
    }

    std::vector<ColorTargetState> colorTargets = mColorTargets;
    if (colorTargets.empty()) {
        colorTargets.push_back(ColorTargetState{});
    }

    std::vector<wgpu::BlendState> blendStates(colorTargets.size());
    std::vector<wgpu::ColorTargetState> nativeColorTargets(colorTargets.size());
    for (size_t i = 0; i < colorTargets.size(); ++i) {
        const auto& target = colorTargets[i];
        auto& nativeBlend = blendStates[i];
        auto& nativeTarget = nativeColorTargets[i];

        if (target.blend.enable) {
            nativeBlend.color.srcFactor = detail::ToWGPU(target.blend.srcColor);
            nativeBlend.color.dstFactor = detail::ToWGPU(target.blend.dstColor);
            nativeBlend.color.operation = detail::ToWGPU(target.blend.colorOp);
            nativeBlend.alpha.srcFactor = detail::ToWGPU(target.blend.srcAlpha);
            nativeBlend.alpha.dstFactor = detail::ToWGPU(target.blend.dstAlpha);
            nativeBlend.alpha.operation = detail::ToWGPU(target.blend.alphaOp);
        }

        nativeTarget.format = detail::ToWGPU(target.format);
        nativeTarget.blend = target.blend.enable ? &nativeBlend : nullptr;
        nativeTarget.writeMask = wgpu::ColorWriteMask::All;
    }

    //NOTE: Fragment state
    wgpu::FragmentState fragmentState{};
    fragmentState.module = *shaderModule;
    fragmentState.entryPoint = wgpu::StringView{"fs_main", 7};
    fragmentState.targetCount = nativeColorTargets.size();
    fragmentState.targets = nativeColorTargets.data();

    //NOTE: Multisample state
    wgpu::MultisampleState multisampleState{};
    multisampleState.count = 1;
    multisampleState.mask = ~0u;
    multisampleState.alphaToCoverageEnabled = false;

    //NOTE: Render pipeline descriptor
    wgpu::RenderPipelineDescriptor rpd{};
    rpd.label = wgpu::StringView{mDesc.label.c_str(), mDesc.label.size()};
    rpd.layout = pipelineLayout;
    rpd.vertex = vertexState;
    rpd.primitive = primitiveState;
    rpd.depthStencil = useDepth ? &depthStencilState : nullptr;
    rpd.fragment = &fragmentState;
    rpd.multisample = multisampleState;

    mPipeline = nativeDevice->CreateRenderPipeline(&rpd);
    if (!mPipeline) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Pipeline: CreateRenderPipeline failed");
    }

    return ok();
}

} // namespace ct::gfx::webgpu
