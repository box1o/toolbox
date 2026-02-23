#include <toolbox/base/logger/logger.hpp>
#include <toolbox/gfx/window/window.hpp>

#include <toolbox/gfx/events/input/events.hpp>
#include <toolbox/gfx/events/window/events.hpp>

#include <GLFW/glfw3.h>

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
#include <emscripten/html5.h>
#endif

namespace ct::gfx {

struct Window::Impl {
    GLFWwindow* window{nullptr};
    GLFWcursor* cursor{nullptr};

    ~Impl() {
        if (cursor) {
            glfwDestroyCursor(cursor);
            cursor = nullptr;
        }
    }
};

namespace {

static void GlfwErrorCallback(int error, const char* desc) {
    log::Error("GLFW Error ({}): {}", error, desc ? desc : "(null)");
}

static GLFWcursor* MakeCursor(CursorType type) noexcept {
    switch (type) {
    case CursorType::Arrow:     return glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    case CursorType::IBeam:     return glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    case CursorType::Crosshair: return glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    case CursorType::Hand:      return glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    case CursorType::HResize:   return glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    case CursorType::VResize:   return glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    default:                    return nullptr;
    }
}

static Window* GetSelf(GLFWwindow* w) noexcept {
    return static_cast<Window*>(glfwGetWindowUserPointer(w));
}

// GLFW mouse buttons are 0..7 and match your MouseButton values.
static ct::events::MouseButton ToMouseButton(int glfwButton) noexcept {
    const int clamped = (glfwButton < 0) ? 0 : (glfwButton > 7 ? 7 : glfwButton);
    return static_cast<ct::events::MouseButton>(static_cast<u8>(clamped));
}

// GLFW key codes are ints; your KeyCode enum contains many matching values.
static ct::events::KeyCode ToKeyCode(int glfwKey) noexcept {
    return static_cast<ct::events::KeyCode>(static_cast<u16>(glfwKey));
}

} // namespace

result<ref<Window>> Window::Create(const WindowInfo& info) noexcept {
    ref<Window> win(new Window());
    win->mImpl = scope<Impl>(new Impl());

    if (auto res = win->InitGLFW(); !res) return err(res.error());
    if (auto res = win->CreateWindowGLFW(info); !res) return err(res.error());

    return win;
}

Window::~Window() { Close(); }

result<void> Window::InitGLFW() noexcept {
    if (sGlfwInit) {
        log::Critical("GLFW is already initialized");
        return ok();
    }

    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit()) {
        log::Critical("GLFW init failed");
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "GLFW initialization failed");
    }

    sGlfwInit = true;
    return ok();
}

result<void> Window::CreateWindowGLFW(const WindowInfo& info) noexcept {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_FLOATING,  info.floating  ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, info.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, info.decorated ? GLFW_TRUE : GLFW_FALSE);

    GLFWmonitor* monitor = info.fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    mImpl->window = glfwCreateWindow(
        static_cast<int>(info.width),
        static_cast<int>(info.height),
        info.title.c_str(),
        monitor,
        nullptr);

    if (!mImpl->window) {
        log::Critical("Failed to create GLFW window");
        if (sWindowCount == 0) ShutdownGLFW();
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "GLFW window creation failed");
    }

    ++sWindowCount;

    mTitle      = info.title;
    mWidth      = info.width;
    mHeight     = info.height;
    mFullscreen = info.fullscreen;
    mAspectRatio = (mHeight > 0) ? static_cast<f32>(mWidth) / static_cast<f32>(mHeight) : 1.0f;

    float sx = 1.0f, sy = 1.0f;
    glfwGetWindowContentScale(mImpl->window, &sx, &sy);
    mContentScaleX = sx;
    mContentScaleY = sy;

    SetupCallbacks();

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    SetupEmscriptenResize();
#endif

    return ok();
}

void Window::ShutdownGLFW() noexcept {
    if (!sGlfwInit) return;
    glfwTerminate();
    sGlfwInit = false;
}

void* Window::GetNativeHandle() const noexcept {
    return (mImpl && mImpl->window) ? static_cast<void*>(mImpl->window) : nullptr;
}

bool Window::ShouldClose() const noexcept {
    if (!mImpl || !mImpl->window) return true;
    return glfwWindowShouldClose(mImpl->window) != 0;
}

void Window::PollEvents() const noexcept {
    // On desktop and web builds, GLFW routes platform/browser events into these callbacks.
    glfwPollEvents();
}

void Window::Close() noexcept {
    if (!mImpl || !mImpl->window) return;

    // Do NOT emit WindowCloseEvent here: GLFW close callback emits it for user-close requests.
    glfwSetWindowUserPointer(mImpl->window, nullptr);
    glfwDestroyWindow(mImpl->window);
    mImpl->window = nullptr;

    if (sWindowCount > 0) --sWindowCount;
    if (sWindowCount == 0) ShutdownGLFW();
}

void Window::SetEventCallback(EventCallback callback) noexcept {
    mEventCallback = std::move(callback);
}

bool Window::HasEventCallback() const noexcept {
    return static_cast<bool>(mEventCallback);
}

void Window::DispatchEvent(ct::events::EventBase& e) noexcept {
    if (mEventCallback) mEventCallback(e);
}

void Window::SetCursorMode(CursorMode mode) noexcept {
    if (!mImpl || !mImpl->window) return;

    mCursorMode = mode;
    int value = GLFW_CURSOR_NORMAL;

    switch (mode) {
    case CursorMode::Normal:   value = GLFW_CURSOR_NORMAL; break;
    case CursorMode::Hidden:   value = GLFW_CURSOR_HIDDEN; break;
    case CursorMode::Disabled: value = GLFW_CURSOR_DISABLED; break;
    }

    glfwSetInputMode(mImpl->window, GLFW_CURSOR, value);
}

void Window::SetCursorType(CursorType type) noexcept {
    if (!mImpl || !mImpl->window) return;

    GLFWcursor* newCursor = MakeCursor(type);
    if (!newCursor) return;

    glfwSetCursor(mImpl->window, newCursor);
    if (mImpl->cursor) glfwDestroyCursor(mImpl->cursor);

    mImpl->cursor = newCursor;
    mCursorType = type;
}

void Window::HandleResize(u32 width, u32 height) noexcept {
    if (width == 0 || height == 0) return;
    if (width == mWidth && height == mHeight) return;

    mWidth  = width;
    mHeight = height;
    mAspectRatio = static_cast<f32>(mWidth) / static_cast<f32>(mHeight);

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    // Keep GLFW window size in sync with canvas pixel size on web builds.
    if (mImpl && mImpl->window) {
        glfwSetWindowSize(mImpl->window, static_cast<int>(width), static_cast<int>(height));
    }
#endif

    ct::events::WindowResizeEvent e{mWidth, mHeight};
    DispatchEvent(e);
}

void Window::SetupCallbacks() noexcept {
    glfwSetWindowUserPointer(mImpl->window, this);

    // --------------------------
    // Window events
    // --------------------------

    glfwSetWindowCloseCallback(mImpl->window, [](GLFWwindow* w) {
        if (auto* self = GetSelf(w)) {
            ct::events::WindowCloseEvent e{};
            self->DispatchEvent(e);
        }
    });

    // Framebuffer resize is the correct signal for swapchain/viewport size.
    glfwSetFramebufferSizeCallback(mImpl->window, [](GLFWwindow* w, int width, int height) {
        if (auto* self = GetSelf(w)) {
            self->HandleResize(static_cast<u32>(width), static_cast<u32>(height));
        }
    });

    glfwSetWindowContentScaleCallback(mImpl->window, [](GLFWwindow* w, float sx, float sy) {
        if (auto* self = GetSelf(w)) {
            self->mContentScaleX = sx;
            self->mContentScaleY = sy;
        }
    });

    glfwSetWindowFocusCallback(mImpl->window, [](GLFWwindow* w, int focused) {
        if (auto* self = GetSelf(w)) {
            if (focused) {
                ct::events::WindowFocusEvent e{};
                self->DispatchEvent(e);
            } else {
                ct::events::WindowLostFocusEvent e{};
                self->DispatchEvent(e);
            }
        }
    });

    glfwSetWindowPosCallback(mImpl->window, [](GLFWwindow* w, int x, int y) {
        if (auto* self = GetSelf(w)) {
            ct::events::WindowMovedEvent e{static_cast<i32>(x), static_cast<i32>(y)};
            self->DispatchEvent(e);
        }
    });

    glfwSetWindowIconifyCallback(mImpl->window, [](GLFWwindow* w, int iconified) {
        if (auto* self = GetSelf(w)) {
            if (iconified) {
                ct::events::WindowMinimizedEvent e{};
                self->DispatchEvent(e);
            } else {
                ct::events::WindowRestoredEvent e{};
                self->DispatchEvent(e);
            }
        }
    });

    glfwSetWindowMaximizeCallback(mImpl->window, [](GLFWwindow* w, int maximized) {
        if (auto* self = GetSelf(w)) {
            if (maximized) {
                ct::events::WindowMaximizedEvent e{};
                self->DispatchEvent(e);
            } else {
                ct::events::WindowRestoredEvent e{};
                self->DispatchEvent(e);
            }
        }
    });

    // --------------------------
    // Keyboard events
    // --------------------------

    glfwSetKeyCallback(mImpl->window, [](GLFWwindow* w, int key, int, int action, int) {
        if (auto* self = GetSelf(w)) {
            const auto kc = ToKeyCode(key);

            if (action == GLFW_PRESS) {
                ct::events::KeyPressedEvent e{kc, 0};
                self->DispatchEvent(e);
            } else if (action == GLFW_REPEAT) {
                ct::events::KeyPressedEvent e{kc, 1};
                self->DispatchEvent(e);
            } else if (action == GLFW_RELEASE) {
                ct::events::KeyReleasedEvent e{kc};
                self->DispatchEvent(e);
            }
        }
    });

    glfwSetCharCallback(mImpl->window, [](GLFWwindow* w, unsigned int codepoint) {
        if (auto* self = GetSelf(w)) {
            ct::events::KeyTypedEvent e{static_cast<u32>(codepoint)};
            self->DispatchEvent(e);
        }
    });

    // --------------------------
    // Mouse events
    // --------------------------

    glfwSetCursorPosCallback(mImpl->window, [](GLFWwindow* w, double x, double y) {
        if (auto* self = GetSelf(w)) {
            const f32 fx = static_cast<f32>(x);
            const f32 fy = static_cast<f32>(y);

            f32 dx = 0.0f;
            f32 dy = 0.0f;

            if (self->mHasLastMousePos) {
                dx = fx - self->mLastMouseX;
                dy = fy - self->mLastMouseY;
            } else {
                self->mHasLastMousePos = true;
            }

            self->mLastMouseX = fx;
            self->mLastMouseY = fy;

            ct::events::MouseMovedEvent e{fx, fy, dx, dy};
            self->DispatchEvent(e);
        }
    });

    glfwSetScrollCallback(mImpl->window, [](GLFWwindow* w, double ox, double oy) {
        if (auto* self = GetSelf(w)) {
            ct::events::MouseScrolledEvent e{static_cast<f32>(ox), static_cast<f32>(oy)};
            self->DispatchEvent(e);
        }
    });

    glfwSetMouseButtonCallback(mImpl->window, [](GLFWwindow* w, int button, int action, int) {
        if (auto* self = GetSelf(w)) {
            const auto mb = ToMouseButton(button);

            double x = 0.0, y = 0.0;
            glfwGetCursorPos(w, &x, &y);
            const f32 fx = static_cast<f32>(x);
            const f32 fy = static_cast<f32>(y);

            if (action == GLFW_PRESS) {
                ct::events::MouseButtonPressedEvent e{mb, fx, fy};
                self->DispatchEvent(e);
            } else if (action == GLFW_RELEASE) {
                ct::events::MouseButtonReleasedEvent e{mb, fx, fy};
                self->DispatchEvent(e);

                // Convenience event (release -> click)
                ct::events::MouseButtonClickedEvent c{mb, fx, fy};
                self->DispatchEvent(c);
            }
        }
    });

    glfwSetCursorEnterCallback(mImpl->window, [](GLFWwindow* w, int entered) {
        if (auto* self = GetSelf(w)) {
            if (entered) {
                ct::events::MouseEnteredEvent e{};
                self->DispatchEvent(e);
            } else {
                ct::events::MouseLeftEvent e{};
                self->DispatchEvent(e);
            }
        }
    });
}

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
void Window::SetupEmscriptenResize() noexcept {
    auto cb = [](int, const EmscriptenUiEvent*, void* userData) -> EM_BOOL {
        auto* self = static_cast<Window*>(userData);
        if (!self) return EM_FALSE;

        double cssW = 0.0, cssH = 0.0;
        emscripten_get_element_css_size("canvas", &cssW, &cssH);

        const double dpr = emscripten_get_device_pixel_ratio();
        const u32 pixelW = static_cast<u32>(cssW * dpr);
        const u32 pixelH = static_cast<u32>(cssH * dpr);

        if (pixelW == 0 || pixelH == 0) return EM_FALSE;

        emscripten_set_canvas_element_size("canvas", static_cast<int>(pixelW), static_cast<int>(pixelH));
        self->HandleResize(pixelW, pixelH);
        return EM_TRUE;
    };

    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, cb);

    // Initial sizing
    double cssW = 0.0, cssH = 0.0;
    emscripten_get_element_css_size("canvas", &cssW, &cssH);

    const double dpr = emscripten_get_device_pixel_ratio();
    const u32 pixelW = static_cast<u32>(cssW * dpr);
    const u32 pixelH = static_cast<u32>(cssH * dpr);

    if (pixelW > 0 && pixelH > 0) {
        emscripten_set_canvas_element_size("canvas", static_cast<int>(pixelW), static_cast<int>(pixelH));
        HandleResize(pixelW, pixelH);
    }
}
#endif

const std::string& Window::GetTitle() const noexcept { return mTitle; }
u32 Window::GetWidth() const noexcept { return mWidth; }
u32 Window::GetHeight() const noexcept { return mHeight; }
f32 Window::GetAspectRatio() const noexcept { return mAspectRatio; }
bool Window::IsFullScreen() const noexcept { return mFullscreen; }
f32 Window::GetContentScaleX() const noexcept { return mContentScaleX; }
f32 Window::GetContentScaleY() const noexcept { return mContentScaleY; }
CursorMode Window::GetCursorMode() const noexcept { return mCursorMode; }
CursorType Window::GetCursorType() const noexcept { return mCursorType; }

} // namespace ct::gfx
