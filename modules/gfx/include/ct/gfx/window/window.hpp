#pragma once

#include <functional>

#include "GLFW/glfw3.h"
#include "ct/base/types/types.hpp"

namespace ct::gfx {

struct WindowInfo{
    std::string title{"desktop window"};
    u32 height{600};
    u32 width{800};
    bool fullscreen{false};
    bool resizable{true};
    bool decorated{true};
};


enum class CursorMode : u8 { Normal, Hidden, Disabled };
enum class CursorType : u8 { Arrow, IBeam, Crosshair, Hand, HResize, VResize };

class Window{
public:
    using ResizeCallback = std::function<void(u32 width, u32 height)>;

    Window();
    ~Window() = default;

    // [[nodiscard]] virtual const std::string& GetTitle() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetWidth() const noexcept = 0;
    // [[nodiscard]] virtual u32 GetHeight() const noexcept = 0;
    // [[nodiscard]] virtual f32 GetAspectRatio() const noexcept = 0;
    //
    // [[nodiscard]] virtual bool IsFullScreen() const noexcept = 0;
    //
    // [[nodiscard]] virtual CursorMode GetCursorMode() const noexcept = 0;
    // virtual void SetCursorMode(CursorMode mode) noexcept = 0;
    //
    // [[nodiscard]] virtual CursorType GetCursorType() const noexcept = 0;
    // virtual void SetCursorType(CursorType type) noexcept = 0;
    //
    // [[nodiscard]] virtual void* GetNativeWindow() const noexcept = 0;
    //
    // [[nodiscard]] virtual bool ShouldClose() const noexcept = 0;
    // virtual void PollEvents() noexcept = 0;
    // virtual void Close() noexcept = 0;
    //
    // virtual void SetResizeCallback(ResizeCallback callback) = 0;



};



} 
