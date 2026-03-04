#pragma once

#include <string>

#include <toolbox/base/base.hpp>
#include <toolbox/math/math.hpp>

#include "camera.hpp"

#include "toolbox/gfx/events/base.hpp"
#include "toolbox/gfx/events/input/events.hpp"

namespace ct {

struct EditorCameraConfig {
    vec3f pivot{0.0f, 0.0f, 0.0f};

    f32 distance{5.0f};
    f32 minDistance{0.1f};
    f32 maxDistance{500.0f};

    f32 yaw{0.0f};
    f32 pitch{0.0f};
    f32 minPitch{-radians(89.0f)};
    f32 maxPitch{radians(89.0f)};

    f32 rotateSpeed{0.005f};
    f32 panSpeed{0.001f};
    f32 dollySpeed{0.5f};
    f32 fastMultiplier{3.0f};

    events::MouseButton orbitButton{events::MouseButton::Left};
    events::MouseButton panButton{events::MouseButton::Middle};
    events::KeyCode fastModifier{events::KeyCode::LeftShift};
};

struct EditorCameraDesc {
    std::string name;
    CameraProjection projection{};
    EditorCameraConfig config{};
    u32 viewportWidth{1280};
    u32 viewportHeight{720};
};

class EditorCamera {
public:
    EditorCamera();
    explicit EditorCamera(const EditorCameraDesc& desc);

    [[nodiscard]] Camera& GetCamera() noexcept { return mCamera; }
    [[nodiscard]] const Camera& GetCamera() const noexcept { return mCamera; }
    [[nodiscard]] EditorCameraConfig& GetConfig() noexcept { return mConfig; }
    [[nodiscard]] const EditorCameraConfig& GetConfig() const noexcept { return mConfig; }
    [[nodiscard]] const vec3f& GetPivot() const noexcept { return mConfig.pivot; }
    [[nodiscard]] f32 GetDistance() const noexcept { return mConfig.distance; }
    [[nodiscard]] f32 GetYaw() const noexcept { return mConfig.yaw; }
    [[nodiscard]] f32 GetPitch() const noexcept { return mConfig.pitch; }

    void SetPivot(const vec3f& pivot) noexcept;
    void SetDistance(f32 distance) noexcept;
    void SetAngles(f32 yaw, f32 pitch) noexcept;
    void SetViewportSize(u32 width, u32 height) noexcept;

    // NOTE: snaps camera to frame a sphere centered at 'center' with given 'radius'
    void Focus(const vec3f& center, f32 radius) noexcept;

    // NOTE: call once per frame — currently reserved for smooth damping
    void Tick(f32 deltaTime) noexcept;

    // NOTE: feed raw events from the event system
    void OnEvent(events::EventBase& event) noexcept;

private:
    Camera mCamera;
    EditorCameraConfig mConfig;

    u32 mViewportWidth{1280};
    u32 mViewportHeight{720};

    bool mOrbiting{false};
    bool mPanning{false};
    bool mFastMode{false};
    f32 mLastMouseX{0.0f};
    f32 mLastMouseY{0.0f};

    void ApplyOrbit(f32 dx, f32 dy) noexcept;
    void ApplyPan(f32 dx, f32 dy) noexcept;
    void ApplyDolly(f32 delta) noexcept;
    void RebuildCameraPose() noexcept;
    [[nodiscard]] vec3f ComputeEyePosition() const noexcept;
};

} // namespace ct
