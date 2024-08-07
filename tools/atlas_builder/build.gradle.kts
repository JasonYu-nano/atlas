/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

plugins {
    id("buildlogic.kotlin-application-conventions")
}

sourceSets {
    main {
        kotlin {
            setSrcDirs(listOf("src"))
        }
    }
}

dependencies {
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core")
    implementation("org.slf4j:slf4j-simple")
    implementation("io.github.oshai:kotlin-logging-jvm")

    implementation(project(":tool_core"))
}

application {
    // Define the main class for the application.
    mainClass = "com.spacelion.atlas_builder.AtlasBuilderKt"
}