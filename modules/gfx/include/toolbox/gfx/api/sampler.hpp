#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

class Device;

enum class FilterMode : u8 { Nearest, Linear };
enum class AddressMode : u8 { Repeat, MirrorRepeat, ClampToEdge };

struct SamplerDesc {
    FilterMode minFilter{FilterMode::Linear};
    FilterMode magFilter{FilterMode::Linear};

    AddressMode addressU{AddressMode::Repeat};
    AddressMode addressV{AddressMode::Repeat};
    AddressMode addressW{AddressMode::Repeat};

    std::string debugName{"Sampler"};
};

class Sampler {
public:
    virtual ~Sampler() = default;
    [[nodiscard]] virtual void* GetNativeSampler() const noexcept = 0;

    [[nodiscard]] static result<ref<Sampler>> Create(ref<Device> device, const SamplerDesc& desc = {}) noexcept;

protected:
    Sampler() = default;
};

} // namespace ct::gfx
