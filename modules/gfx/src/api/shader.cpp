#include "toolbox/gfx/api/shader.hpp"
#include "toolbox/gfx/api/device.hpp"
#include <fstream>
#include <sstream>

namespace ct {

result<ref<Shader>> Shader::Create(ref<Device> device, const ShaderInfo& info) noexcept {
    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");
    if (info.source.empty())
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Shader source is empty");

    ref<Shader> shader(new Shader());
    shader->mDevice = device.get();
    shader->mSource = info.source;
    shader->mLabel = info.label;
    shader->mVertexEntry = info.vertexEntry;
    shader->mFragmentEntry = info.fragmentEntry;

    if (!shader->Compile(*device)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to compile shader");
    }

    log::Info("[wgpu] Shader created: {}", info.label);
    return shader;
}

result<ref<Shader>> Shader::FromFile(ref<Device> device, const ShaderFileInfo& info) noexcept {
    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");

    std::ifstream file(info.path);
    if (!file.is_open()) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE,
            "Failed to open shader file: " + info.path.string());
    }

    std::ostringstream ss;
    ss << file.rdbuf();

    ShaderInfo shaderInfo{};
    shaderInfo.source = ss.str();
    shaderInfo.label = info.label.empty() ? info.path.filename().string() : info.label;
    shaderInfo.vertexEntry = info.vertexEntry;
    shaderInfo.fragmentEntry = info.fragmentEntry;

    return Create(device, shaderInfo);
}

Shader::~Shader() { mModule = nullptr; }

bool Shader::Compile(const Device& device) {
    wgpu::ShaderSourceWGSL wgsl{};
    wgsl.code = mSource.c_str();

    wgpu::ShaderModuleDescriptor desc{};
    desc.label = mLabel.c_str();
    desc.nextInChain = &wgsl;

    mModule = device.GetDevice().CreateShaderModule(&desc);
    return mModule != nullptr;
}

bool Shader::Reload(Device& device) {
    wgpu::ShaderModule oldModule = mModule;
    mModule = nullptr;

    if (!Compile(device)) {
        log::Error("[wgpu] Shader reload failed: {}", mLabel);
        mModule = oldModule;
        return false;
    }

    log::Info("[wgpu] Shader reloaded: {}", mLabel);
    return true;
}

wgpu::ShaderModule Shader::GetHandle() const noexcept { return mModule; }
const std::string& Shader::GetVertexEntry() const noexcept { return mVertexEntry; }
const std::string& Shader::GetFragmentEntry() const noexcept { return mFragmentEntry; }
const std::string& Shader::GetLabel() const noexcept { return mLabel; }
const std::string& Shader::GetSource() const noexcept { return mSource; }

} // namespace ct
