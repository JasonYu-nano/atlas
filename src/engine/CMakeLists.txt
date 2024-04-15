add_atlas_library(
        TARGET engine
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB core
)