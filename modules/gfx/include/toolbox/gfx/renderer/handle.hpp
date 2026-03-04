#pragma once

#include <toolbox/base/base.hpp>

namespace ct::gfx {

template <typename T> struct Handle {
    u32 id = ~0u;

    [[nodiscard]] bool IsValid() const { return id != ~0u; }
    bool operator==(const Handle& other) const { return id == other.id; }
    bool operator!=(const Handle& other) const { return id != other.id; }
    explicit operator bool() const { return IsValid(); }
};

struct MeshHandle : Handle<MeshHandle> {};
struct MaterialHandle : Handle<MaterialHandle> {};
struct TextureHandle : Handle<TextureHandle> {};
struct ShaderHandle : Handle<ShaderHandle> {};

} // namespace ct::gfx
