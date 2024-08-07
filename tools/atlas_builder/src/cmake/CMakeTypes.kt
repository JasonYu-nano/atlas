/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

package com.spacelion.atlas_builder.cmake

class CMakeTypes private constructor() {
    companion object {
        val EDITOR_ONLY = "EDITOR_ONLY"
        val TEST_TARGET = "TEST_TARGET"
        val GAME_TARGET = "GAME_TARGET"
        val TARGET = "TARGET"
        val PUBLIC_DEFINITIONS = "PUBLIC_DEFINITIONS"
        val PRIVATE_DEFINITIONS = "PRIVATE_DEFINITIONS"
        val PUBLIC_INCLUDE_DIRS = "PUBLIC_INCLUDE_DIRS"
        val PRIVATE_INCLUDE_DIRS = "PRIVATE_INCLUDE_DIRS"
        val PUBLIC_LINK_LIB = "PUBLIC_LINK_LIB"
        val PRIVATE_LINK_LIB = "PRIVATE_LINK_LIB"
        val DEPENDENCY_TARGETS = "DEPENDENCY_TARGETS"
        val PLATFORM_CODE_DIRS = "PLATFORM_CODE_DIRS"

        val OPTIONS = setOf(EDITOR_ONLY, TEST_TARGET, GAME_TARGET)
        val ONE_VALUE_ARGS = setOf(TARGET)
        val MULTI_VALUE_ARGS = setOf(PUBLIC_DEFINITIONS, PRIVATE_DEFINITIONS, PUBLIC_INCLUDE_DIRS,
            PRIVATE_INCLUDE_DIRS, PUBLIC_LINK_LIB, PRIVATE_LINK_LIB, DEPENDENCY_TARGETS, PLATFORM_CODE_DIRS)

        val ADD_LIBRARY_FUNCTION = "add_atlas_library"
        val ADD_EXECUTABLE_FUNCTION = "add_atlas_executable"
        val ADD_PROJECT_FUNCTION = "add_atlas_project"
    }
}