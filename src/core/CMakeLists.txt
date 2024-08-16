find_package(spdlog CONFIG REQUIRED)
find_package(boost_locale REQUIRED CONFIG)

find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

find_package(nlohmann_json REQUIRED)

add_atlas_library(
        TARGET core
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB spdlog::spdlog Boost::locale PkgConfig::tomlplusplus nlohmann_json::nlohmann_json
        PLATFORM_CODE_DIRS "include/platform" "src/platform"
)

if (WITH_EDITOR)
    target_compile_definitions(core PUBLIC NAME_PRESERVING_CASE_SENSITIVE=1)
endif ()

if (${PLATFORM_APPLE})
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(core PUBLIC ${COCOA_LIBRARY})
endif ()