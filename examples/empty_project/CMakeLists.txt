add_atlas_project(
        TARGET empty_project
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PRIVATE_LINK_LIB rhi
)