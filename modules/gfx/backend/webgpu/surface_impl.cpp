#include "surface_impl.hpp"
#include "device_impl.hpp"

#include <toolbox/base/logger/logger.hpp>
#include <toolbox/gfx/window/window.hpp>

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#define GLFW_EXPOSE_NATIVE_EMSCRIPTEN
#ifndef GLFW_PLATFORM_EMSCRIPTEN
#define GLFW_PLATFORM_EMSCRIPTEN 0
#endif
#else
#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#endif

#ifdef GLFW_EXPOSE_NATIVE_COCOA
#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>
#endif

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3native.h>
#endif

#ifdef None
#undef None
#endif
#ifdef Always
#undef Always
#endif
#ifdef Success
#undef Success
#endif
#ifdef Bool
#undef Bool
#endif
#ifdef Status
#undef Status
#endif
#ifdef True
#undef True
#endif
#ifdef False
#undef False
#endif

namespace ct::gfx::webgpu {

bool SurfaceImpl::Init(ref<Device> device, ref<Window> window, const SurfaceDesc&) noexcept {
    if (!device || !window) return false;

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("SurfaceImpl: device is not WebGPU device");
        return false;
    }

    mSurface = CreateNativeSurface(dev->InstanceHandle(), window);
    if (!mSurface) {
        log::Error("SurfaceImpl: failed to create native surface");
        return false;
    }
    return true;
}

wgpu::Surface CreateNativeSurface(const wgpu::Instance& instance, ref<Window> window) {
    wgpu::SurfaceDescriptor sd{};
    wgpu::Surface surface{nullptr};

#ifdef __EMSCRIPTEN__
    wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector canvas{};
    canvas.selector = {"canvas", 6};
    sd.nextInChain = &canvas;
    surface = instance.CreateSurface(&sd);
    return surface;
#else
    if (!window) return {};

    auto* glfwWin = static_cast<GLFWwindow*>(window->GetNativeHandle());
    if (!glfwWin) return {};

    const int platform = glfwGetPlatform();

#if defined(GLFW_EXPOSE_NATIVE_X11)
    if (platform == GLFW_PLATFORM_X11) {
        void* xDisplay = reinterpret_cast<void*>(glfwGetX11Display());
        uint64_t xWindow = static_cast<uint64_t>(glfwGetX11Window(glfwWin));
        if (xDisplay && xWindow) {
            wgpu::SurfaceSourceXlibWindow src{};
            src.display = xDisplay;
            src.window = xWindow;
            sd.nextInChain = &src;
            return instance.CreateSurface(&sd);
        }
    }
#endif

#if defined(GLFW_EXPOSE_NATIVE_WAYLAND)
    if (platform == GLFW_PLATFORM_WAYLAND) {
        void* wlDisplay = reinterpret_cast<void*>(glfwGetWaylandDisplay());
        void* wlSurface = reinterpret_cast<void*>(glfwGetWaylandWindow(glfwWin));
        if (wlDisplay && wlSurface) {
            wgpu::SurfaceSourceWaylandSurface src{};
            src.display = wlDisplay;
            src.surface = wlSurface;
            sd.nextInChain = &src;
            return instance.CreateSurface(&sd);
        }
    }
#endif

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
    if (platform == GLFW_PLATFORM_WIN32) {
        HWND hwnd = glfwGetWin32Window(glfwWin);
        HINSTANCE hinst = GetModuleHandle(nullptr);
        if (hwnd && hinst) {
            wgpu::SurfaceSourceWindowsHWND src{};
            src.hinstance = hinst;
            src.hwnd = hwnd;
            sd.nextInChain = &src;
            return instance.CreateSurface(&sd);
        }
    }
#endif

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
    if (platform == GLFW_PLATFORM_COCOA) {
        id metalLayer = [CAMetalLayer layer];
        NSWindow* nsWindow = glfwGetCocoaWindow(glfwWin);
        if (!nsWindow) return {};

        [nsWindow.contentView setWantsLayer:YES];
        [nsWindow.contentView setLayer:metalLayer];

        wgpu::SurfaceSourceMetalLayer src{};
        src.layer = metalLayer;
        sd.nextInChain = &src;
        return instance.CreateSurface(&sd);
    }
#endif

    log::Error("CreateNativeSurface: unsupported GLFW platform {}", platform);
    return {};
#endif
}

} // namespace ct::gfx::webgpu
