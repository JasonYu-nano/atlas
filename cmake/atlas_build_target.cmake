macro(_build_target_parse_arguments)
    set(options EDITOR_ONLY GAME_TARGET TEST_TARGET)
    set(one_value_args TARGET)
    set(multi_value_args PUBLIC_DEFINITIONS PRIVATE_DEFINITIONS PUBLIC_INCLUDE_DIRS PRIVATE_INCLUDE_DIRS
            PUBLIC_LINK_LIB PRIVATE_LINK_LIB DEPENDENCY_TARGETS PLATFORM_CODE_DIRS)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})
endmacro()

macro(_add_target_properties)
    if (ARG_EDITOR_ONLY)
        set_target_properties(${ARG_TARGET} PROPERTIES EDITOR_ONLY 1)
    else ()
        set_target_properties(${ARG_TARGET} PROPERTIES EDITOR_ONLY 0)
    endif ()
endmacro()

macro(_add_compile_definitions)
    if (ARG_PUBLIC_DEFINITIONS)
        foreach (PUBLIC_DEFINITION ${ARG_PUBLIC_DEFINITIONS})
            target_compile_definitions(${ARG_TARGET} PUBLIC ${PUBLIC_DEFINITION})
        endforeach ()
    endif ()

    if (ARG_PRIVATE_DEFINITIONS)
        foreach (PRIVATE_DEFINITION ${ARG_PRIVATE_DEFINITIONS})
            target_compile_definitions(${ARG_TARGET} PRIVATE ${PRIVATE_DEFINITION})
        endforeach ()
    endif ()
endmacro()

macro(_add_include_dirs)
    if (ARG_PUBLIC_INCLUDE_DIRS)
        foreach (INCLUDE_DIR ${ARG_PUBLIC_INCLUDE_DIRS})
            target_include_directories(${ARG_TARGET} PUBLIC ${INCLUDE_DIR})
        endforeach ()
    endif ()

    if (ARG_PRIVATE_INCLUDE_DIRS)
        foreach (INCLUDE_DIR ${ARG_PRIVATE_INCLUDE_DIRS})
            target_include_directories(${ARG_TARGET} PRIVATE ${INCLUDE_DIR})
        endforeach ()
    endif ()
    target_include_directories(${ARG_TARGET} PRIVATE "${CMAKE_SOURCE_DIR}/intermediate/build_targets/${ARG_TARGET}")
endmacro()

macro(_add_dependency)
    if (ARG_DEPENDENCY_TARGETS)
        foreach (TARGET ${ARG_DEPENDENCY_TARGETS})
            add_dependencies(${ARG_TARGET} ${TARGET})
        endforeach ()
    endif ()
endmacro()

function(_test_file_need_exclude FILE_PATH OUT_RESULT)
    set(RESULT "FALSE")

    foreach (DIR ${ARG_PLATFORM_CODE_DIRS})

        if (${FILE_PATH} MATCHES ${DIR})
            if (${PLATFORM_WINDOWS})
                if (${FILE_PATH} MATCHES "mac|linux")
                    set(RESULT "TRUE")
                endif ()

            elseif (${PLATFORM_APPLE})
                if (${FILE_PATH} MATCHES "windows|linux")
                    set(RESULT "TRUE")
                endif ()

            elseif (${PLATFORM_LINUX})
                if (${FILE_PATH} MATCHES "windows|mac")
                    set(RESULT "TRUE")
                endif ()

            endif ()

            break()
        endif ()

    endforeach ()

    set(${OUT_RESULT} ${RESULT} PARENT_SCOPE)
endfunction()

function(_exclude_platform_files FILES)
    set(TEMP_RESULT "")

    foreach(FILE_PATH ${${FILES}})
        _test_file_need_exclude(${FILE_PATH} NEED_EXCLUDE)
        if (NEED_EXCLUDE STREQUAL "FALSE")
            LIST(APPEND TEMP_RESULT ${FILE_PATH})
        else ()
            message(STATUS "Atlas: exclude platform file: ${FILE_PATH}")
        endif ()
    endforeach()

    set(${FILES} ${TEMP_RESULT} PARENT_SCOPE)
endfunction()

macro(_setup_ide)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MODULE_FILES})

    if (ARG_GAME_TARGET)
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "game")
    elseif (ARG_TEST_TARGET)
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "test")
    else ()
        set_target_properties(${ARG_TARGET} PROPERTIES FOLDER "atlas_engine")
    endif ()
endmacro()

macro(_add_link_libs)
    get_target_property(TARGET_EDITOR_ONLY ${ARG_TARGET} EDITOR_ONLY)

    if (ARG_PUBLIC_LINK_LIB)
        foreach (LINK_LIB ${ARG_PUBLIC_LINK_LIB})
            if (NOT TARGET_EDITOR_ONLY AND TARGET ${LINK_LIB})
                get_property(editor_only TARGET ${LINK_LIB} PROPERTY EDITOR_ONLY)
                if (editor_only)
                    message(FATAL_ERROR "Link an editor target to runtime target is not allowed")
                endif ()
            endif ()

            target_link_libraries(${ARG_TARGET} PUBLIC ${LINK_LIB})
        endforeach ()
    endif ()

    if (ARG_PRIVATE_LINK_LIB)
        foreach (LINK_LIB ${ARG_PRIVATE_LINK_LIB})
            if (NOT TARGET_EDITOR_ONLY AND TARGET ${LINK_LIB})
                get_property(editor_only TARGET ${LINK_LIB} PROPERTY EDITOR_ONLY)
                if (editor_only)
                    message(FATAL_ERROR "Link an editor target to runtime target is not allowed")
                endif ()
            endif ()

            target_link_libraries(${ARG_TARGET} PRIVATE ${LINK_LIB})
        endforeach ()
    endif ()
endmacro()

macro(_include_definition_file)
    set(DEFINITION_FILE "${CMAKE_SOURCE_DIR}/intermediate/build_targets/${ARG_TARGET}/${ARG_TARGET}_definitions.hpp")
    if (EXISTS ${DEFINITION_FILE})
        if(MSVC)
            add_definitions(/FI${DEFINITION_FILE})
        else()
            # GCC or Clang
            add_definitions(-include ${DEFINITION_FILE})
        endif()
    endif ()
endmacro()

macro(add_atlas_library)
    _build_target_parse_arguments(${ARGV})

    set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    if (${PLATFORM_APPLE})
        file(GLOB_RECURSE MODULE_FILES *.hpp *.cpp *.h *.mm)
    else ()
        file(GLOB_RECURSE MODULE_FILES *.hpp *.cpp)
    endif ()
    file(GLOB_RECURSE GENERATED_FILES "${CMAKE_SOURCE_DIR}/intermediate/build_targets/${ARG_TARGET}/*")

    _exclude_platform_files(MODULE_FILES)

    if(SHARED)
        add_library(${ARG_TARGET} SHARED ${MODULE_FILES} ${GENERATED_FILES})
    else()
        add_library(${ARG_TARGET} STATIC ${MODULE_FILES} ${GENERATED_FILES})
    endif()

    _add_target_properties()
    _add_compile_definitions()
    _add_include_dirs()
    _add_link_libs()
    _add_dependency()
    _include_definition_file()

    _setup_ide()
endmacro()

macro(add_atlas_executable)
    _build_target_parse_arguments(${ARGV})

    set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    if (${PLATFORM_APPLE})
        file(GLOB_RECURSE MODULE_FILES *.hpp *.cpp *.h *.mm)
    else ()
        file(GLOB_RECURSE MODULE_FILES *.hpp *.cpp)
    endif ()
    file(GLOB_RECURSE GENERATED_FILES "${CMAKE_SOURCE_DIR}/intermediate/build_targets/${ARG_TARGET}/*")

    _exclude_platform_files(MODULE_FILES)

    add_executable(${ARG_TARGET} ${MODULE_FILES} ${GENERATED_FILES})

    _add_target_properties()
    _add_compile_definitions()
    _add_include_dirs()
    _add_link_libs()
    _add_dependency()
    _include_definition_file()

    _setup_ide()
endmacro()

macro(add_atlas_project)
    _build_target_parse_arguments(${ARGV})

    set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

    file(GLOB_RECURSE MODULE_FILES *.hpp *.cpp)

    _exclude_platform_files(MODULE_FILES)

    if(SHARED)
        add_library(${ARG_TARGET} SHARED ${MODULE_FILES})
    else()
        add_library(${ARG_TARGET} STATIC ${MODULE_FILES})
    endif()

    _add_target_properties()
    _add_compile_definitions()
    _add_include_dirs()
    _add_link_libs()
    _add_dependency()
    _include_definition_file()

    if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(BUILD_DIR "${MODULE_DIR}/build/debug")
    elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
        set(BUILD_DIR "${MODULE_DIR}/build/debug_optimize")
    elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
        set(BUILD_DIR "${MODULE_DIR}/build/release")
    endif()

    set_target_properties(${ARG_TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/out/lib
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/out/bin
        LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/out/bin
    )

    _setup_ide()
endmacro()