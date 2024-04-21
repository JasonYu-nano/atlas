find_package(spdlog REQUIRED)
find_package(Boost COMPONENTS locale REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(nlohmann_json REQUIRED)

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

if (${PLATFORM_APPLE})
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(core PUBLIC ${COCOA_LIBRARY})
endif ()