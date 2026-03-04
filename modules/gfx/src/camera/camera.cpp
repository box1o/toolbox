#include "../include/toolbox/gfx/camera/camera.hpp"
#include "toolbox/math/interop/transform.hpp"

namespace ct {

Camera::Camera() { Rebuild(); }

Camera::Camera(const CameraDesc& desc)
    : mName(desc.name), mProjDesc(desc.projection), mPosition(desc.position), mTarget(desc.target),
      mUp(desc.up) {
    Rebuild();
}

Camera::Camera(const CameraProjection& projection) : mProjDesc(projection) { Rebuild(); }

Camera::Camera(const CameraProjection& projection, const vec3f& position, const vec3f& target)
    : mProjDesc(projection), mPosition(position), mTarget(target) {
    Rebuild();
}

void Camera::SetName(std::string_view name) { mName = name; }

void Camera::SetPosition(const vec3f& position) noexcept {
    mPosition = position;
    RebuildView();
    RebuildViewProjection();
}

void Camera::SetTarget(const vec3f& target) noexcept {
    mTarget = target;
    RebuildView();
    RebuildViewProjection();
}

void Camera::SetUp(const vec3f& up) noexcept {
    mUp = up;
    RebuildView();
    RebuildViewProjection();
}

void Camera::SetPose(const vec3f& position, const vec3f& target, const vec3f& up) noexcept {
    mPosition = position;
    mTarget = target;
    mUp = up;
    RebuildView();
    RebuildViewProjection();
}

void Camera::SetPose(const vec3f& position, const vec3f& target) noexcept {
    mPosition = position;
    mTarget = target;
    RebuildView();
    RebuildViewProjection();
}

void Camera::SetProjection(const CameraProjection& proj) noexcept {
    mProjDesc = proj;
    RebuildProjection();
    RebuildViewProjection();
}

void Camera::SetAspectRatio(f32 aspect) noexcept {
    mProjDesc.aspect = aspect;
    RebuildProjection();
    RebuildViewProjection();
}

vec3f Camera::GetForward() const noexcept { return (mTarget - mPosition).normalize(); }
vec3f Camera::GetRight() const noexcept { return cross(GetForward(), mUp).normalize(); }
vec3f Camera::GetUpDerived() const noexcept { return cross(GetRight(), GetForward()).normalize(); }

void Camera::Rebuild() noexcept {
    RebuildView();
    RebuildProjection();
    RebuildViewProjection();
}

void Camera::RebuildView() noexcept { mView = lookAt(mPosition, mTarget, mUp); }

void Camera::RebuildProjection() noexcept {
    switch (mProjDesc.type) {
    case CameraProjectionType::Perspective:
        mProj = perspective(mProjDesc.yfov, mProjDesc.aspect, mProjDesc.znear, mProjDesc.zfar);
        break;
    case CameraProjectionType::Orthographic:
        const f32 halfX = mProjDesc.xmag * 0.5f;
        const f32 halfY = mProjDesc.ymag * 0.5f;
        mProj = ortho(-halfX, halfX, -halfY, halfY, mProjDesc.znear, mProjDesc.zfar);
        break;
    }
}

void Camera::RebuildViewProjection() noexcept { mViewProj = mProj * mView; }

} // namespace ct
