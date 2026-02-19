#pragma once
#include <toolbox/gfx/api/device.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {

class WGPUDeviceImpl final : public Device {
public:
    explicit WGPUDeviceImpl(const DeviceDesc& desc);
    ~WGPUDeviceImpl() override;

    [[nodiscard]] void* GetInstance() const noexcept override { return (void*)&mInstance; }
    [[nodiscard]] void* GetAdapter() const noexcept override { return (void*)&mAdapter; }
    [[nodiscard]] void* GetDevice() const noexcept override { return (void*)&mDevice; }
    [[nodiscard]] void* GetQueue() const noexcept override { return (void*)&mQueue; }


    [[nodiscard]] const AdapterInfo& GetAdapterInfo() const noexcept override {
        return mAdapterInfo;
    }
    [[nodiscard]] const Limits& GetLimits() const noexcept override { return mLimits; }

    void Tick() const override;

public:
    bool CreateInstance() noexcept;
    bool RequestAdapter(const DeviceDesc& desc) noexcept;
    bool RequestDevice(const DeviceDesc& desc) noexcept;

    void QueryAdapterInfo() noexcept;
    void QueryLimits() noexcept;

public:
    struct DebugUserData {
        DebugConfig cfg{};
    };

private:
    wgpu::Instance mInstance;
    wgpu::Adapter mAdapter;
    wgpu::Device mDevice;
    wgpu::Queue mQueue;

    AdapterInfo mAdapterInfo{};
    Limits mLimits{};

    DebugUserData mDebugUserData{};
};

} // namespace ct::gfx
