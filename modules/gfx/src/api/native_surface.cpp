//NOTE: webgpu MUST be included BEFORE X11 headers to avoid macro collisions
#include <webgpu/webgpu_cpp.h>

#include "toolbox/base/base.hpp"
#include "toolbox/gfx/api/surface.hpp"
#include "toolbox/gfx/window/window.hpp"

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

namespace ct::detail {

wgpu::Surface CreateWindowNativeSurface(const wgpu::Instance& instance, const Window& window) {
    wgpu::SurfaceDescriptor surfaceDesc{};
    wgpu::Surface surface{nullptr};

#ifndef __EMSCRIPTEN__
    auto win = static_cast<GLFWwindow*>(window.GetNativeHandle());

    switch (glfwGetPlatform()) {

#if defined(GLFW_EXPOSE_NATIVE_X11)
    case GLFW_PLATFORM_X11: {
        log::Info("Creating X11 surface");
        void* xDisplay = reinterpret_cast<void*>(glfwGetX11Display());
        uint64_t xWindow = static_cast<uint64_t>(glfwGetX11Window(win));
        if (xDisplay && xWindow != 0) {
            wgpu::SurfaceSourceXlibWindow xlibSource{};
            xlibSource.display = xDisplay;
            xlibSource.window = xWindow;
            surfaceDesc.nextInChain = &xlibSource;
            surface = instance.CreateSurface(&surfaceDesc);
        }
    } break;
#endif

#if defined(GLFW_EXPOSE_NATIVE_WAYLAND)
    case GLFW_PLATFORM_WAYLAND: {
        log::Info("Creating Wayland surface");
        void* wlDisplay = reinterpret_cast<void*>(glfwGetWaylandDisplay());
        void* wlSurface = reinterpret_cast<void*>(glfwGetWaylandWindow(win));
        if (wlDisplay && wlSurface) {
            wgpu::SurfaceSourceWaylandSurface waylandSource{};
            waylandSource.display = wlDisplay;
            waylandSource.surface = wlSurface;
            surfaceDesc.nextInChain = &waylandSource;
            surface = instance.CreateSurface(&surfaceDesc);
        }
    } break;
#endif

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
    case GLFW_PLATFORM_WIN32: {
        log::Info("Creating Win32 surface");
        HWND hwnd = glfwGetWin32Window(win);
        HINSTANCE hinstance = GetModuleHandle(NULL);
        if (hwnd && hinstance) {
            wgpu::SurfaceSourceWindowsHWND win32Source{};
            win32Source.hinstance = hinstance;
            win32Source.hwnd = hwnd;
            surfaceDesc.nextInChain = &win32Source;
            surface = instance.CreateSurface(&surfaceDesc);
        }
    } break;
#endif

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
    case GLFW_PLATFORM_COCOA: {
        log::Info("Creating Cocoa surface");
        id metalLayer = [CAMetalLayer layer];
        NSWindow* nsWindow = glfwGetCocoaWindow(win);
        [nsWindow.contentView setWantsLayer:YES];
        [nsWindow.contentView setLayer:metalLayer];

        wgpu::SurfaceSourceMetalLayer cocoaSource{};
        cocoaSource.layer = metalLayer;
        surfaceDesc.nextInChain = &cocoaSource;
        surface = instance.CreateSurface(&surfaceDesc);
    } break;
#endif

    default:
        log::Critical("Unsupported platform for surface creation");
        break;
    }

#else
    log::Info("Creating Emscripten canvas surface");
    wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector canvasSource{};
    canvasSource.selector = {"canvas", 6};
    surfaceDesc.nextInChain = &canvasSource;
    surface = instance.CreateSurface(&surfaceDesc);
#endif

    if (!surface) {
        log::Critical("Failed to create native WebGPU surface");
    }

    return surface;
}

} // namespace ct::detail
