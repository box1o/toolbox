#include "device.hpp"
#include "toolbox/gfx/common.hpp"

#include <atomic>
#include <memory>
#include <webgpu/webgpu_cpp.h>

#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

namespace ct::gfx {

namespace detail {

struct AdapterRequestState {
    std::atomic_bool done{false};
    wgpu::Adapter adapter = nullptr;
    std::string message{};
};

struct DeviceRequestState {
    std::atomic_bool done{false};
    wgpu::Device device = nullptr;
    std::string message{};
};

static constexpr wgpu::PowerPreference PowerPreferenceToWGPU(const PowerPreference p) noexcept {
    switch (p) {
        case PowerPreference::LowPower:        return wgpu::PowerPreference::LowPower;
        case PowerPreference::HighPerformance: return wgpu::PowerPreference::HighPerformance;
        default:                               return wgpu::PowerPreference::Undefined;
    }
}

static void OnDeviceLost(
    const wgpu::Device&,
    wgpu::DeviceLostReason,
    wgpu::StringView message,
    WGPUDeviceImpl::DebugUserData* ud)
{
    if (!ud) return;
    if (ud->cfg.deviceLostLogs) {
        log::Warn("Device lost: {}", dsv(message));
    }
}

static void OnUncapturedError(
    const wgpu::Device&,
    wgpu::ErrorType type,
    wgpu::StringView message,
    WGPUDeviceImpl::DebugUserData* ud)
{
    if (!ud) return;
    const std::string msg = dsv(message);
    if (ud->cfg.verboseErrors) {
        log::Error("[wgpu] Uncaptured error (type {}): {}", static_cast<int>(type), msg);
    } else {
        log::Error("[wgpu] Uncaptured error: {}", msg);
    }
}

static void ConfigureDeviceDescriptor(wgpu::DeviceDescriptor& desc, WGPUDeviceImpl::DebugUserData* ud) {
    desc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, OnDeviceLost, ud);
    desc.SetUncapturedErrorCallback(OnUncapturedError, ud);
}

static inline void PumpUntilDone(wgpu::Instance& instance, const std::atomic_bool& doneFlag) {
#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
    while (!doneFlag.load(std::memory_order_acquire)) emscripten_sleep(1);
#else
    while (!doneFlag.load(std::memory_order_acquire)) instance.ProcessEvents();
#endif
}

} // namespace detail

WGPUDeviceImpl::WGPUDeviceImpl(const DeviceDesc& desc) {
    mDebugUserData.cfg = desc.debug;
}

WGPUDeviceImpl::~WGPUDeviceImpl() {
};

bool WGPUDeviceImpl::CreateInstance() noexcept {
#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
    mInstance = wgpu::CreateInstance(nullptr);
    return mInstance != nullptr;
#else
    wgpu::InstanceDescriptor id{};
    static constexpr auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    id.requiredFeatureCount = 1;
    id.requiredFeatures = &kTimedWaitAny;

    mInstance = wgpu::CreateInstance(&id);
    return mInstance != nullptr;
#endif
}

bool WGPUDeviceImpl::RequestAdapter(const DeviceDesc& desc) noexcept {
    if (!mInstance) return false;

    wgpu::RequestAdapterOptions opts{};
    opts.powerPreference = detail::PowerPreferenceToWGPU(desc.powerPreference);

    auto state = std::make_shared<detail::AdapterRequestState>();

    mInstance.RequestAdapter(
        &opts,
        wgpu::CallbackMode::AllowSpontaneous,
        [state](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message) {
            state->message = dsv(message);
            if (status == wgpu::RequestAdapterStatus::Success) {
                state->adapter = adapter;
            } else {
                state->adapter = nullptr;
            }
            state->done.store(true, std::memory_order_release);
        });

    detail::PumpUntilDone(mInstance, state->done);

    if (!state->adapter) {
        if (!state->message.empty()) log::Error("[wgpu] Adapter request failed: {}", state->message);
        return false;
    }

    mAdapter = state->adapter;
    return true;
}

bool WGPUDeviceImpl::RequestDevice(const DeviceDesc& desc) noexcept {
    if (!mAdapter) return false;

    mDebugUserData.cfg = desc.debug;

    wgpu::DeviceDescriptor dd{};
    detail::ConfigureDeviceDescriptor(dd, &mDebugUserData);

    // IMPORTANT: this must be a real allocation, not a default-constructed null ref<>
    auto state = std::make_shared<detail::DeviceRequestState>();

    mAdapter.RequestDevice(
        &dd,
        wgpu::CallbackMode::AllowSpontaneous,
        [state](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message) {
            state->message = dsv(message);
            if (status == wgpu::RequestDeviceStatus::Success) {
                state->device = device;
            } else {
                state->device = nullptr;
            }
            state->done.store(true, std::memory_order_release);
        });

    detail::PumpUntilDone(mInstance, state->done);

    if (!state->device) {
        if (!state->message.empty()) log::Error("[wgpu] Device request failed: {}", state->message);
        return false;
    }

    mDevice = state->device;
    mQueue  = mDevice.GetQueue();
    return (mDevice != nullptr) && (mQueue != nullptr);
}

void WGPUDeviceImpl::Tick() const {
#if !defined(WEBGPU_BACKEND_EMSCRIPTEN)
    if (mInstance) mInstance.ProcessEvents();
#endif
}

void WGPUDeviceImpl::QueryAdapterInfo() noexcept {
    if (!mAdapter) return;

    wgpu::AdapterInfo ai{};
    mAdapter.GetInfo(&ai);

    mAdapterInfo = {
        .vendor       = dsv(ai.vendor),
        .architecture = dsv(ai.architecture),
        .device       = dsv(ai.device),
        .description  = dsv(ai.description),
        .backendType  = static_cast<u32>(ai.backendType),
        .adapterType  = static_cast<u32>(ai.adapterType),
    };
}

void WGPUDeviceImpl::QueryLimits() noexcept {
    if (!mDevice) return;

    wgpu::Limits lim{};
    mDevice.GetLimits(&lim);

    mLimits = {
        .maxVertexAttributes    = lim.maxVertexAttributes,
        .maxColorAttachments    = lim.maxColorAttachments,
        .maxTextureDimension2D  = lim.maxTextureDimension2D,
        .maxBufferSize          = lim.maxBufferSize,
        .maxBindGroups          = lim.maxBindGroups,
    };
}

} // namespace ct::gfx
