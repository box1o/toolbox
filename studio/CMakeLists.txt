cmake_minimum_required(VERSION 4.2.0)

set(STUDIO_NAME studio)

if(NOT DEFINED TARGET_VERSION)
    set(TARGET_VERSION 0.0.1)
    message(STATUS "TARGET_VERSION not defined, defaulting to ${TARGET_VERSION}")
endif()

project(${STUDIO_NAME}
    VERSION ${TARGET_VERSION}
    DESCRIPTION "studio application"
    LANGUAGES CXX
)

set(STUDIO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(STUDIO_RES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

file(GLOB_RECURSE STUDIO_HEADERS
    CONFIGURE_DEPENDS
    ${STUDIO_SRC_DIR}/*.h
    ${STUDIO_SRC_DIR}/*.hpp
)

file(GLOB_RECURSE STUDIO_SOURCES
    CONFIGURE_DEPENDS
    ${STUDIO_SRC_DIR}/*.cpp
)

add_executable(${STUDIO_NAME}
        ${STUDIO_HEADERS}
        ${STUDIO_SOURCES}
)

target_compile_features(${STUDIO_NAME} PRIVATE cxx_std_23)

ct_apply_compiler_options(${STUDIO_NAME})

target_compile_definitions(${STUDIO_NAME}
    PRIVATE
        STUDIO_VERSION="${PROJECT_VERSION}"
)

target_include_directories(${STUDIO_NAME}
    PRIVATE
        ${STUDIO_SRC_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)


target_link_libraries(${STUDIO_NAME}
    PRIVATE
    ct::base
)



set_target_properties(${STUDIO_NAME} PROPERTIES
        OUTPUT_NAME "studio"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(WIN32)
    set_target_properties(${STUDIO_NAME}
            PROPERTIES WIN32_EXECUTABLE ON
    )
elseif(APPLE)
    set_target_properties(${STUDIO_NAME}
            PROPERTIES MACOSX_BUNDLE ON
    )
endif()



install(TARGETS ${STUDIO_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(EXISTS "${STUDIO_RES_DIR}")
    install(
            DIRECTORY ${STUDIO_RES_DIR}/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/ct
    )

    add_custom_command(
            TARGET ${STUDIO_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${STUDIO_RES_DIR}
                    ${CMAKE_BINARY_DIR}/bin/resources
    )
endif()
