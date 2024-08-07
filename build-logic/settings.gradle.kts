/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

dependencyResolutionManagement {
    // Reuse version catalog from the main build.
    versionCatalogs {
        create("libs", { from(files("../gradle/libs.versions.toml")) })
    }
}

rootProject.name = "build-logic"
