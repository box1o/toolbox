#pragma once

#include <string>
#include <string_view>

#include <toolbox/base/base.hpp>
#include <toolbox/math/math.hpp>

namespace ct {

enum class CameraProjectionType : u8 { Perspective, Orthographic };

struct CameraProjection {
    CameraProjectionType type{CameraProjectionType::Perspective};
    f32 yfov{radians(60.0f)};
    f32 aspect{16.0f / 9.0f};
    f32 znear{0.1f};
    f32 zfar{1000.0f};
    f32 xmag{1.0f};
    f32 ymag{1.0f};
};

struct CameraDesc {
    std::string name;
    CameraProjection projection{};
    vec3f position{0.0f, 2.0f, -3.0f};
    vec3f target{0.0f, 0.0f, 0.0f};
    vec3f up{0.0f, 1.0f, 0.0f};
};

class Camera {
public:
    Camera();
    explicit Camera(const CameraDesc& desc);
    explicit Camera(const CameraProjection& projection);
    Camera(const CameraProjection& projection, const vec3f& position, const vec3f& target);

    [[nodiscard]] const std::string& GetName() const noexcept { return mName; }
    [[nodiscard]] const CameraProjection& GetProjection() const noexcept { return mProjDesc; }
    [[nodiscard]] const vec3f& GetPosition() const noexcept { return mPosition; }
    [[nodiscard]] const vec3f& GetTarget() const noexcept { return mTarget; }
    [[nodiscard]] const vec3f& GetUp() const noexcept { return mUp; }
    [[nodiscard]] const mat4f& GetViewMatrix() const noexcept { return mView; }
    [[nodiscard]] const mat4f& GetProjectionMatrix() const noexcept { return mProj; }
    [[nodiscard]] const mat4f& GetViewProjection() const noexcept { return mViewProj; }

    void SetName(std::string_view name);
    void SetPosition(const vec3f& position) noexcept;
    void SetTarget(const vec3f& target) noexcept;
    void SetUp(const vec3f& up) noexcept;
    void SetPose(const vec3f& position, const vec3f& target, const vec3f& up) noexcept;
    void SetPose(const vec3f& position, const vec3f& target) noexcept;
    void SetProjection(const CameraProjection& proj) noexcept;
    void SetAspectRatio(f32 aspect) noexcept;

    [[nodiscard]] vec3f GetForward() const noexcept;
    [[nodiscard]] vec3f GetRight() const noexcept;
    [[nodiscard]] vec3f GetUpDerived() const noexcept;

private:
    std::string mName;
    CameraProjection mProjDesc{};

    vec3f mPosition{0.0f, 2.0f, -3.0f};
    vec3f mTarget{0.0f, 0.0f, 0.0f};
    vec3f mUp{0.0f, 1.0f, 0.0f};

    mat4f mView{mat4f::identity()};
    mat4f mProj{mat4f::identity()};
    mat4f mViewProj{mat4f::identity()};

    void Rebuild() noexcept;
    void RebuildView() noexcept;
    void RebuildProjection() noexcept;
    void RebuildViewProjection() noexcept;
};

} // namespace ct
