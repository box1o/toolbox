#pragma once
#include <toolbox/gfx/api/device.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class DeviceImpl final : public Device {
public:
    explicit DeviceImpl(const DeviceDesc& desc);
    ~DeviceImpl() override = default;

    result<void> Initialize() noexcept;

    [[nodiscard]] const wgpu::Instance& InstanceHandle() const noexcept { return mInstance; }
    [[nodiscard]] const wgpu::Device& DeviceHandle() const noexcept { return mDevice; }
    [[nodiscard]] const wgpu::Queue& QueueHandle() const noexcept { return mQueue; }
    [[nodiscard]] const AdapterInfo& GetAdapterInfo() const noexcept override { return mAdapterInfo; }

    [[nodiscard]] const Limits& GetLimits() const noexcept override { return mLimits; }

    [[nodiscard]] DeviceNativeHandles GetNative() const noexcept override;

    void Tick() const noexcept override;

private:
    result<void> CreateInstance() noexcept;
    result<void> RequestAdapter() noexcept;
    result<void> RequestDevice() noexcept;

    void QueryAdapterInfo() noexcept;
    void QueryLimits() noexcept;

    static wgpu::PowerPreference ToWGPU(PowerPreference p) noexcept;
    static std::string ToString(wgpu::StringView sv);

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
