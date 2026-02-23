#include "device_impl.hpp"
#include "toolbox/base/errors/result.hpp"
#include <toolbox/base/logger/logger.hpp>

#include <atomic>
#include <cstdlib>
#include <memory>

#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

namespace ct::gfx::webgpu {

namespace {
struct AdapterReqState {
    std::atomic_bool done{false};
    wgpu::Adapter adapter{nullptr};
    std::string message{};
};

struct DeviceReqState {
    std::atomic_bool done{false};
    wgpu::Device device{nullptr};
    std::string message{};
};

static inline void PumpUntilDone(wgpu::Instance& instance, const std::atomic_bool& doneFlag) {
#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
    while (!doneFlag.load(std::memory_order_acquire)) emscripten_sleep(1);
#else
    while (!doneFlag.load(std::memory_order_acquire)) instance.ProcessEvents();
#endif
}
} // namespace

DeviceImpl::DeviceImpl(const DeviceDesc& desc) : mDesc(desc) {}

result<void> DeviceImpl::Initialize() noexcept {
    if (auto res = CreateInstance(); !res) return res;
    if (auto res = RequestAdapter(); !res) return res;
    if (auto res = RequestDevice(); !res) return res;

    QueryAdapterInfo();
    QueryLimits();

    return ok();
}

wgpu::PowerPreference DeviceImpl::ToWGPU(PowerPreference p) noexcept {
    switch (p) {
    case PowerPreference::LowPower:
        return wgpu::PowerPreference::LowPower;
    case PowerPreference::HighPerformance:
        return wgpu::PowerPreference::HighPerformance;
    default:
        return wgpu::PowerPreference::Undefined;
    }
}

std::string DeviceImpl::ToString(wgpu::StringView sv) {
    if (!sv.data || sv.length == 0) return {};
    return std::string(sv.data, sv.length);
}

void DeviceImpl::OnDeviceLost(
    const wgpu::Device&, wgpu::DeviceLostReason, wgpu::StringView message, DeviceImpl* self) {
    if (!self) return;
    if (self->mDesc.debug.deviceLostLogs) {
        log::Warn("[wgpu] Device lost: {}", ToString(message));
    }
}

void DeviceImpl::OnUncapturedError(
    const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message, DeviceImpl* self) {
    if (!self) return;

    const auto text = ToString(message);
    if (self->mDesc.debug.verboseErrors) {
        log::Error("[wgpu] Uncaptured error (type {}): {}", static_cast<int>(type), text);
    } else {
        log::Error("[wgpu] Uncaptured error: {}", text);
    }

    // Fail fast on GPU validation/runtime errors to avoid submitting invalid work.
    std::abort();
}

result<void> DeviceImpl::CreateInstance() noexcept {
#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
    mInstance = wgpu::CreateInstance(nullptr);
    return mInstance != nullptr;
#else
    wgpu::InstanceDescriptor id{};
    static constexpr auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    id.requiredFeatureCount = 1;
    id.requiredFeatures = &kTimedWaitAny;

    mInstance = wgpu::CreateInstance(&id);
    if (!mInstance) {
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "Device: failed to create instance");
    }
#endif
    return ok();
}

result<void> DeviceImpl::RequestAdapter() noexcept {
    if (!mInstance) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: instance not initialized");
    }

    wgpu::RequestAdapterOptions opts{};
    opts.powerPreference = ToWGPU(mDesc.powerPreference);

    auto state = std::make_shared<AdapterReqState>();

    mInstance.RequestAdapter(&opts, wgpu::CallbackMode::AllowSpontaneous,
        [state](
            wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message) {
            state->message = DeviceImpl::ToString(message);
            state->adapter = (status == wgpu::RequestAdapterStatus::Success) ? adapter : nullptr;
            state->done.store(true, std::memory_order_release);
        });

    PumpUntilDone(mInstance, state->done);

    if (!state->adapter) {
        if (!state->message.empty())
            log::Error("[wgpu] Adapter request failed: {}", state->message);
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: failed to request adapter");
    }

    mAdapter = state->adapter;
    return ok();
}

result<void> DeviceImpl::RequestDevice() noexcept {
    if (!mAdapter)
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: adapter not initialized");

    wgpu::DeviceDescriptor dd{};

    dd.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, &DeviceImpl::OnDeviceLost, this);

    dd.SetUncapturedErrorCallback(&DeviceImpl::OnUncapturedError, this);

    auto state = std::make_shared<DeviceReqState>();

    mAdapter.RequestDevice(&dd, wgpu::CallbackMode::AllowSpontaneous,
        [state](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message) {
            state->message = DeviceImpl::ToString(message);
            state->device = (status == wgpu::RequestDeviceStatus::Success) ? device : nullptr;
            state->done.store(true, std::memory_order_release);
        });

    PumpUntilDone(mInstance, state->done);

    if (!state->device) {
        if (!state->message.empty()) log::Error("[wgpu] Device request failed: {}", state->message);
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: failed to request device");
    }

    mDevice = state->device;
    mQueue = mDevice.GetQueue();
    return ok();
}

void DeviceImpl::QueryAdapterInfo() noexcept {
    if (!mAdapter) return;

    wgpu::AdapterInfo ai{};
    mAdapter.GetInfo(&ai);

    mAdapterInfo.vendor = ToString(ai.vendor);
    mAdapterInfo.architecture = ToString(ai.architecture);
    mAdapterInfo.device = ToString(ai.device);
    mAdapterInfo.description = ToString(ai.description);
    mAdapterInfo.backendType = static_cast<u32>(ai.backendType);
    mAdapterInfo.adapterType = static_cast<u32>(ai.adapterType);
}

void DeviceImpl::QueryLimits() noexcept {
    if (!mDevice) return;

    wgpu::Limits lim{};
    mDevice.GetLimits(&lim);

    mLimits.maxVertexAttributes = lim.maxVertexAttributes;
    mLimits.maxColorAttachments = lim.maxColorAttachments;
    mLimits.maxTextureDimension2D = lim.maxTextureDimension2D;
    mLimits.maxBufferSize = lim.maxBufferSize;
    mLimits.maxBindGroups = lim.maxBindGroups;
}

DeviceNativeHandles DeviceImpl::GetNative() const noexcept {
    DeviceNativeHandles h{};
    h.instance = (void*)mInstance.Get();
    h.adapter = (void*)mAdapter.Get();
    h.device = (void*)mDevice.Get();
    h.queue = (void*)mQueue.Get();
    return h;
}

void DeviceImpl::Tick() const noexcept {
#if !defined(WEBGPU_BACKEND_EMSCRIPTEN)
    if (mInstance) mInstance.ProcessEvents();
#endif
}

} // namespace ct::gfx::webgpu
