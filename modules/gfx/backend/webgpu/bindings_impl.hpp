#pragma once
#include <toolbox/gfx/api/bindings.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class BindGroupLayoutImpl final : public BindGroupLayout {
public:
    bool Init(ref<Device> device, const BindGroupLayoutDesc& desc) noexcept;

    [[nodiscard]] void* GetNativeLayout() const noexcept override { return (void*)mLayout.Get(); }
    [[nodiscard]] const wgpu::BindGroupLayout& Handle() const noexcept { return mLayout; }

private:
    wgpu::Device mDevice;
    wgpu::BindGroupLayout mLayout;
};

class BindGroupImpl final : public BindGroup {
public:
    bool Init(ref<Device> device, const BindGroupDesc& desc) noexcept;

    [[nodiscard]] void* GetNativeGroup() const noexcept override { return (void*)mGroup.Get(); }
    [[nodiscard]] const wgpu::BindGroup& Handle() const noexcept { return mGroup; }

private:
    wgpu::Device mDevice;
    wgpu::BindGroup mGroup;
};

class PipelineLayoutImpl final : public PipelineLayout {
public:
    bool Init(ref<Device> device, const PipelineLayoutDesc& desc) noexcept;

    [[nodiscard]] void* GetNativeLayout() const noexcept override { return (void*)mLayout.Get(); }
    [[nodiscard]] const wgpu::PipelineLayout& Handle() const noexcept { return mLayout; }

private:
    wgpu::Device mDevice;
    wgpu::PipelineLayout mLayout;
};

} // namespace ct::gfx::webgpu
