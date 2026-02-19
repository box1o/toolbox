#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/common.hpp"
#include <functional>

namespace ct::gfx {

struct AdapterInfo {
    std::string vendor{};
    std::string architecture{};
    std::string device{};
    std::string description{};
    u32 backendType{};
    u32 adapterType{};
};

struct Limits {
    u32 maxVertexAttributes{0};
    u32 maxColorAttachments{0};
    u32 maxTextureDimension2D{0};
    u64 maxBufferSize{0};
    u32 maxBindGroups{0};
};

enum class PowerPreference : u8 {
    LowPower,
    HighPerformance,
};

struct DeviceDesc {
    PowerPreference powerPreference{PowerPreference::HighPerformance};
    DebugConfig debug{};
};

enum class Status : u8 {
    Success = 0,
    Error = 1,
};

class Device {
public:
    using DeviceCreatedCallback = std::function<void(Status status, void* userData)>;
    using AdapterCreatedCallback = std::function<void(Status status, void* userData)>;

public:
    virtual ~Device() = default;

    [[nodiscard]] virtual void* GetInstance() const noexcept = 0;
    [[nodiscard]] virtual void* GetAdapter() const noexcept = 0;
    [[nodiscard]] virtual void* GetDevice() const noexcept = 0;
    [[nodiscard]] virtual void* GetQueue() const noexcept = 0;

    [[nodiscard]] virtual const AdapterInfo& GetAdapterInfo() const noexcept = 0;
    [[nodiscard]] virtual const Limits& GetLimits() const noexcept = 0;

    virtual void Tick() const = 0;

    // NOTE: async creation is not implemented in this backend yet; the parameter is kept for API compatibility.
    [[nodiscard]] static result<ref<Device>> Create(const DeviceDesc& desc, bool async = false) noexcept;

protected:
    Device() = default;
};

} // namespace ct::gfx
