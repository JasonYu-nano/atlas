add_atlas_executable(
        TARGET launcher
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB engine
        PLATFORM_CODE_DIRS src
)