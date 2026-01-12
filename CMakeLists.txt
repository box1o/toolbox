cmake_minimum_required(VERSION 4.2.0)

project(ct VERSION 0.0.1 LANGUAGES C CXX)

# Options
option(CT_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(CT_PLATFORM_DESKTOP "Build for desktop platforms" OFF)
option(CT_PLATFORM_ANDROID "Build for Android" OFF)
option(CT_PLATFORM_IOS     "Build for iOS" OFF)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CTCOptions)
include(CTDependencies)
include(CTModule)


message(STATUS "")
message(STATUS "╔═══════════════════════════════════════════════════════════════╗")
message(STATUS "║                ct Configuration Summary                       ║")
message(STATUS "╚═══════════════════════════════════════════════════════════════╝")
message(STATUS "  Version:              ${PROJECT_VERSION}")
message(STATUS "  Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler:             ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  C++ standard:         C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

add_subdirectory(3rdparty)



set(MODULES_DIR "${CMAKE_SOURCE_DIR}/modules")
file(GLOB CHILD_FOLDERS RELATIVE "${MODULES_DIR}" "${MODULES_DIR}/*")

foreach(FOLDER ${CHILD_FOLDERS})
    if(IS_DIRECTORY "${MODULES_DIR}/${FOLDER}")
        message(STATUS "Module ../${FOLDER}")
        add_subdirectory("${MODULES_DIR}/${FOLDER}")
    endif()
endforeach()



add_subdirectory(studio)
