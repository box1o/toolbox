#include "toolbox/gfx/window/window.hpp"
#include "toolbox/base/base.hpp"

#include <GLFW/glfw3.h>

namespace ct {

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

namespace detail {

void ErrorCallback(int error, const char* description) {
    log::Error("GLFW Error ({}): {}", error, description ? description : "(null)");
}

[[nodiscard]] GLFWcursor* CreateGlfwCursor(CursorType type) noexcept {
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

} // namespace detail

result<ref<Window>> Window::Create(const WindowInfo& info) noexcept {
    ref<Window> win(new Window());
    win->mImpl = scope<Impl>(new Impl());

    if (!win->InitializeGLFW()) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to initialize GLFW");
    }
    if (!win->InitializeWindow(info)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create window");
    }

    return win;
}

Window::~Window() {
    Close();
    Terminate();
}

bool Window::InitializeGLFW() {
    if (sInitialized) return true;

    if (!glfwInit()) {
        log::Critical("Failed to initialize GLFW");
        return false;
    }
    glfwSetErrorCallback(detail::ErrorCallback);
    sInitialized = true;
    return true;
}

bool Window::InitializeWindow(const WindowInfo& info) {
#if defined(GLFW_PLATFORM_X11) && defined(GLFW_PLATFORM)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_FLOATING, info.floating ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, info.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, info.decorated ? GLFW_TRUE : GLFW_FALSE);

    GLFWmonitor* monitor = info.fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    mImpl->window = glfwCreateWindow(
        static_cast<int>(info.width),
        static_cast<int>(info.height),
        info.title.c_str(), monitor, nullptr);

    if (!mImpl->window) {
        if (sWindowCount == 0) Terminate();
        log::Critical("Failed to create GLFW window");
        return false;
    }

    ++sWindowCount;
    mTitle = info.title;
    mWidth = info.width;
    mHeight = info.height;
    mFullscreen = info.fullscreen;
    mAspectRatio = (mHeight > 0) ? static_cast<f32>(mWidth) / static_cast<f32>(mHeight) : 1.0f;

    float sx = 1.0f, sy = 1.0f;
    glfwGetWindowContentScale(mImpl->window, &sx, &sy);
    mContentScaleX = sx;
    mContentScaleY = sy;

    SetupCallbacks();
    return true;
}

void Window::Terminate() {
    if (!sInitialized) return;
    glfwTerminate();
    sInitialized = false;
}

void* Window::GetNativeHandle() const noexcept {
    return mImpl ? static_cast<void*>(mImpl->window) : nullptr;
}

bool Window::ShouldClose() const noexcept {
    return !mImpl || !mImpl->window || glfwWindowShouldClose(mImpl->window) != 0;
}

void Window::PollEvents() const noexcept {
    glfwPollEvents();
}

void Window::Close() noexcept {
    if (!mImpl || !mImpl->window) return;

    glfwDestroyWindow(mImpl->window);
    mImpl->window = nullptr;
    --sWindowCount;
    if (sWindowCount == 0) Terminate();
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

    GLFWcursor* newCursor = detail::CreateGlfwCursor(type);
    if (!newCursor) return;

    glfwSetCursor(mImpl->window, newCursor);

    if (mImpl->cursor) glfwDestroyCursor(mImpl->cursor);
    mImpl->cursor = newCursor;
    mCursorType = type;
}

void Window::SetupCallbacks() {
    glfwSetWindowUserPointer(mImpl->window, this);

    glfwSetFramebufferSizeCallback(mImpl->window, [](GLFWwindow* window, int width, int height) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!self) return;

        self->mWidth = static_cast<u32>(width);
        self->mHeight = static_cast<u32>(height);
        self->mAspectRatio = (self->mHeight > 0)
            ? static_cast<f32>(self->mWidth) / static_cast<f32>(self->mHeight)
            : 1.0f;

        for (const auto& cb : self->mResizeCallbacks) {
            if (cb) cb(self->mWidth, self->mHeight);
        }
    });

    glfwSetWindowContentScaleCallback(mImpl->window, [](GLFWwindow* window, float sx, float sy) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!self) return;
        self->mContentScaleX = sx;
        self->mContentScaleY = sy;
    });
}

void Window::SetResizeCallback(ResizeCallback callback) {
    mResizeCallbacks.emplace_back(std::move(callback));
}

f32 Window::GetContentScaleX() const noexcept { return mContentScaleX; }
f32 Window::GetContentScaleY() const noexcept { return mContentScaleY; }
const std::string& Window::GetTitle() const noexcept { return mTitle; }
u32 Window::GetWidth() const noexcept { return mWidth; }
u32 Window::GetHeight() const noexcept { return mHeight; }
bool Window::IsFullScreen() const noexcept { return mFullscreen; }
f32 Window::GetAspectRatio() const noexcept { return mAspectRatio; }
CursorMode Window::GetCursorMode() const noexcept { return mCursorMode; }
CursorType Window::GetCursorType() const noexcept { return mCursorType; }

} // namespace ct
