add_atlas_library(
        TARGET application
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB engine
        PLATFORM_CODE_DIRS "include/platform" "src/platform"
)