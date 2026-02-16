#pragma once
#include "toolbox/base/base.hpp"
#include <webgpu/webgpu_cpp.h>

namespace ct {

struct DeviceInfo {
    bool validate{true};
    bool verbose{false};
};

struct AdapterCapabilities {
    std::string vendor{};
    std::string architecture{};
    std::string device{};
    std::string description{};
    u32 backendType{};
    u32 adapterType{};
};

struct DeviceCapabilities {
    u32 maxVertexAttributes{0};
    u32 maxColorAttachments{0};
    u32 maxTextureDimension2D{0};
    u32 maxBufferSize{0};
    u32 maxBindGroups{0};
};

class Device {
public:
    ~Device();

    [[nodiscard]] const AdapterCapabilities& GetAdapterCapabilities() const noexcept;
    [[nodiscard]] const DeviceCapabilities& GetCapabilities() const noexcept;

    [[nodiscard]] wgpu::Instance GetInstance() const noexcept;
    [[nodiscard]] wgpu::Adapter GetAdapter() const noexcept;
    [[nodiscard]] wgpu::Device GetDevice() const noexcept;
    [[nodiscard]] wgpu::Queue GetQueue() const noexcept;

    void Tick() const;

    [[nodiscard]] static result<ref<Device>> Create(const DeviceInfo& info = {}) noexcept;

private:
    Device() = default;
    bool CreateInstance(const DeviceInfo& info);
    bool CreateAdapter();
    bool CreateDevice(const DeviceInfo& info);

    AdapterCapabilities mAdapterCapabilities{};
    DeviceCapabilities mDeviceCapabilities{};

    wgpu::Instance mInstance{nullptr};
    wgpu::Adapter mAdapter{nullptr};
    wgpu::Device mDevice{nullptr};
    wgpu::Queue mQueue{nullptr};
};

} // namespace ct
