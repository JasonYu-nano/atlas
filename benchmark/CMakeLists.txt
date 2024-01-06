find_package(benchmark)
add_atlas_executable(
        TARGET benchmark
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_LINK_LIB benchmark::benchmark core
)