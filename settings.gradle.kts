/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

pluginManagement {
    // Include 'plugins build' to define convention plugins.
    includeBuild("build-logic")
    plugins {
        kotlin("jvm") version "2.0.0"
    }
}

plugins {
}

rootProject.name = "atlas"
include("tool_core")
project(":tool_core").projectDir = file("tools/tool_core")

include("atlas_builder")
project(":atlas_builder").projectDir = file("tools/atlas_builder")
