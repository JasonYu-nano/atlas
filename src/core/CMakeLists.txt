find_package(spdlog)
find_package(Boost)

add_atlas_library(
        TARGET core
        PRIVATE_DEFINITIONS CORE_EXPORT
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB spdlog::spdlog Boost::locale
)