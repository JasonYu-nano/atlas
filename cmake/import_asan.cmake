# import asan dll to binary directory
function(IMPORT_ASAN)
    if (MSVC)
        message(STATUS "Checking for ASAN runtime DLL.")

        # get the MSVC compiler path
        get_filename_component(COMPILER_DIR ${CMAKE_C_COMPILER} DIRECTORY)

        # construct the full path of the ASAN dll
        set(ASAN_DLL "${COMPILER_DIR}/clang_rt.asan_dbg_dynamic-x86_64.dll")
        message(STATUS "Looking for ASAN DLL at: ${ASAN_DLL}")

        # 检查 DLL 是否存在
        if (EXISTS ${ASAN_DLL})
            message(STATUS "Found ASAN DLL: ${ASAN_DLL}")

            file(COPY ${ASAN_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            message(STATUS "Copied ASAN DLL to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        else()
            message(FATAL_ERROR "ASAN runtime DLL not found: ${ASAN_DLL}")
        endif()
    endif()
endfunction()