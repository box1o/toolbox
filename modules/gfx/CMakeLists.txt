cmake_minimum_required(VERSION 4.2.0)

project(ct_gfx
    VERSION 1.0.0
    DESCRIPTION "All graphics related functionality"
    LANGUAGES CXX
)

file(GLOB_RECURSE HEADERS
    include/*.hpp
    backends/*.hpp
)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    backends/*.cpp
)



set(GFX_DEPS
    stb::stb

    ct::base
    glfw
)


find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    list(APPEND GFX_DEPS Vulkan::Vulkan)
    message(STATUS "Vulkan found, adding to ct_gfx dependencies")
else()
    message(FATAL_ERROR "Vulkan not found, but is required for ct_gfx module")
endif()

add_ct_module(gfx
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDENCIES ${GFX_DEPS}
)

