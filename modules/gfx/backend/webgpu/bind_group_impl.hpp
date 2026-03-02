#pragma once
#include <toolbox/gfx/api/bind_group.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class BindGroupImpl final : public BindGroup {
public:
    explicit BindGroupImpl(ref<Device> device, BindGroupDesc desc);
    ~BindGroupImpl() override = default;

    [[nodiscard]] void* GetNativeHandle() noexcept override {
        return static_cast<void*>(&mBindGroup);
    }

    result<void> Initialize() noexcept override;

private:
    result<wgpu::BindGroupLayout> CreateNativeLayout(wgpu::Device& device) noexcept;

    BindGroupDesc mDesc{};
    ref<Device> mDevice{nullptr};
    wgpu::BindGroup mBindGroup{nullptr};
    wgpu::BindGroupLayout mNativeLayout{nullptr};
};

} // namespace ct::gfx::webgpu
