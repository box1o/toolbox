#pragma once
#include <toolbox/base/base.hpp>
#include <vector>

namespace ct::gfx {

enum class VertexInputRate : u8 { PerVertex, PerInstance };

enum class VertexFormat : u8 {
    Float2,
    Float3,
    Float4,
    UByte4Norm,
};

struct VertexAttributeDesc {
    u32 location{0};
    u32 binding{0};
    VertexFormat format{VertexFormat::Float3};
    u32 offset{0};
    std::string name{};
};

struct VertexBindingDesc {
    u32 binding{0};
    u32 stride{0};
    VertexInputRate rate{VertexInputRate::PerVertex};
};

struct VertexLayoutDesc {
    std::vector<VertexBindingDesc> bindings{};
    std::vector<VertexAttributeDesc> attributes{};

    VertexLayoutDesc& Binding(u32 binding, u32 stride, VertexInputRate rate) {
        bindings.push_back({binding, stride, rate});
        return *this;
    }

    VertexLayoutDesc& Attribute(
        u32 location, u32 binding, VertexFormat format, u32 offset, std::string name = {}) {
        attributes.push_back({location, binding, format, offset, std::move(name)});
        return *this;
    }
};

class VertexLayout {
public:
    explicit VertexLayout(VertexLayoutDesc desc) : mDesc(std::move(desc)) {}
    [[nodiscard]] const VertexLayoutDesc& GetDesc() const noexcept { return mDesc; }

private:
    VertexLayoutDesc mDesc;
};


} // namespace ct::gfx
