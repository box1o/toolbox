#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/types.hpp"

namespace ct {

struct SamplerInfo {
    FilterMode minFilter{FilterMode::Linear};
    FilterMode magFilter{FilterMode::Linear};
    FilterMode mipmapFilter{FilterMode::Linear};
    AddressMode addressU{AddressMode::Repeat};
    AddressMode addressV{AddressMode::Repeat};
    AddressMode addressW{AddressMode::Repeat};
    f32 maxAnisotropy{1.0f};
    CompareFunction compare{CompareFunction::Undefined};

    [[nodiscard]] static constexpr SamplerInfo LinearRepeat() noexcept { return {}; }

    [[nodiscard]] static constexpr SamplerInfo LinearClamp() noexcept {
        SamplerInfo info{};
        info.addressU = AddressMode::ClampToEdge;
        info.addressV = AddressMode::ClampToEdge;
        info.addressW = AddressMode::ClampToEdge;
        return info;
    }

    [[nodiscard]] static constexpr SamplerInfo NearestRepeat() noexcept {
        SamplerInfo info{};
        info.minFilter = FilterMode::Nearest;
        info.magFilter = FilterMode::Nearest;
        info.mipmapFilter = FilterMode::Nearest;
        return info;
    }

    [[nodiscard]] static constexpr SamplerInfo NearestClamp() noexcept {
        SamplerInfo info{};
        info.minFilter = FilterMode::Nearest;
        info.magFilter = FilterMode::Nearest;
        info.mipmapFilter = FilterMode::Nearest;
        info.addressU = AddressMode::ClampToEdge;
        info.addressV = AddressMode::ClampToEdge;
        info.addressW = AddressMode::ClampToEdge;
        return info;
    }

    [[nodiscard]] static constexpr SamplerInfo DepthCompare() noexcept {
        SamplerInfo info{};
        info.minFilter = FilterMode::Linear;
        info.magFilter = FilterMode::Linear;
        info.mipmapFilter = FilterMode::Nearest;
        info.addressU = AddressMode::ClampToEdge;
        info.addressV = AddressMode::ClampToEdge;
        info.addressW = AddressMode::ClampToEdge;
        info.compare = CompareFunction::Less;
        return info;
    }
};

class Device;

class Sampler {
public:
    ~Sampler();

    [[nodiscard]] const SamplerInfo& GetInfo() const noexcept;
    [[nodiscard]] wgpu::Sampler GetHandle() const noexcept;

    [[nodiscard]] static result<ref<Sampler>> Create(
        ref<Device> device, const SamplerInfo& info = {}) noexcept;

private:
    Sampler() = default;
    bool Init(const Device& device, const SamplerInfo& info);

    SamplerInfo mInfo{};
    wgpu::Sampler mSampler{nullptr};
};

} // namespace ct
