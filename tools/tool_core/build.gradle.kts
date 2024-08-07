/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

plugins {
    id("buildlogic.kotlin-library-conventions")
}

sourceSets {
    main {
        kotlin {
            setSrcDirs(listOf("src"))
        }
    }
}

dependencies {
    implementation("org.slf4j:slf4j-simple")
    implementation("io.github.oshai:kotlin-logging-jvm")
}
