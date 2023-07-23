cmake_minimum_required(VERSION 3.16)

project(utf8_with_msvc LANGUAGES C CXX)

add_executable(utf8_with_msvc
    main.cpp
    $<$<C_COMPILER_ID:MSVC>: project/utf8_with_msvc.manifest>
)

target_compile_options(utf8_with_msvc PUBLIC
    $<$<C_COMPILER_ID:MSVC>:/utf-8>
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)
