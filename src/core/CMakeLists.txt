find_package(spdlog)
find_package(Boost)
find_package(tomlplusplus)

add_atlas_library(
        TARGET core
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB spdlog::spdlog Boost::locale tomlplusplus::tomlplusplus
        PLATFORM_CODE_DIRS "include/platform" "src/platform"
)

if (with_editor)
    target_compile_definitions(core PUBLIC NAME_PRESERVING_CASE_SENSITIVE=1)
endif ()