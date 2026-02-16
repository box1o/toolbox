cmake_minimum_required(VERSION 4.2.0)

project(gfx
    VERSION 1.0.0
    DESCRIPTION "Graphics module for rendering"
    LANGUAGES CXX
)

file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

set(GFX_DEPS
    toolbox::base
    toolbox::math
    yaml-cpp::yaml-cpp
    stb::stb
)

if(NOT EMSCRIPTEN)
    list(APPEND GFX_DEPS
        glfw
        webgpu_dawn
    )
else()
    if(EMDAWNWEBGPU_PORT STREQUAL "")
        set(_WGPU_PORT "emdawnwebgpu")
        message(STATUS "using emscripten emdawnwebgpu")
    else()
        set(_WGPU_PORT "${EMDAWNWEBGPU_PORT}")
        message(STATUS "using external emdawnwebgpu port: ${_WGPU_PORT}")
    endif()
endif()

add_module(gfx
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDENCIES ${GFX_DEPS}
)

if(EMSCRIPTEN)
    target_compile_definitions(${namespace}_gfx PUBLIC WEBGPU_BACKEND_EMSCRIPTEN)
    target_compile_options(${namespace}_gfx PUBLIC
        "--use-port=${_WGPU_PORT}"
        "-sUSE_GLFW=3"
    )
    target_link_options(${namespace}_gfx PUBLIC
        "--use-port=${_WGPU_PORT}"
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
    )
endif()
