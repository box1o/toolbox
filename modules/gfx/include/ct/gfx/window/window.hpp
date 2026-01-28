#pragma once

#include <ct/base/base.hpp>
#include <string>


namespace ct::gfx {


struct WindowInfo {
    std::string title{"Window"};
    u32 width{800};
    u32 height{600};
    bool fullscreen{false};
    bool resizable{true};
    bool decorated{true};
};

class Window {
public:
    virtual ~Window() = default;


    [[nodiscard]] virtual void* GetWindowHandle() const noexcept = 0;

    [[nodiscard]] virtual bool ShouldClose() const noexcept = 0;
    virtual void PollEvents() const noexcept = 0;



    [[nodiscard]] static ref<Window> Create(const WindowInfo& info = {});

protected:
    Window() = default;
};

}
