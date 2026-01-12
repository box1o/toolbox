cmake_minimum_required(VERSION 4.2.0)

add_subdirectory(stb)

ct_find_or_add(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

ct_find_or_add(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)



set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

ct_find_or_add(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
    DISABLE_INSTALL
)
