#pragma once
#include <toolbox/gfx/api/device.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class DeviceImpl final : public Device {
public:
    explicit DeviceImpl(const DeviceDesc& desc);
    ~DeviceImpl() override = default;

    [[nodiscard]] Limits& GetLimits() noexcept override { return mLimits; }
    [[nodiscard]] AdapterInfo& GetAdapterInfo() noexcept override { return mAdapterInfo; }
    // clang-format off
    [[nodiscard]] void* GetNativeInstanceHandle() noexcept override { return static_cast<void*>(&mInstance); }
    [[nodiscard]] void* GetNativeDeviceHandle() noexcept override { return static_cast<void*>(&mDevice); }
    [[nodiscard]] void* GetNativeAdapterHandle() noexcept override { return static_cast<void*>(&mAdapter); }
    [[nodiscard]] void* GetNativeQueueHandle() noexcept override { return static_cast<void*>(&mQueue); }
    // clang-format on

    void Tick() const noexcept override;

    // NOTE: Initialize
    result<void> Initialize() noexcept override;

private:
    result<void> CreateInstance() noexcept;
    result<void> RequestAdapter() noexcept;
    result<void> RequestDevice() noexcept;

    void QueryAdapterInfo() noexcept;
    void QueryLimits() noexcept;

    // Callbacks
    static void OnDeviceLost(const wgpu::Device& device, wgpu::DeviceLostReason reason,
        wgpu::StringView message, DeviceImpl* self);

    static void OnUncapturedError(const wgpu::Device& device, wgpu::ErrorType type,
        wgpu::StringView message, DeviceImpl* self);

private:
    DeviceDesc mDesc{};

    wgpu::Instance mInstance;
    wgpu::Adapter mAdapter;
    wgpu::Device mDevice;
    wgpu::Queue mQueue;

    AdapterInfo mAdapterInfo{};
    Limits mLimits{};
};

} // namespace ct::gfx::webgpu
