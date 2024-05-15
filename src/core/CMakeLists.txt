find_package(spdlog)
find_package(Boost)
find_package(tomlplusplus)
find_package(nlohmann_json)

add_atlas_library(
        TARGET core
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB spdlog::spdlog Boost::locale tomlplusplus::tomlplusplus nlohmann_json::nlohmann_json
        PLATFORM_CODE_DIRS "include/platform" "src/platform"
)

if (WITH_EDITOR)
    target_compile_definitions(core PUBLIC NAME_PRESERVING_CASE_SENSITIVE=1)
endif ()