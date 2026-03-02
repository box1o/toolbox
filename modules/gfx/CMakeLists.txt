cmake_minimum_required(VERSION 4.2.0)

project(gfx
    VERSION 1.0.0
    DESCRIPTION "Graphics module for rendering"
    LANGUAGES CXX
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/backend/window/*.hpp"
)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/backend/window/*.cpp"
)

set(USE_WEBGPU ON CACHE BOOL "Enable WebGPU backend")

if(USE_WEBGPU)
  file(GLOB_RECURSE TMP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/backend/webgpu/*.hpp")
  list(APPEND HEADERS ${TMP})

  file(GLOB_RECURSE TMP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/backend/webgpu/*.cpp")
  list(APPEND SOURCES ${TMP})

  unset(TMP)
endif()

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
            ${BACKEND_SOURCES}
    HEADERS ${HEADERS}
            ${BACKEND_HEADERS}
    DEPENDENCIES ${GFX_DEPS}
)

target_compile_definitions(${namespace}_gfx PUBLIC
    $<$<BOOL:${USE_WEBGPU}>:USE_WEBGPU_BACKEND>
)

if(EMSCRIPTEN)
    target_compile_definitions(${namespace}_gfx PUBLIC WEBGPU_BACKEND_EMSCRIPTEN)
    target_compile_options(${namespace}_gfx PUBLIC
        "--use-port=${_WGPU_PORT}"
    )
    target_link_options(${namespace}_gfx PUBLIC
        "--use-port=${_WGPU_PORT}"
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
    )
endif()
