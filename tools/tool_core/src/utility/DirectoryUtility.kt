/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

package com.spacelion.tool_core.utility

import io.github.oshai.kotlinlogging.KotlinLogging
import java.io.File

val logger = KotlinLogging.logger {}

class DirectoryUtility private constructor() {
    companion object {
        val ENGINE_ROOT_DIRECTORY = getEngineRootDirectory()
        val ENGINE_INTERMEDIATE_DIRECTORY = "$ENGINE_ROOT_DIRECTORY${File.separatorChar}intermediate"
        val ENGINE_SOURCE_DIRECTORY = "$ENGINE_ROOT_DIRECTORY${File.separatorChar}src"
        val BUILD_TARGET_INTERMEDIATE_DIRECTORY = "$ENGINE_INTERMEDIATE_DIRECTORY${File.separatorChar}build_targets";

        private fun getEngineRootDirectory() : String {
            val rootName = "${File.separatorChar}atlas"
            val workDir = System.getProperty("user.dir")
            logger.debug { "Current work directory is $workDir" }

            val index = workDir.indexOf(rootName)
            check(index != -1)

            val rootDir = workDir.removeRange(index + rootName.length, workDir.length)
            check(File(rootDir).exists())

            logger.debug { "Engine root directory is $rootDir" }

            return rootDir
        }
    }
}