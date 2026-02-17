#pragma once
#include "toolbox/base/base.hpp"
#include <webgpu/webgpu_cpp.h>
#include <string>
#include <string_view>
#include <filesystem>

namespace ct {

class Device;

struct ShaderInfo {
    std::string source{};
    std::string label{"shader"};
    std::string vertexEntry{"vs_main"};
    std::string fragmentEntry{"fs_main"};

    [[nodiscard]] static ShaderInfo FromSource(
        std::string_view wgslSource,
        std::string_view label = "shader") noexcept {
        ShaderInfo info{};
        info.source = std::string(wgslSource);
        info.label = std::string(label);
        return info;
    }
};

struct ShaderFileInfo {
    std::filesystem::path path{};
    std::string label{};
    std::string vertexEntry{"vs_main"};
    std::string fragmentEntry{"fs_main"};
};

class Shader {
public:
    ~Shader();

    [[nodiscard]] wgpu::ShaderModule GetHandle() const noexcept;
    [[nodiscard]] const std::string& GetVertexEntry() const noexcept;
    [[nodiscard]] const std::string& GetFragmentEntry() const noexcept;
    [[nodiscard]] const std::string& GetLabel() const noexcept;
    [[nodiscard]] const std::string& GetSource() const noexcept;

    //NOTE: recompiles from current source — useful for hot-reload
    [[nodiscard]] bool Reload(Device& device);

    [[nodiscard]] static result<ref<Shader>> Create(
        ref<Device> device, const ShaderInfo& info) noexcept;

    [[nodiscard]] static result<ref<Shader>> FromFile(
        ref<Device> device, const ShaderFileInfo& info) noexcept;

private:
    Shader() = default;
    bool Compile(const Device& device);

    wgpu::ShaderModule mModule{nullptr};
    Device* mDevice{nullptr};
    std::string mSource{};
    std::string mLabel{};
    std::string mVertexEntry{"vs_main"};
    std::string mFragmentEntry{"fs_main"};
};

} // namespace ct
