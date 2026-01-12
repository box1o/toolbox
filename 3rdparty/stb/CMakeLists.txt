cmake_minimum_required(VERSION 4.2.0)

project(stb
    VERSION 1.0.0
    DESCRIPTION "stb_image single-file library"
    LANGUAGES C
)

add_library(stb STATIC stb_image.c stb_image_write.c)
add_library(stb::stb ALIAS stb)

target_include_directories(stb
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(stb PUBLIC c_std_99)

set_target_properties(stb PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME stb
    EXPORT_NAME stb
    POSITION_INDEPENDENT_CODE ON
)
