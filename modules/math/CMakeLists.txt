cmake_minimum_required(VERSION 4.2.0)

project(ct_math
    VERSION 1.0.0
    DESCRIPTION "Mathematics and linear algebra module for ct"
    LANGUAGES CXX
)

file(GLOB_RECURSE HEADERS
    include/*.hpp
)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)


add_ct_module(math
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
)

