add_atlas_library(
        TARGET rhi
        PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl
        PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC_LINK_LIB application
        PLATFORM_CODE_DIRS "include/opengl/platform" "src/opengl/platform"
)

find_package(OpenGL COMPONENTS OpenGL)

if (OpenGL_OpenGL_FOUND)
    message(STATUS "Atlas: OpenGL found")
    target_link_libraries(rhi PUBLIC OpenGL::GL)
endif ()
