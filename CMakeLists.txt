cmake_minimum_required(VERSION 3.26)

include(${CMAKE_SOURCE_DIR}/cmake/pre_check.cmake)
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
DO_CHECK()

project(atlas LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# include cmake file
include(${CMAKE_SOURCE_DIR}/cmake/detect_platform.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/atlas_build_target.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/import_asan.cmake)

# compile option
option(SHARED               "build with dynamic link"                   ON)
option(UNICODE              "build with UNICODE string"                 ON)
option(WITH_EDITOR          "build with game editor"                    ON)
option(WITH_TEST            "build with unit test"                      OFF)
option(WITH_BENCHMARK       "build with benchmark"                      OFF)
option(ASAN_ENABLED         "enable address sanitizer"                  OFF)

# redirect output directory
set(CMAKE_DEBUG_POSTFIX "d")
set(ENGINE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ENGINE_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ENGINE_OUTPUT_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ENGINE_OUTPUT_DIRECTORY}/bin)

if(SHARED)
    add_compile_definitions(AE_SHARED)
endif()

if(UNICODE)
    add_compile_definitions(UNICODE)
endif()

if(WITH_EDITOR)
    add_compile_definitions(WITH_EDITOR=1)
endif()

if (WITH_CRASH_HANDLER)
    add_compile_definitions(WITH_CRASH_HANDLER=1)
endif()

if (MSVC)
    if (SHARED)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else ()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif ()

    # handle the utf-8 encoding problem in MSVC
    add_compile_options(/utf-8)

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /GL")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
    add_compile_definitions(_HAS_EXCEPTIONS=0)

    if(ASAN_ENABLED)
        IMPORT_ASAN()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        add_compile_definitions(_DISABLE_VECTOR_ANNOTATION)
    endif()
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")
endif ()



if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_compile_definitions(DEBUG)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    add_compile_definitions(DEBUG_OPTIMIZE)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_compile_definitions(RELEASE)
endif()

if(WIN32)
    add_compile_definitions(WIN32)
    add_compile_definitions(_WIN32)
endif()

## set absolute path
set(ATLAS_TOOL_DIRECTORY ${CMAKE_SOURCE_DIR}/tools)

## run build tool

set(EXECUTABLE_PATHS "")

file(GLOB_RECURSE SUB_PATH LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/tools/AtlasBuilder/bin/Release/*")
foreach(PATH ${SUB_PATH})
    if(IS_DIRECTORY ${PATH})
        list(APPEND EXECUTABLE_PATHS ${PATH})
    endif()
endforeach()

find_program(ATLAS_BUILDER NAMES AtlasBuilder PATHS ${EXECUTABLE_PATHS} NO_DEFAULT_PATH REQUIRED)

execute_process(
        COMMAND ${ATLAS_BUILDER} --package_manager=Vcpkg
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULTS_VARIABLE EXIT_CODE
        OUTPUT_VARIABLE OUTPUT
)

if (EXIT_CODE)
    message("${OUTPUT}")
    message(FATAL_ERROR "Atlas: some errors occurred while running atlas builder. msg: ${EXIT_CODE}")
else ()
    message("${OUTPUT}")
endif ()

add_subdirectory(src)

if (WITH_BENCHMARK)
    add_subdirectory(benchmark)
endif ()

if (WITH_TEST)
    add_subdirectory(test)
endif ()

add_subdirectory(examples)