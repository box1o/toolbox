#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/types.hpp"

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

namespace ct {

namespace detail {

struct AdapterRequestState {
    bool done = false;
    wgpu::Adapter adapter = nullptr;
};

struct DeviceRequestState {
    bool done = false;
    wgpu::Device device = nullptr;
};

} // namespace detail

result<ref<Device>> Device::Create(const DeviceInfo& info) noexcept {
    ref<Device> dev(new Device());

    if (!dev->CreateInstance(info)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create wgpu instance");
    }
    log::Info("[wgpu] Instance created");

    if (!dev->CreateAdapter()) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create adapter");
    }
    log::Info("[wgpu] Adapter: {}", dev->GetAdapterCapabilities().device);

    if (!dev->CreateDevice(info)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create device");
    }
    log::Info("[wgpu] Device created");

    return dev;
}

Device::~Device() {
    mQueue = nullptr;
    mDevice = nullptr;
    mAdapter = nullptr;
    mInstance = nullptr;
}

bool Device::CreateInstance(const DeviceInfo& info) {
    (void)info;
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    mInstance = wgpu::CreateInstance(nullptr);
#else
    wgpu::InstanceDescriptor desc{};

    static constexpr auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    desc.requiredFeatureCount = 1;
    desc.requiredFeatures = &kTimedWaitAny;

    mInstance = wgpu::CreateInstance(&desc);
#endif
    return mInstance != nullptr;
}

bool Device::CreateAdapter() {
    wgpu::RequestAdapterOptions opts{};
    opts.powerPreference = wgpu::PowerPreference::HighPerformance;

    detail::AdapterRequestState state;
    mInstance.RequestAdapter(&opts, wgpu::CallbackMode::AllowSpontaneous,
        [&](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message) {
            state.done = true;
            if (status != wgpu::RequestAdapterStatus::Success) {
                log::Error("Adapter request failed: {}", dsv(message));
                return;
            }
            state.adapter = adapter;
        });

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    while (!state.done) {
        emscripten_sleep(10);
    }
#else
    while (!state.done) {
        mInstance.ProcessEvents();
    }
#endif

    if (!state.adapter) return false;
    mAdapter = state.adapter;

    wgpu::AdapterInfo adapterInfo;
    mAdapter.GetInfo(&adapterInfo);

    mAdapterCapabilities = {
        .vendor = dsv(adapterInfo.vendor),
        .architecture = dsv(adapterInfo.architecture),
        .device = dsv(adapterInfo.device),
        .description = dsv(adapterInfo.description),
        .backendType = static_cast<u32>(adapterInfo.backendType),
        .adapterType = static_cast<u32>(adapterInfo.adapterType),
    };

    return true;
}

bool Device::CreateDevice(const DeviceInfo& info) {
    (void)info;
    wgpu::DeviceDescriptor deviceDesc{};

    deviceDesc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous,
        [](const wgpu::Device&, wgpu::DeviceLostReason reason, wgpu::StringView message) {
            log::Error("[wgpu] Device lost: {}", dsv(message));
        });

    deviceDesc.SetUncapturedErrorCallback(
        [](const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message) {
            log::Error("[wgpu] Error (type {}): {}", static_cast<int>(type), dsv(message));
        });

    detail::DeviceRequestState state;
    mAdapter.RequestDevice(&deviceDesc, wgpu::CallbackMode::AllowSpontaneous,
        [&](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message) {
            state.done = true;
            if (status != wgpu::RequestDeviceStatus::Success) {
                log::Error("Device request failed: {}", dsv(message));
                return;
            }
            state.device = device;
        });

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    while (!state.done) {
        emscripten_sleep(10);
    }
#else
    while (!state.done) {
        mInstance.ProcessEvents();
    }
#endif

    if (!state.device) return false;

    mDevice = state.device;
    mQueue = mDevice.GetQueue();

    if (!mDevice || !mQueue) return false;

    wgpu::Limits limits;
    mDevice.GetLimits(&limits);

    mDeviceCapabilities = {
        .maxVertexAttributes = limits.maxVertexAttributes,
        .maxColorAttachments = limits.maxColorAttachments,
        .maxTextureDimension2D = limits.maxTextureDimension2D,
        .maxBufferSize = limits.maxBufferSize,
        .maxBindGroups = limits.maxBindGroups,
    };

    return true;
}

const AdapterCapabilities& Device::GetAdapterCapabilities() const noexcept {
    return mAdapterCapabilities;
}
const DeviceCapabilities& Device::GetCapabilities() const noexcept { return mDeviceCapabilities; }

wgpu::Instance Device::GetInstance() const noexcept { return mInstance; }
wgpu::Adapter Device::GetAdapter() const noexcept { return mAdapter; }
wgpu::Device Device::GetDevice() const noexcept { return mDevice; }
wgpu::Queue Device::GetQueue() const noexcept { return mQueue; }

void Device::Tick() const {
#ifndef WEBGPU_BACKEND_EMSCRIPTEN
    if (mInstance) mInstance.ProcessEvents();
#endif
}

} // namespace ct
