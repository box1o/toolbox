#pragma once
#include <toolbox/base/base.hpp>

#include "texture.hpp"

namespace ct::gfx {

// clang-format off
enum class TextureFilter : u8 {
    Nearest = 0,
    Linear  = 1
};
enum class TextureWrap : u8 {
    Repeat        = 0,
    MirrorRepeat  = 1,
    ClampToEdge   = 2,
};
// clang-format on

struct SamplerConfig {
    TextureFilter minFilter{TextureFilter::Linear};
    TextureFilter magFilter{TextureFilter::Linear};
    TextureWrap wrapU{TextureWrap::Repeat};
    TextureWrap wrapV{TextureWrap::Repeat};
    TextureWrap wrapW{TextureWrap::Repeat};
    f32 maxAnisotropy{1.0f};
    f32 borderColor[4]{0.0f, 0.0f, 0.0f, 1.0f};

    [[nodiscard]] static SamplerConfig LinearRepeat() {
        SamplerConfig cfg;
        cfg.minFilter = TextureFilter::Linear;
        cfg.magFilter = TextureFilter::Linear;
        cfg.wrapU = TextureWrap::Repeat;
        cfg.wrapV = TextureWrap::Repeat;
        cfg.wrapW = TextureWrap::Repeat;
        cfg.maxAnisotropy = 1.0f;
        return cfg;
    }

    [[nodiscard]] static SamplerConfig LinearClamp() {
        SamplerConfig cfg;
        cfg.minFilter = TextureFilter::Linear;
        cfg.magFilter = TextureFilter::Linear;
        cfg.wrapU = TextureWrap::ClampToEdge;
        cfg.wrapV = TextureWrap::ClampToEdge;
        cfg.wrapW = TextureWrap::ClampToEdge;
        cfg.maxAnisotropy = 1.0f;
        return cfg;
    }

    [[nodiscard]] static SamplerConfig PointRepeat() {
        SamplerConfig cfg;
        cfg.minFilter = TextureFilter::Nearest;
        cfg.magFilter = TextureFilter::Nearest;
        cfg.wrapU = TextureWrap::Repeat;
        cfg.wrapV = TextureWrap::Repeat;
        cfg.wrapW = TextureWrap::Repeat;
        cfg.maxAnisotropy = 1.0f;
        return cfg;
    }
};

struct SamplerDesc {
    SamplerConfig config{SamplerConfig::LinearRepeat()};
};

// fwd
class Device;
class Sampler {
public:
    virtual ~Sampler() = default;

    [[nodiscard]] virtual void* GetNativeSamplerHandle() noexcept = 0;

    [[nodiscard]] static result<ref<Sampler>> Create(
        ref<Device> device, const SamplerDesc& desc = {}) noexcept;

protected:
    virtual result<void> Initialize() noexcept = 0;
    Sampler() = default;
};

} // namespace ct::gfx
