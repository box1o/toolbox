#include "toolbox/gfx/window/window.hpp"
#include "toolbox/base/base.hpp"

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

namespace detail {

static void ErrorCallback(int error, const char* description) {
    log::Error("GLFW Error ({}): {}", error, description ? description : "(null)");
}

[[nodiscard]] static GLFWcursor* CreateGlfwCursor(CursorType type) noexcept {
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
    // NOTE: Window() is private, so we cannot use make_shared/createRef here.
    ref<Window> win(new Window());
    win->mImpl = scope<Impl>(new Impl());

    if (!win->InitializeGLFW()) {
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "Failed to initialize GLFW");
    }
    if (!win->InitializeWindow(info)) {
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "Failed to create window");
    }
    return ok(win);
}

Window::~Window() { Close(); }

bool Window::InitializeGLFW() noexcept {
    if (sInitialized) return true;

    glfwSetErrorCallback(detail::ErrorCallback);
    if (!glfwInit()) {
        log::Critical("Failed to initialize GLFW");
        return false;
    }

    sInitialized = true;
    return true;
}

bool Window::InitializeWindow(const WindowInfo& info) noexcept {
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
        info.title.c_str(),
        monitor,
        nullptr);

    if (!mImpl->window) {
        log::Critical("Failed to create GLFW window");
        if (sWindowCount == 0) TerminateGLFW();
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

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    SetupEmscriptenResize();
#endif

    return true;
}

void Window::TerminateGLFW() noexcept {
    if (!sInitialized) return;
    glfwTerminate();
    sInitialized = false;
}

void* Window::GetNativeHandle() const noexcept {
    return (mImpl && mImpl->window) ? static_cast<void*>(mImpl->window) : nullptr;
}

bool Window::ShouldClose() const noexcept {
    return (!mImpl || !mImpl->window) ? true : (glfwWindowShouldClose(mImpl->window) != 0);
}

void Window::PollEvents() const noexcept { glfwPollEvents(); }

void Window::Close() noexcept {
    if (!mImpl || !mImpl->window) return;

    glfwSetWindowUserPointer(mImpl->window, nullptr);
    glfwDestroyWindow(mImpl->window);
    mImpl->window = nullptr;

    if (sWindowCount > 0) --sWindowCount;
    if (sWindowCount == 0) TerminateGLFW();
}

void Window::SetCursorMode(CursorMode mode) noexcept {
    if (!mImpl || !mImpl->window) return;

    mCursorMode = mode;

    int value = GLFW_CURSOR_NORMAL;
    switch (mode) {
    case CursorMode::Normal:   value = GLFW_CURSOR_NORMAL;   break;
    case CursorMode::Hidden:   value = GLFW_CURSOR_HIDDEN;   break;
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

void Window::HandleResize(u32 width, u32 height) noexcept {
    if (width == 0 || height == 0) return;
    if (width == mWidth && height == mHeight) return;

    mWidth = width;
    mHeight = height;
    mAspectRatio = static_cast<f32>(mWidth) / static_cast<f32>(mHeight);

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    if (mImpl && mImpl->window) {
        glfwSetWindowSize(mImpl->window, static_cast<int>(width), static_cast<int>(height));
    }
#endif

    for (const auto& [id, cb] : mResizeCallbacks) {
        (void)id;
        if (cb) cb(mWidth, mHeight);
    }
}

void Window::SetupCallbacks() noexcept {
    glfwSetWindowUserPointer(mImpl->window, this);

    glfwSetFramebufferSizeCallback(mImpl->window, [](GLFWwindow* window, int width, int height) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!self) return;
        self->HandleResize(static_cast<u32>(width), static_cast<u32>(height));
    });

    glfwSetWindowContentScaleCallback(mImpl->window, [](GLFWwindow* window, float sx, float sy) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!self) return;
        self->mContentScaleX = sx;
        self->mContentScaleY = sy;
    });
}

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
void Window::SetupEmscriptenResize() noexcept {
    auto resizeCallback = [](int, const EmscriptenUiEvent*, void* userData) -> EM_BOOL {
        auto* self = static_cast<Window*>(userData);
        if (!self) return EM_FALSE;

        double cssW = 0.0, cssH = 0.0;
        emscripten_get_element_css_size("canvas", &cssW, &cssH);

        double dpr = emscripten_get_device_pixel_ratio();
        u32 pixelW = static_cast<u32>(cssW * dpr);
        u32 pixelH = static_cast<u32>(cssH * dpr);

        if (pixelW == 0 || pixelH == 0) return EM_FALSE;

        emscripten_set_canvas_element_size("canvas", static_cast<int>(pixelW), static_cast<int>(pixelH));
        self->HandleResize(pixelW, pixelH);
        return EM_TRUE;
    };

    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, resizeCallback);

    double cssW = 0.0, cssH = 0.0;
    emscripten_get_element_css_size("canvas", &cssW, &cssH);
    double dpr = emscripten_get_device_pixel_ratio();
    u32 pixelW = static_cast<u32>(cssW * dpr);
    u32 pixelH = static_cast<u32>(cssH * dpr);

    if (pixelW > 0 && pixelH > 0) {
        emscripten_set_canvas_element_size("canvas", static_cast<int>(pixelW), static_cast<int>(pixelH));
        HandleResize(pixelW, pixelH);
    }
}
#endif

CallbackId Window::AddResizeCallback(ResizeCallback callback) {
    const CallbackId id = mNextCallbackId++;
    mResizeCallbacks.emplace(id, std::move(callback));
    return id;
}

void Window::RemoveResizeCallback(CallbackId id) { mResizeCallbacks.erase(id); }

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
