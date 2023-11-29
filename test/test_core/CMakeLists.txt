find_package(GTest)

add_atlas_executable(
    TARGET test_core
    PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_LINK_LIB GTest::gtest core
)