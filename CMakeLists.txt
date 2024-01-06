cmake_minimum_required(VERSION 3.26)
project(atlas LANGUAGES CXX)

# for conan package manager
enable_language(C)

set(CMAKE_CXX_STANDARD 23)
set(dotnet_sdk_version "net7.0")

# compile option
option(shared           "build with dynamic link"                   ON)
option(unicode          "build with unicode string"                 ON)
option(with_test        "build with unit test"                      OFF)
option(with_benchmark   "build with benchmark"                      OFF)
option(with_ispc        "use ispc compiler to generate simd code"   OFF)

if(shared)
    add_compile_definitions(AE_SHARED)
endif()

if(unicode)
    add_compile_definitions(UNICODE)
endif()

if (MSVC)
    if (shared)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else ()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif ()

    # handle the utf-8 encoding problem in MSVC
    add_compile_options(/utf-8)
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /GL")

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_compile_definitions(DEBUG)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    add_compile_definitions(DEVELOPMENT)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_compile_definitions(RELEASE)
endif()

if(WIN32)
    add_compile_definitions(WIN32)
    add_compile_definitions(_WIN32)
endif()

# set output
set(CMAKE_DEBUG_POSTFIX "d")
set(output_dir ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_dir}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_dir}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_dir}/bin)

## set absolute path
set(atlas_tool_dir ${CMAKE_SOURCE_DIR}/tools)

include(${CMAKE_SOURCE_DIR}/cmake/atlas_build_target.cmake)

## run build tool
set(abt_exist 0)

set(atlas_build_tool_dir ${output_dir}/${dotnet_sdk_version})

if (WIN32)
    if (EXISTS ${atlas_build_tool_dir}/AtlasBuildTool.exe)
        set(abt_exist 1)
    endif ()
elseif (APPLE)
    if (EXISTS ${atlas_build_tool_dir}/AtlasBuildTool)
        set(abt_exist 1)
    endif ()
endif ()

if (abt_exist)
    execute_process(
            COMMAND ${atlas_build_tool_dir}/AtlasBuildTool
            RESULT_VARIABLE exit_code
    )

    if (exit_code)
        message(FATAL_ERROR "some errors occurred while running atlas build tool")
    endif ()
else ()
    message(FATAL_ERROR "can't find atlas build tool in ${atlas_build_tool_dir}. please make sure tools are built first")
endif ()

add_subdirectory(src)
add_subdirectory(benchmark)
if (with_test)
    add_subdirectory(test)
endif ()

message(${CMAKE_BINARY_DIR})