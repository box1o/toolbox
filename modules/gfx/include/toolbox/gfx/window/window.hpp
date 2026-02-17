#pragma once

#include <functional>
#include <string>
#include <toolbox/base/base.hpp>
#include <unordered_map>

namespace ct {

enum class CursorMode : u8 { Normal, Hidden, Disabled };
enum class CursorType : u8 { Arrow, IBeam, Crosshair, Hand, HResize, VResize };

using CallbackId = u32;

struct WindowInfo {
    std::string title{"toolbox"};
    u32 width{800};
    u32 height{600};
    bool floating{false};
    bool fullscreen{false};
    bool resizable{true};
    bool decorated{true};
};

class Window {
public:
    using ResizeCallback = std::function<void(u32 width, u32 height)>;
    ~Window();

    [[nodiscard]] const std::string& GetTitle() const noexcept;
    [[nodiscard]] u32 GetWidth() const noexcept;
    [[nodiscard]] u32 GetHeight() const noexcept;
    [[nodiscard]] f32 GetAspectRatio() const noexcept;
    [[nodiscard]] bool IsFullScreen() const noexcept;

    [[nodiscard]] CursorMode GetCursorMode() const noexcept;
    void SetCursorMode(CursorMode mode) noexcept;

    [[nodiscard]] CursorType GetCursorType() const noexcept;
    void SetCursorType(CursorType type) noexcept;

    [[nodiscard]] f32 GetContentScaleX() const noexcept;
    [[nodiscard]] f32 GetContentScaleY() const noexcept;

    [[nodiscard]] void* GetNativeHandle() const noexcept;
    [[nodiscard]] bool ShouldClose() const noexcept;
    void PollEvents() const noexcept;
    void Close() noexcept;

    [[nodiscard]] CallbackId AddResizeCallback(ResizeCallback callback);
    void RemoveResizeCallback(CallbackId id);

    [[nodiscard]] static result<ref<Window>> Create(const WindowInfo& info = {}) noexcept;

private:
    Window() = default;
    bool InitializeGLFW();
    bool InitializeWindow(const WindowInfo& info);
    void Terminate();
    void SetupCallbacks();
    void HandleResize(u32 width, u32 height);

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    void SetupEmscriptenResize();
#endif

    struct Impl;
    scope<Impl> mImpl;

    std::unordered_map<CallbackId, ResizeCallback> mResizeCallbacks;
    CallbackId mNextCallbackId{0};

    std::string mTitle{"toolbox"};
    u32 mWidth{0};
    u32 mHeight{0};
    f32 mAspectRatio{1.0f};
    bool mFullscreen{false};

    CursorMode mCursorMode{CursorMode::Normal};
    CursorType mCursorType{CursorType::Arrow};

    f32 mContentScaleX{1.0f};
    f32 mContentScaleY{1.0f};

    static inline u32 sWindowCount{0};
    static inline bool sInitialized{false};
};

} // namespace ct
