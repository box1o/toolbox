#include "swapchain.hpp"
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/window/window.hpp"

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

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

// X11 macro cleanup (avoid collisions with your enums)
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

namespace ct::gfx::detail {

wgpu::Surface CreateWindowNativeSurface(ref<Device> device, ref<Window> window) {
    wgpu::SurfaceDescriptor surfaceDesc{};
    wgpu::Surface surface{nullptr};

#ifdef __EMSCRIPTEN__
    log::Info("Creating Emscripten canvas surface");
    wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector canvasSource{};
    canvasSource.selector = {"canvas", 6};
    surfaceDesc.nextInChain = &canvasSource;
    surface = instance.CreateSurface(&surfaceDesc);

    if (!surface) log::Critical("Failed to create Emscripten WebGPU surface");
    return surface;
#else
    if (!window) {
        log::Critical("CreateWindowNativeSurface: window is null");
        return {};
    }

    auto* glfwWin = static_cast<GLFWwindow*>(window->GetNativeHandle());
    if (!glfwWin) {
        log::Critical("CreateWindowNativeSurface: GLFW native handle is null");
        return {};
    }

    auto* instance = (wgpu::Instance*)device->GetInstance();
    if (!instance) {
        log::Critical("CreateWindowNativeSurface: device instance is null");
        return {};
    }

    const int platform = glfwGetPlatform();
    switch (platform) {

#if defined(GLFW_EXPOSE_NATIVE_X11)
    case GLFW_PLATFORM_X11: {
        log::Info("Creating X11 surface");
        void* xDisplay = reinterpret_cast<void*>(glfwGetX11Display());
        const uint64_t xWindow = static_cast<uint64_t>(glfwGetX11Window(glfwWin));

        if (xDisplay && xWindow != 0) {
            wgpu::SurfaceSourceXlibWindow xlibSource{};
            xlibSource.display = xDisplay;
            xlibSource.window = xWindow;
            surfaceDesc.nextInChain = &xlibSource;
            surface = instance->CreateSurface(&surfaceDesc);
        }
    } break;
#endif

#if defined(GLFW_EXPOSE_NATIVE_WAYLAND)
    case GLFW_PLATFORM_WAYLAND: {
        log::Info("Creating Wayland surface");
        void* wlDisplay = reinterpret_cast<void*>(glfwGetWaylandDisplay());
        void* wlSurface = reinterpret_cast<void*>(glfwGetWaylandWindow(glfwWin));

        if (wlDisplay && wlSurface) {
            wgpu::SurfaceSourceWaylandSurface waylandSource{};
            waylandSource.display = wlDisplay;
            waylandSource.surface = wlSurface;
            surfaceDesc.nextInChain = &waylandSource;
            surface = instance->CreateSurface(&surfaceDesc);
        }
    } break;
#endif

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
    case GLFW_PLATFORM_WIN32: {
        log::Info("Creating Win32 surface");
        HWND hwnd = glfwGetWin32Window(glfwWin);
        HINSTANCE hinstance = GetModuleHandle(NULL);

        if (hwnd && hinstance) {
            wgpu::SurfaceSourceWindowsHWND win32Source{};
            win32Source.hinstance = hinstance;
            win32Source.hwnd = hwnd;
            surfaceDesc.nextInChain = &win32Source;
            surface = instance->CreateSurface(&surfaceDesc);
        }
    } break;
#endif

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
    case GLFW_PLATFORM_COCOA: {
        log::Info("Creating Cocoa/Metal surface");

        id metalLayer = [CAMetalLayer layer];
        NSWindow* nsWindow = glfwGetCocoaWindow(glfwWin);
        if (!nsWindow) break;

        [nsWindow.contentView setWantsLayer:YES];
        [nsWindow.contentView setLayer:metalLayer];

        wgpu::SurfaceSourceMetalLayer cocoaSource{};
        cocoaSource.layer = metalLayer;
        surfaceDesc.nextInChain = &cocoaSource;
        surface = instance->CreateSurface(&surfaceDesc);
    } break;
#endif

    default:
        log::Critical(
            "Unsupported GLFW platform for surface creation (platform id = {})", platform);
        break;
    }

    if (!surface) {
        log::Critical("Failed to create native WebGPU surface");
    }

    return surface;
#endif
}

} // namespace ct::gfx::detail
