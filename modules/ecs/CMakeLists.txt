cmake_minimum_required(VERSION 4.2.0)

project(ecs
    VERSION 1.0.0
    DESCRIPTION "entity component system"
    LANGUAGES CXX
)

file(GLOB_RECURSE HEADERS
    include/*.hpp
)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

set(ECS_DEPS
    toolbox::base
)

add_module(ecs
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDENCIES ${ECS_DEPS}
)
