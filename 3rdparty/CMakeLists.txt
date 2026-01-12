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


