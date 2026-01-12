cmake_minimum_required(VERSION 4.2.0)

project(ct_base
    VERSION 1.0.0
    DESCRIPTION "Core functionality: types, logging, error handling"
    LANGUAGES CXX
)

file(GLOB_RECURSE HEADERS
    include/*.hpp
)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)


add_ct_module(base
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDENCIES fmt spdlog
)

