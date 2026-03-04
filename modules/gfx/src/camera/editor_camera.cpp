#include "../include/toolbox/gfx/camera/editor_camera.hpp"
#include "toolbox/gfx/events/dispatcher.hpp"

namespace ct {

EditorCamera::EditorCamera() { RebuildCameraPose(); }

EditorCamera::EditorCamera(const EditorCameraDesc& desc)
    : mConfig(desc.config), mViewportWidth(desc.viewportWidth),
      mViewportHeight(desc.viewportHeight) {

    const f32 aspect = (mViewportHeight > 0)
                           ? static_cast<f32>(mViewportWidth) / static_cast<f32>(mViewportHeight)
                           : 1.0f;

    CameraProjection proj = desc.projection;
    proj.aspect = aspect;

    mCamera = Camera(CameraDesc{.name = desc.name,
        .projection = proj,
        .position = ComputeEyePosition(),
        .target = mConfig.pivot});

    RebuildCameraPose();
}

// ─── Setters ───

void EditorCamera::SetPivot(const vec3f& pivot) noexcept {
    mConfig.pivot = pivot;
    RebuildCameraPose();
}

void EditorCamera::SetDistance(f32 distance) noexcept {
    mConfig.distance = clamp(distance, mConfig.minDistance, mConfig.maxDistance);
    RebuildCameraPose();
}

void EditorCamera::SetAngles(f32 yaw, f32 pitch) noexcept {
    mConfig.yaw = yaw;
    mConfig.pitch = clamp(pitch, mConfig.minPitch, mConfig.maxPitch);
    RebuildCameraPose();
}

void EditorCamera::SetViewportSize(u32 width, u32 height) noexcept {
    mViewportWidth = width;
    mViewportHeight = height;

    const f32 aspect = (height > 0) ? static_cast<f32>(width) / static_cast<f32>(height) : 1.0f;

    mCamera.SetAspectRatio(aspect);
}

void EditorCamera::Focus(const vec3f& center, f32 radius) noexcept {
    mConfig.pivot = center;

    // NOTE: compute distance so the sphere fits in the frustum
    const f32 halfFov = mCamera.GetProjection().yfov * 0.5f;
    const f32 sinHalf = sin(halfFov);
    if (sinHalf > 0.0001f) {
        mConfig.distance = radius / sinHalf;
    } else {
        mConfig.distance = radius * 2.0f;
    }

    mConfig.distance = clamp(mConfig.distance, mConfig.minDistance, mConfig.maxDistance);
    RebuildCameraPose();
}

void EditorCamera::Tick(f32 deltaTime) noexcept {
    // NOTE: reserved for future smooth damping / animation
    (void)deltaTime;
}

// NOTE: Events
void EditorCamera::OnEvent(events::EventBase& event) noexcept {
    events::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<events::MouseButtonPressedEvent>(
        [&](const events::MouseButtonPressedEvent& ev) {
            if (ev.button == mConfig.orbitButton) {
                mOrbiting = true;
                mLastMouseX = ev.x;
                mLastMouseY = ev.y;
                return true;
            }
            if (ev.button == mConfig.panButton) {
                mPanning = true;
                mLastMouseX = ev.x;
                mLastMouseY = ev.y;
                return true;
            }
            return false;
        });

    dispatcher.Dispatch<events::MouseButtonReleasedEvent>(
        [&](const events::MouseButtonReleasedEvent& ev) {
            if (ev.button == mConfig.orbitButton) {
                mOrbiting = false;
                return true;
            }
            if (ev.button == mConfig.panButton) {
                mPanning = false;
                return true;
            }
            return false;
        });

    dispatcher.Dispatch<events::MouseMovedEvent>([&](const events::MouseMovedEvent& ev) {
        const f32 dx = ev.x - mLastMouseX;
        const f32 dy = ev.y - mLastMouseY;
        mLastMouseX = ev.x;
        mLastMouseY = ev.y;

        if (mOrbiting) {
            ApplyOrbit(dx, dy);
            return true;
        }
        if (mPanning) {
            ApplyPan(dx, dy);
            return true;
        }
        return false;
    });

    dispatcher.Dispatch<events::MouseScrolledEvent>([&](const events::MouseScrolledEvent& ev) {
        ApplyDolly(ev.offsetY);
        return true;
    });

    dispatcher.Dispatch<events::KeyPressedEvent>([&](const events::KeyPressedEvent& ev) {
        if (ev.key == mConfig.fastModifier) {
            mFastMode = true;
        }
        return false;
    });

    dispatcher.Dispatch<events::KeyReleasedEvent>([&](const events::KeyReleasedEvent& ev) {
        if (ev.key == mConfig.fastModifier) {
            mFastMode = false;
        }
        return false;
    });
}

void EditorCamera::ApplyOrbit(f32 dx, f32 dy) noexcept {
    const f32 speed =
        mFastMode ? mConfig.rotateSpeed * mConfig.fastMultiplier : mConfig.rotateSpeed;

    mConfig.yaw -= dx * speed;
    mConfig.pitch += dy * speed;
    mConfig.pitch = clamp(mConfig.pitch, mConfig.minPitch, mConfig.maxPitch);

    RebuildCameraPose();
}

void EditorCamera::ApplyPan(f32 dx, f32 dy) noexcept {
    const f32 speed = mFastMode ? mConfig.panSpeed * mConfig.fastMultiplier : mConfig.panSpeed;

    // NOTE: scale pan by distance so it feels consistent at any zoom level
    const f32 scale = speed * mConfig.distance;

    const vec3f right = mCamera.GetRight();
    const vec3f up = mCamera.GetUpDerived();

    mConfig.pivot = mConfig.pivot - right * (dx * scale) + up * (dy * scale);

    RebuildCameraPose();
}

void EditorCamera::ApplyDolly(f32 delta) noexcept {
    const f32 speed = mFastMode ? mConfig.dollySpeed * mConfig.fastMultiplier : mConfig.dollySpeed;

    // NOTE: logarithmic scaling — dolly feels uniform regardless of distance
    const f32 factor = 1.0f - delta * speed * 0.1f;
    mConfig.distance *= factor;
    mConfig.distance = clamp(mConfig.distance, mConfig.minDistance, mConfig.maxDistance);

    RebuildCameraPose();
}

vec3f EditorCamera::ComputeEyePosition() const noexcept {
    // NOTE: spherical coordinates → cartesian
    //   yaw rotates around Y axis, pitch tilts up/down
    const f32 cp = cos(mConfig.pitch);
    const f32 sp = sin(mConfig.pitch);
    const f32 cy = cos(mConfig.yaw);
    const f32 sy = sin(mConfig.yaw);

    const vec3f offset{
        mConfig.distance * cp * sy, mConfig.distance * sp, mConfig.distance * cp * cy};

    return mConfig.pivot + offset;
}

void EditorCamera::RebuildCameraPose() noexcept {
    const vec3f eye = ComputeEyePosition();
    mCamera.SetPose(eye, mConfig.pivot);
}

} // namespace ct
