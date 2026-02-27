#pragma once
#include <filesystem>
#include <string>
#include <toolbox/base/base.hpp>
#include <toolbox/base/errors/result.hpp>
#include <vector>

namespace ct::gfx {

using Path = std::filesystem::path;

enum class ShaderStage : u8 {
    None = 0,
    Vertex = 1 << 0,
    Fragment = 1 << 1,
    Compute = 1 << 2,
};

// clang-format off
[[nodiscard]] constexpr ShaderStage operator|(ShaderStage a, ShaderStage b) noexcept { return static_cast<ShaderStage>(static_cast<u8>(a) | static_cast<u8>(b)); }
[[nodiscard]] constexpr ShaderStage operator&(ShaderStage a, ShaderStage b) noexcept { return static_cast<ShaderStage>(static_cast<u8>(a) & static_cast<u8>(b)); }
[[nodiscard]] constexpr ShaderStage operator~(ShaderStage a) noexcept { return static_cast<ShaderStage>(~static_cast<u8>(a)); }
constexpr ShaderStage& operator|=(ShaderStage& a, ShaderStage b) noexcept { a = a | b; return a; }
constexpr ShaderStage& operator&=(ShaderStage& a, ShaderStage b) noexcept { a = a & b; return a; }
[[nodiscard]] constexpr bool HasStage(ShaderStage flags, ShaderStage stage) noexcept { return (flags & stage) != ShaderStage::None; }
// clang-format on

struct ShaderStageInfo {
    ShaderStage stage{};
    Path path{};
    std::string source{};
    std::string name{};
    bool isFile{true};
};

struct ShaderDesc {
    std::string entryPoint{"main"};
};

class Device;

class Shader {
public:
    virtual ~Shader() = default;

    class Builder {
    public:
        Builder(ref<Device> dev, const ShaderDesc& d)
            : mDevice(std::move(dev)), mDesc(d) {}

        Builder& AddShaderStage(ShaderStage stage, const Path& path);
        Builder& AddStageSource(ShaderStage stage, std::string source, std::string name = "sh_stage");
        Builder& AddShaderFile(ShaderStage stages, const Path& path);
        Builder& AddShaderSource(ShaderStage stages, std::string source, std::string name = "sh_combined");
        Builder& AddIncludePath(const Path& path);
        Builder& EnableReflection(bool enable = true);
        Builder& EnableCache(bool enable = true);

        result<ref<Shader>> Build() noexcept;

    private:
        ref<Device> mDevice{};
        ShaderDesc mDesc{};
        std::vector<ShaderStageInfo> mStages{};
        std::vector<Path> mIncludePaths{};
        bool mEnableReflection{false};
        bool mEnableCache{false};
    };

    [[nodiscard]] virtual const std::string& GetEntryPoint() const noexcept = 0;
    [[nodiscard]] virtual void* GetNativeShaderHandle() const noexcept = 0;

    [[nodiscard]] static Builder Create(ref<Device> device, const ShaderDesc& desc = {}) noexcept {
        return Builder{std::move(device), desc};
    }

protected:
    virtual result<void> Initialize() noexcept = 0;
    Shader() = default;
};

} // namespace ct::gfx
