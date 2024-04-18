add_atlas_executable(
        TARGET launcher
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB engine
        PLATFORM_CODE_DIRS src
)

if (${PLATFORM_APPLE})
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(launcher PRIVATE ${COCOA_LIBRARY})
endif ()