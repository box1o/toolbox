cmake_minimum_required(VERSION 4.2.0)

add_subdirectory(stb)

find_or_add(yaml
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
)

find_or_add(cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  CMAKE_CACHE_VARS
    "CXXOPTS_STANDALONE_PROJECT=OFF"
)


#NOTE: Graphics
if(NOT EMSCRIPTEN)
  find_or_add(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
  )

  find_or_add(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
    CMAKE_CACHE_VARS
        "SPDLOG_FMT_EXTERNAL=ON"
  )

  find_or_add(glfw
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG master
      DISABLE_INSTALL
  )

  find_or_add(Dawn
      GIT_REPOSITORY https://dawn.googlesource.com/dawn.git
      POST_FETCH_SCRIPT  tools/fetch_dawn_dependencies.py
      CMAKE_CACHE_VARS
          DAWN_ENABLE_BACKEND_VULKAN=ON
          DAWN_ENABLE_BACKEND_D3D11=OFF
          DAWN_ENABLE_BACKEND_D3D12=OFF
          DAWN_ENABLE_BACKEND_METAL=OFF
  )
else()
  message(STATUS "Skipping Dawn and GLFW")
endif()



# will try the wgsl language is not will go back to use siprv and glsl
# find_or_add(spirv-cross
#     GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
#     GIT_TAG main 
# )
# find_or_add(shaderc
#     GIT_REPOSITORY https://github.com/google/shaderc.git
#     GIT_TAG main
#     RECURSE_SUBMODULES
#     DISABLE_INSTALL
#     POST_FETCH_SCRIPT utils/git-sync-deps
#     CMAKE_CACHE_VARS
#         SHADERC_SKIP_TESTS=ON
#         SHADERC_SKIP_INSTALL=ON
#         SHADERC_SKIP_EXAMPLES=ON
#         SHADERC_SKIP_COPYRIGHT_CHECK=ON
# )


#NOTE: Math related
# find_or_add(sophus
#     GIT_REPOSITORY https://github.com/strasdat/Sophus.git
#     GIT_TAG "main"
# )
